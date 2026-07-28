#include "CenterSolver.h"

#include <algorithm>

std::vector<Move> CenterSolver::commutator(const Move& a, const Move& b) {
    Move aInv{a.face, a.depth, a.turns == 2 ? 2 : -a.turns};
    Move bInv{b.face, b.depth, b.turns == 2 ? 2 : -b.turns};
    return {a, b, aInv, bInv};
}

static int centerScore(const Cube& work, int face) {
    int n = work.size();
    Color target = static_cast<Color>(face);
    int correct = 0, total = 0;
    int start = 1, end = n - 1; // ignore outer frame (edges/corners)
    for (int r = start; r < end; ++r) {
        for (int c = start; c < end; ++c) {
            ++total;
            if (work.get(face, r, c) == target) ++correct;
        }
    }
    return total == 0 ? 100 : (correct * 100) / total;
}

static int opposite(int face) {
    switch (face) {
        case U: return D; case D: return U;
        case F: return B; case B: return F;
        case L: return R; case R: return L;
        default: return face;
    }
}

std::vector<Move> CenterSolver::solveFace(Cube& work, int face) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4) return moves;

    auto append = [&](const std::vector<Move>& seq) {
        for (const auto& m : seq) {
            work.apply(m);
            moves.push_back(m);
        }
    };

    int maxDepth = n / 2;
    int opp = opposite(face);

    // Goal: push centerScore to >= 95
    for (int attempt = 0; attempt < 24 && centerScore(work, face) < 95; ++attempt) {
        int bestDepth = 1;
        int bestGain = -1;
        int scoreBefore = centerScore(work, face);

        // Try each inner depth and pick the commutator that improves most
        for (int depth = 1; depth < maxDepth; ++depth) {
            Move faceTurn{face, 0, 1};
            Move slice{opp, depth, 1};
            auto seq = commutator(faceTurn, slice);

            // Speculatively apply
            for (auto& m : seq) work.apply(m);
            int scoreAfter = centerScore(work, face);
            int gain = scoreAfter - scoreBefore;

            // Undo
            for (auto it = seq.rbegin(); it != seq.rend(); ++it) {
                Move inv{it->face, it->depth, it->turns == 2 ? 2 : -it->turns};
                work.apply(inv);
            }

            if (gain > bestGain) {
                bestGain = gain;
                bestDepth = depth;
            }
        }

        // Apply best (or rotate face to change alignment if no gain)
        if (bestGain > 0) {
            Move faceTurn{face, 0, 1};
            Move slice{opp, bestDepth, 1};
            append(commutator(faceTurn, slice));
        } else {
            append({Move{face, 0, 1}}); // reorient and try again
        }
    }

    return moves;
}

std::vector<Move> CenterSolver::solve(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4) return solution;

    // U/D first (stable axis), then belt faces
    const int order[] = {U, D, F, B, L, R};
    for (int face : order) {
        auto stage = solveFace(work, face);
        solution.insert(solution.end(), stage.begin(), stage.end());
    }
    return solution;
}
