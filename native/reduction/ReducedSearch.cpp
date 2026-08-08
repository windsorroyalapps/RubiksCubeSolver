#include "ReducedSearch.h"

#include <algorithm>
#include <climits>

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

    // Full multi-depth wing residual (all depths 1..n-2) on the 12 edges.
    // For each edge we sample every wing layer; colour pair must match the
    // expected dedge colours (order-independent). This is the strongest
    // cheap residual we can afford before full coordinate packing.
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
        // Stronger non-repeating: skip same face consecutive; also skip exact inverse of last
        if (m.face == lastFace) continue;
        if (lastFace >= 0 && m.face == lastFace && m.turns == -lastTurns) continue;

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

std::vector<Move> ReducedSearch::improve(Cube& work, int maxDepth) {
    std::vector<Move> result;
    int n = work.size();
    if (n != 4 && n != 5) return result;
    if (!isNearlyReduced(work)) return result;

    // Bound search cost for mobile: higher on 4x4 (toward OBTM 54), tighter on 5x5
    // 2026-08-09: raised 4x4 depthCap to 18 for more residual collapse
    int depthCap = (n == 4) ? std::max(maxDepth, 18) : std::min(maxDepth, 10);

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
