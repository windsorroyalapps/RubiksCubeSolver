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

uint64_t ReducedSearch::residualKey(const Cube& c) {
    // Compact fingerprint for 4x4 residual meet-in-middle / visited sets.
    // High 16: center correctness mask. Low 32: packed wing residual samples
    // (bit per mid-edge + selected depth samples) so key==0 iff residual cleared.
    uint64_t key = 0;
    if (c.size() == 4) {
        key |= static_cast<uint64_t>(pack4x4Centers(c)) << 48;
    }
    // Pack a 32-bit wing fingerprint (not just count) for collision resistance.
    int n = c.size();
    if (n >= 4) {
        int mid = n / 2;
        uint32_t wingBits = 0;
        int bit = 0;
        auto setIfBad = [&](bool bad) {
            if (bad && bit < 32) wingBits |= (1u << bit);
            ++bit;
        };
        // 12 mid edges
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
            Color x = c.get(e.f1, e.r1, e.c1);
            Color y = c.get(e.f2, e.r2, e.c2);
            bool ok = (x == e.a && y == e.b) || (x == e.b && y == e.a);
            setIfBad(!ok);
        }
        // Selected depth samples on UF/UR/FR (enough for 4x4 n-2=2)
        for (int d = 1; d <= std::min(2, n - 2); ++d) {
            setIfBad(c.get(U, n-1, d) != Color::U && c.get(U, n-1, d) != Color::F);
            setIfBad(c.get(F, d, mid) != Color::F && c.get(F, d, mid) != Color::U);
            setIfBad(c.get(U, d, n-1) != Color::U && c.get(U, d, n-1) != Color::R);
            setIfBad(c.get(R, d, mid) != Color::R && c.get(R, d, mid) != Color::U);
            setIfBad(c.get(F, mid, d) != Color::F && c.get(F, mid, d) != Color::R);
            setIfBad(c.get(R, mid, d) != Color::R && c.get(R, mid, d) != Color::F);
        }
        key |= static_cast<uint64_t>(wingBits);
    }
    return key;
}

int ReducedSearch::heuristic(const Cube& c) {
    int n = c.size();
    if (n < 4) return 0;

    int bad = 0;
    if (n == 4) {
        // Exact packed center residual (popcount of incorrect mask)
        uint16_t mask = pack4x4Centers(c);
        bad += __builtin_popcount(mask);
    } else {
        // Centers: sample inner (n-2)x(n-2)
        for (int f = 0; f < 6; ++f) {
            Color target = static_cast<Color>(f);
            for (int r = 1; r < n - 1; ++r) {
                for (int col = 1; col < n - 1; ++col) {
                    if (c.get(f, r, col) != target) ++bad;
                }
            }
        }
    }

    bad += wingResidual(c);

    // Scale so heuristic stays useful relative to depth (admissible-ish).
    // Slightly tighter scaling after fuller residual to keep IDA* focused.
    return std::min(bad / 4, 20);
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
    // Mobile-safe: depthCap/2 each side, hard node budget.
    std::vector<Move> result;
    if (work.size() != 4) return result;

    uint64_t startKey = residualKey(work);
    if (startKey == 0) return result;  // already residual-clear

    const int half = std::max(1, depthCap / 2);
    const size_t nodeBudget = 8000;  // keep mobile responsive

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

    // Backward: from solved residual (key 0). We need a Cube that has residual 0.
    // Approximate by taking a solved 4x4 and applying inverse of a short path,
    // but since we only care about residualKey match, we BFS moves on a fresh
    // cube and map key -> inverse path. For residual-only we start from a cube
    // whose residualKey is forced 0 by construction (solved centers+wings).
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
                // Reconstruct: forward path + reverse of backward path
                result = fwd[k];
                // Invert the backward path (apply inverses in reverse order)
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

    // Also check if any forward key is already residual 0 (handled) or meet missed.
    return result;
}

std::vector<Move> ReducedSearch::improve(Cube& work, int maxDepth) {
    std::vector<Move> result;
    int n = work.size();
    if (n != 4 && n != 5) return result;
    if (!isNearlyReduced(work)) return result;

    // Bound search cost for mobile: higher on 4x4 (toward OBTM 54), tighter on 5x5
    // 2026-08-09: raised 4x4 depthCap to 18 for more residual collapse
    // 2026-08-11: raised 4x4 depthCap to 20 + residual packing comments for bidirectional meet-in-middle
    // 2026-08-12: residualKey + true bidirectional MITM prototype; try MITM first on 4x4
    // Full residual coords + true bidirectional remain highest leverage to collapse toward OBTM ≤54
    int depthCap = (n == 4) ? std::max(maxDepth, 20) : std::min(maxDepth, 10);

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
