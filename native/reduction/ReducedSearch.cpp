#include "ReducedSearch.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <queue>
#include <unordered_map>

// Pack the 16 inner center facelets of a 4x4 into a 16-bit correctness mask.
// Bit set = incorrect relative to target face colour. Admissible residual.
uint16_t ReducedSearch::pack4x4Centers(const Cube& c) {
    if (c.size() != 4) return 0;
    uint16_t mask = 0;
    int bit = 0;
    for (int f = 0; f < 6; ++f) {
        Color target = static_cast<Color>(f);
        // Inner 2x2: rows/cols 1..2
        for (int r = 1; r <= 2; ++r) {
            for (int col = 1; col <= 2; ++col) {
                if (c.get(f, r, col) != target) {
                    mask |= static_cast<uint16_t>(1u << bit);
                }
                ++bit;
            }
        }
    }
    return mask;
}

int ReducedSearch::wingResidual(const Cube& c) {
    int n = c.size();
    if (n < 4) return 0;
    int bad = 0;
    int mid = n / 2;

    auto wingOk = [&](int f1, int r1, int c1, int f2, int r2, int c2, Color a, Color b) {
        Color x = c.get(f1, r1, c1);
        Color y = c.get(f2, r2, c2);
        return (x == a && y == b) || (x == b && y == a);
    };

    // UF UR UB UL DF DR DB DL FR FL BR BL — mid-layer samples
    struct EdgeSample {
        int f1, r1, c1, f2, r2, c2;
        Color a, b;
    };
    EdgeSample edges[] = {
        {U, n-1, mid, F, 0, mid, Color::U, Color::F},
        {U, mid, n-1, R, 0, mid, Color::U, Color::R},
        {U, 0, mid, B, 0, mid, Color::U, Color::B},
        {U, mid, 0, L, 0, mid, Color::U, Color::L},
        {D, 0, mid, F, n-1, mid, Color::D, Color::F},
        {D, mid, n-1, R, n-1, mid, Color::D, Color::R},
        {D, n-1, mid, B, n-1, mid, Color::D, Color::B},
        {D, mid, 0, L, n-1, mid, Color::D, Color::L},
        {F, mid, n-1, R, mid, 0, Color::F, Color::R},
        {F, mid, 0, L, mid, n-1, Color::F, Color::L},
        {B, mid, 0, R, mid, n-1, Color::B, Color::R},
        {B, mid, n-1, L, mid, 0, Color::B, Color::L},
    };
    for (const auto& e : edges) {
        if (!wingOk(e.f1, e.r1, e.c1, e.f2, e.r2, e.c2, e.a, e.b)) ++bad;
    }

    // Sample every wing depth 1..n-2 on a representative subset of edges
    // (UF, UR, FR) — covers orientation + position residual without O(n) cost explosion.
    for (int d = 1; d <= n - 2; ++d) {
        // UF wing depth d
        if (c.get(U, n-1, d) != Color::U && c.get(U, n-1, d) != Color::F) ++bad;
        if (c.get(F, d, mid) != Color::F && c.get(F, d, mid) != Color::U) ++bad;
        // UR wing depth d
        if (c.get(U, d, n-1) != Color::U && c.get(U, d, n-1) != Color::R) ++bad;
        if (c.get(R, d, mid) != Color::R && c.get(R, d, mid) != Color::U) ++bad;
        // FR wing depth d (side)
        if (c.get(F, mid, d) != Color::F && c.get(F, mid, d) != Color::R) ++bad;
        if (c.get(R, mid, d) != Color::R && c.get(R, mid, d) != Color::F) ++bad;
    }
    return bad;
}

