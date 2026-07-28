#include "CenterSolver.h"

#include <algorithm>

std::vector<Move> CenterSolver::commutator(const Move& a, const Move& b) {
    // A B A' B'
    Move aInv{a.face, a.depth, a.turns == 2 ? 2 : -a.turns};
    Move bInv{b.face, b.depth, b.turns == 2 ? 2 : -b.turns};
    return {a, b, aInv, bInv};
}

std::vector<Move> CenterSolver::solveFace(Cube& work, int face) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4) return moves;

    Color target = static_cast<Color>(face);

    // For each non-center facelet on this face, try to bring correct color in.
    // Strategy: use outer-layer turns + inner-slice commutators.
    // This is a practical reduction approach, not optimal.

    auto append = [&](const std::vector<Move>& seq) {
        for (const auto& m : seq) {
            work.apply(m);
            moves.push_back(m);
        }
    };

    // Sweep inner rows/cols with slice moves to gather correct colors
    // depth 1 .. n/2-1 are the movable center bands
    int maxDepth = n / 2;
    for (int depth = 1; depth < maxDepth; ++depth) {
        // Rotate this face and adjacent sides to funnel pieces
        for (int t = 0; t < 4; ++t) {
            // Check if center band already mostly correct
            int correct = 0;
            int total = 0;
            for (int r = depth; r < n - depth; ++r) {
                for (int c = depth; c < n - depth; ++c) {
                    if (r == n/2 && c == n/2 && (n % 2 == 1)) continue; // fixed center
                    ++total;
                    if (work.get(face, r, c) == target) ++correct;
                }
            }
            if (total > 0 && correct * 100 / total >= 80) break;

            // Apply a gathering sequence: turn face, slice, turn back
            Move faceTurn{face, 0, 1};
            // Opposite face for slice context
            int opp = (face == U) ? D : (face == D) ? U :
                      (face == F) ? B : (face == B) ? F :
                      (face == L) ? R : L;
            Move slice{opp, depth, 1};

            append(commutator(faceTurn, slice));
            append({Move{face, 0, 1}}); // align next
        }
    }

    // Final outer alignment
    for (int i = 0; i < 4; ++i) {
        append({Move{face, 0, 1}});
    }

    return moves;
}

std::vector<Move> CenterSolver::solve(Cube& work) {
    std::vector<Move> solution;
    int n = work.size();
    if (n < 4) return solution;

    // Solve centers in order: U, D, then sides F R B L
    // (standard reduction order reduces disruption)
    const int order[] = {U, D, F, R, B, L};

    for (int face : order) {
        auto stage = solveFace(work, face);
        solution.insert(solution.end(), stage.begin(), stage.end());
    }

    return solution;
}
