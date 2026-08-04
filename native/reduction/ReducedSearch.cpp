#include "ReducedSearch.h"

#include <algorithm>
#include <climits>

// Lightweight residual score: count incorrect center cells + unpaired wings.
// Admissible enough for scaffold; full packing later.
int ReducedSearch::heuristic(const Cube& c) {
    int n = c.size();
    if (n < 4) return 0;

    int bad = 0;
    // Centers: sample inner (n-2)x(n-2) of each face
    for (int f = 0; f < 6; ++f) {
        Color target = static_cast<Color>(f);
        for (int r = 1; r < n - 1; ++r) {
            for (int col = 1; col < n - 1; ++col) {
                if (c.get(f, r, col) != target) ++bad;
            }
        }
    }
    // Rough wing residual: edges not fully solid cost ~1 each
    // (full pairedWings would need EdgePairing; keep scaffold light)
    // Use mid-edge color match as proxy
    int mid = n / 2;
    auto midMatch = [&](int f1, int r1, int c1, int f2, int r2, int c2, Color a, Color b) {
        Color x = c.get(f1, r1, c1);
        Color y = c.get(f2, r2, c2);
        return (x == a && y == b) || (x == b && y == a);
    };
    // UF UR UB UL DF DR DB DL FR FL BR BL mid checks
    if (!midMatch(U, n-1, mid, F, 0, mid, Color::U, Color::F)) ++bad;
    if (!midMatch(U, mid, n-1, R, 0, mid, Color::U, Color::R)) ++bad;
    if (!midMatch(U, 0, mid, B, 0, mid, Color::U, Color::B)) ++bad;
    if (!midMatch(U, mid, 0, L, 0, mid, Color::U, Color::L)) ++bad;
    if (!midMatch(D, 0, mid, F, n-1, mid, Color::D, Color::F)) ++bad;
    if (!midMatch(D, mid, n-1, R, n-1, mid, Color::D, Color::R)) ++bad;
    if (!midMatch(D, n-1, mid, B, n-1, mid, Color::D, Color::B)) ++bad;
    if (!midMatch(D, mid, 0, L, n-1, mid, Color::D, Color::L)) ++bad;
    if (!midMatch(F, mid, n-1, R, mid, 0, Color::F, Color::R)) ++bad;
    if (!midMatch(F, mid, 0, L, mid, n-1, Color::F, Color::L)) ++bad;
    if (!midMatch(B, mid, 0, R, mid, n-1, Color::B, Color::R)) ++bad;
    if (!midMatch(B, mid, n-1, L, mid, 0, Color::B, Color::L)) ++bad;

    // Scale down so heuristic stays small relative to depth
    return std::min(bad / 4, 20);
}

bool ReducedSearch::isNearlyReduced(const Cube& c) {
    // Scaffold: always true for n=4,5; real check would require solid centers+edges
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
    int maxDepth = (n - 1) / 2;
    for (int d = 1; d <= maxDepth; ++d) {
        for (int f = 0; f < 6; ++f) {
            for (int t : {1, -1, 2}) {
                gens.push_back(Move{f, d, t});
            }
        }
    }
    return gens;
}

bool ReducedSearch::ida(Cube& work, int depth, int threshold,
                        int lastFace, std::vector<Move>& path) {
    int h = heuristic(work);
    if (depth + h > threshold) return false;
    if (h == 0) return true;  // residual cleared

    auto gens = generateMoves(work.size());
    for (const auto& m : gens) {
        // Simple non-repeating face filter
        if (m.face == lastFace) continue;

        work.apply(m);
        path.push_back(m);
        if (ida(work, depth + 1, threshold, m.face, path))
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

    // Bound search cost for mobile: tighter on 5x5
    int depthCap = (n == 4) ? maxDepth : std::min(maxDepth, 8);

    for (int thresh = 0; thresh <= depthCap; ++thresh) {
        std::vector<Move> path;
        Cube probe = work;
        if (ida(probe, 0, thresh, -1, path)) {
            // Apply successful path to real work cube
            for (const auto& m : path) work.apply(m);
            return path;
        }
    }
    return result;  // no improvement within depth
}