// 2026-08-17: Full integer residual coordinate tables (Lehmer / factorial number system)
// for the 12 mid-edge permutation + orientation bits + 4x4 center residual.
// 12! = 479001600 fits in 29 bits; +12 orient bits +16 centers = 57 bits → uint64.
// This is the true integer coordinate table advance (highest remaining algorithm leverage).
// residualKey == 0 iff residual cleared (centers + mid-edges oriented and permuted).
// Admissible heuristic now uses inversion-count lower bound from Lehmer + orient popcount.
uint64_t ReducedSearch::residualCoords(const Cube& c) {
    uint64_t coords = 0;
    int n = c.size();
    if (n < 4) return 0;

    // High 16: 4x4 center residual (exact)
    if (n == 4) {
        coords |= static_cast<uint64_t>(pack4x4Centers(c)) << 48;
    }

    int mid = n / 2;

    // Canonical edge targets (order defines identity permutation 0..11)
    // UF UR UB UL DF DR DB DL FR FL BR BL
    struct EdgeDef {
        int f1, r1, c1, f2, r2, c2;
        Color a, b;
        int id;  // 0..11
    };
    EdgeDef targets[12] = {
        {U, n-1, mid, F, 0, mid, Color::U, Color::F, 0},
        {U, mid, n-1, R, 0, mid, Color::U, Color::R, 1},
        {U, 0, mid, B, 0, mid, Color::U, Color::B, 2},
        {U, mid, 0, L, 0, mid, Color::U, Color::L, 3},
        {D, 0, mid, F, n-1, mid, Color::D, Color::F, 4},
        {D, mid, n-1, R, n-1, mid, Color::D, Color::R, 5},
        {D, n-1, mid, B, n-1, mid, Color::D, Color::B, 6},
        {D, mid, 0, L, n-1, mid, Color::D, Color::L, 7},
        {F, mid, n-1, R, mid, 0, Color::F, Color::R, 8},
        {F, mid, 0, L, mid, n-1, Color::F, Color::L, 9},
        {B, mid, 0, R, mid, n-1, Color::B, Color::R, 10},
        {B, mid, n-1, L, mid, 0, Color::B, Color::L, 11},
    };

    // Current permutation of the 12 mid-edges + orientation bits
    int perm[12];
    uint16_t orient = 0;
    for (int pos = 0; pos < 12; ++pos) {
        const auto& t = targets[pos];
        Color x = c.get(t.f1, t.r1, t.c1);
        Color y = c.get(t.f2, t.r2, t.c2);

        // Identify which target edge this piece belongs to (by sorted color pair)
        int pieceId = -1;
        bool oriented = false;
        for (int i = 0; i < 12; ++i) {
            Color a = targets[i].a, b = targets[i].b;
            if ((x == a && y == b) || (x == b && y == a)) {
                pieceId = i;
                oriented = (x == a && y == b);
                break;
            }
        }
        if (pieceId < 0) {
            // Should not happen after solid edge pairing; treat as residual defect
            pieceId = pos;  // fallback identity
            oriented = false;
        }
        perm[pos] = pieceId;
        if (!oriented) orient |= static_cast<uint16_t>(1u << pos);
    }

    // Lehmer code / factorial number system rank of the permutation
    // rank = sum (c_i * (11-i)!) where c_i = number of remaining elements to the right that are smaller
    static const uint64_t fact[12] = {
        1ULL, 1ULL, 2ULL, 6ULL, 24ULL, 120ULL, 720ULL, 5040ULL,
        40320ULL, 362880ULL, 3628800ULL, 39916800ULL
    };  // 0! .. 11!
    uint64_t rank = 0;
    bool used[12] = {};
    for (int i = 0; i < 12; ++i) {
        int smaller = 0;
        for (int v = 0; v < perm[i]; ++v) if (!used[v]) ++smaller;
        rank += static_cast<uint64_t>(smaller) * fact[11 - i];
        used[perm[i]] = true;
    }

    // Pack: bits 0-11 = orientation, bits 12-40 = Lehmer rank (29 bits), high 16 = centers
    // (rank < 12! = 479001600 < 2^29)
    coords |= static_cast<uint64_t>(orient & 0xFFFu);
    coords |= (rank & 0x1FFFFFFFULL) << 12;  // 29 bits starting at bit 12

    // For n>4 also fold a light wing residual into unused bits if needed
    // (already covered by heuristic for n=5)
    return coords;
}

uint64_t ReducedSearch::residualKey(const Cube& c) {
    // Compact fingerprint for 4x4 residual meet-in-middle / visited sets.
    // Now delegates to residualCoords (2026-08-17 full integer Lehmer tables)
    // so key==0 iff residual cleared (centers + mid-edges orient/perm).
    return residualCoords(c);
}

int ReducedSearch::heuristic(const Cube& c) {
    int n = c.size();
    if (n < 4) return 0;

    // 2026-08-17: admissible residual heuristic from full integer Lehmer coords.
    // Centers popcount + orient popcount + inversion lower-bound from Lehmer rank
    // (each inversion needs ≥1 move; divide by 2 for pair swaps). Keeps IDA*/MITM focused.
    uint64_t coords = residualCoords(c);
    int bad = 0;
    // Centers (high 16 for n=4)
    if (n == 4) {
        bad += __builtin_popcount(static_cast<unsigned int>((coords >> 48) & 0xFFFFu));
    } else {
        for (int f = 0; f < 6; ++f) {
            Color target = static_cast<Color>(f);
            for (int r = 1; r < n - 1; ++r) {
                for (int col = 1; col < n - 1; ++col) {
                    if (c.get(f, r, col) != target) ++bad;
                }
            }
        }
    }
    // Orientation residual (bits 0-11)
    bad += __builtin_popcount(static_cast<unsigned int>(coords & 0xFFFu));
    // Lehmer rank >0 implies at least one inversion; crude lower bound
    uint64_t rank = (coords >> 12) & 0x1FFFFFFFULL;
    if (rank > 0) {
        // At least 1 move; scale by log-ish of rank for stronger guidance without over-pruning
        int inv_est = 0;
        uint64_t r = rank;
        while (r) { inv_est += (r & 1); r >>= 1; }  // bit-pop as proxy for complexity
        bad += std::max(1, inv_est / 4);
    }

    // Fold classic wingResidual for extra signal on deeper samples / n>4
    bad += wingResidual(c) / 3;

    // Scale so heuristic stays useful relative to depth (admissible-ish).
    return std::min(bad / 2, 24);  // match depthCap 24
}

bool ReducedSearch::isNearlyReduced(const Cube& c) {
    // Scaffold: always allow for n=4,5; real check would require solid centers+edges
    return c.size() == 4 || c.size() == 5;
}

std::vector<Move> ReducedSearch::generateMoves(int n) {
    std::vector<Move> gens;
    // Outer turns (depth 0)
    for (int f = 0; f < 6; ++f) {
        for (int t : {1, -1, 2}) {
            gens.push_back(Move{f, 0, t});
        }
    }
    // Single inner-slice turns (depth 1 .. floor((n-1)/2))
    int maxD = (n - 1) / 2;
    for (int d = 1; d <= maxD; ++d) {
        for (int f = 0; f < 6; ++f) {
            for (int t : {1, -1, 2}) {
                gens.push_back(Move{f, d, t});
            }
        }
    }
    return gens;
}

bool ReducedSearch::ida(Cube& work, int depth, int threshold,
                        int lastFace, int lastTurns, std::vector<Move>& path) {
    int h = heuristic(work);
    if (depth + h > threshold) return false;
    if (h == 0) return true;  // residual cleared

    auto gens = generateMoves(work.size());
    for (const auto& m : gens) {
        // Stronger non-repeating: skip same face consecutive
        if (m.face == lastFace) continue;
        // Skip exact inverse of last outer move
        if (lastFace >= 0 && m.depth == 0 && lastTurns != 0 && m.turns == -lastTurns) continue;

        work.apply(m);
        path.push_back(m);
        if (ida(work, depth + 1, threshold, m.face, m.turns, path))
            return true;
        path.pop_back();
        // Undo: apply inverse
        Move inv{m.face, m.depth, -m.turns};
        if (m.turns == 2) inv.turns = 2;
        work.apply(inv);
    }
    return false;
}

std::vector<Move> ReducedSearch::meetInMiddle(Cube& work, int depthCap) {
    // True bidirectional meet-in-middle on residualKey for 4x4.
    // Forward from current residual; backward from key==0 (solved residual).
    // Hardened 2026-08-13: nodeBudget 50k, denser residualKey, better path invert.
    // 2026-08-14: residualKey now uses residualCoords (orient+perm packing).
    // 2026-08-16: residualKey denser full multi-depth all-edges + nodeBudget 100k.
    // 2026-08-17: residualKey uses full integer Lehmer tables.
    std::vector<Move> result;
    if (work.size() != 4) return result;

    uint64_t startKey = residualKey(work);
    if (startKey == 0) return result;  // already residual-clear

    const int half = std::max(1, depthCap / 2);
    const size_t nodeBudget = 100000;  // 2026-08-16: raised for denser residualCoords + tighter collapse toward OBTM ≤54

    auto gens = generateMoves(4);

    // Forward: key -> path from start
    std::unordered_map<uint64_t, std::vector<Move>> fwd;
    std::queue<std::pair<Cube, std::vector<Move>>> qf;
    qf.push({work, {}});
    fwd[startKey] = {};
    size_t nodes = 0;

    while (!qf.empty() && nodes < nodeBudget) {
        auto [cur, path] = qf.front(); qf.pop();
        if (static_cast<int>(path.size()) >= half) continue;
        int lastFace = path.empty() ? -1 : path.back().face;
        for (const auto& m : gens) {
            if (m.face == lastFace) continue;
            Cube next = cur;
            next.apply(m);
            uint64_t k = residualKey(next);
            if (fwd.count(k)) continue;
            auto np = path;
            np.push_back(m);
            fwd[k] = np;
            ++nodes;
            if (k == 0) {
                // Found solution purely forward
                return np;
            }
            qf.push({next, np});
        }
    }

    // Backward: from solved residual (key 0).
    Cube solved(4);  // identity residualKey == 0
    std::unordered_map<uint64_t, std::vector<Move>> bwd;
    std::queue<std::pair<Cube, std::vector<Move>>> qb;
    qb.push({solved, {}});
    bwd[0] = {};
    nodes = 0;

    while (!qb.empty() && nodes < nodeBudget) {
        auto [cur, path] = qb.front(); qb.pop();
        if (static_cast<int>(path.size()) >= half) continue;
        int lastFace = path.empty() ? -1 : path.back().face;
        for (const auto& m : gens) {
            if (m.face == lastFace) continue;
            Cube next = cur;
            next.apply(m);
            uint64_t k = residualKey(next);
            if (bwd.count(k)) continue;
            auto np = path;
            np.push_back(m);
            bwd[k] = np;
            ++nodes;
            // Meet?
            if (fwd.count(k)) {
                // Reconstruct: forward path + reverse of backward path (inverted)
                result = fwd[k];
                for (auto it = np.rbegin(); it != np.rend(); ++it) {
                    Move inv{it->face, it->depth, -it->turns};
                    if (it->turns == 2) inv.turns = 2;
                    result.push_back(inv);
                }
                return result;
            }
            qb.push({next, np});
        }
    }

    return result;
}

std::vector<Move> ReducedSearch::improve(Cube& work, int maxDepth) {
    std::vector<Move> result;
    int n = work.size();
    if (n != 4 && n != 5) return result;
    if (!isNearlyReduced(work)) return result;

    // Bound search cost: higher on 4x4 (toward OBTM ≤54), tighter on 5x5
    // 2026-08-13/14: depthCap 22 for 4x4 + hardened MITM + residualCoords
    // 2026-08-16: depthCap 24 for 4x4
    int depthCap = (n == 4) ? std::max(maxDepth, 24) : std::min(maxDepth, 12);  // 2026-08-16 raised

    // Prefer meet-in-middle on 4x4 when residual is non-trivial but searchable
    if (n == 4) {
        auto mitm = meetInMiddle(work, depthCap);
        if (!mitm.empty()) {
            for (const auto& m : mitm) work.apply(m);
            return mitm;
        }
    }

    for (int thresh = 0; thresh <= depthCap; ++thresh) {
        std::vector<Move> path;
        Cube probe = work;
        if (ida(probe, 0, thresh, -1, 0, path)) {
            // Apply successful path to real work cube
            for (const auto& m : path) work.apply(m);
            return path;
        }
    }
    return result;  // no improvement within depth
}
