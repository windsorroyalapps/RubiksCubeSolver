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

// Adjacent faces useful for cross-axis center transfers
static void adjacentFaces(int face, int& a1, int& a2, int& a3, int& a4) {
    switch (face) {
        case U: a1 = F; a2 = R; a3 = B; a4 = L; break;
        case D: a1 = F; a2 = L; a3 = B; a4 = R; break;
        case F: a1 = U; a2 = R; a3 = D; a4 = L; break;
        case B: a1 = U; a2 = L; a3 = D; a4 = R; break;
        case L: a1 = U; a2 = F; a3 = D; a4 = B; break;
        case R: a1 = U; a2 = B; a3 = D; a4 = F; break;
        default: a1 = a2 = a3 = a4 = face; break;
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
    int adj[4];
    adjacentFaces(face, adj[0], adj[1], adj[2], adj[3]);

    // Goal: push centerScore to 100 (or >= 95 fallback)
    for (int attempt = 0; attempt < 48 && centerScore(work, face) < 100; ++attempt) {
        int bestGain = -1;
        std::vector<Move> bestSeq;
        int scoreBefore = centerScore(work, face);

        // Trial set: opposite-slice + adjacent-slice commutators at every inner depth
        for (int depth = 1; depth < maxDepth; ++depth) {
            // 1) classic opposite-slice
            {
                Move faceTurn{face, 0, 1};
                Move slice{opp, depth, 1};
                auto seq = commutator(faceTurn, slice);
                for (auto& m : seq) work.apply(m);
                int gain = centerScore(work, face) - scoreBefore;
                for (auto it = seq.rbegin(); it != seq.rend(); ++it) {
                    Move inv{it->face, it->depth, it->turns == 2 ? 2 : -it->turns};
                    work.apply(inv);
                }
                if (gain > bestGain) {
                    bestGain = gain;
                    bestSeq = seq;
                }
            }
            // 2) adjacent-slice transfers (helps belt faces especially)
            for (int ai = 0; ai < 4; ++ai) {
                Move faceTurn{face, 0, 1};
                Move slice{adj[ai], depth, 1};
                auto seq = commutator(faceTurn, slice);
                for (auto& m : seq) work.apply(m);
                int gain = centerScore(work, face) - scoreBefore;
                for (auto it = seq.rbegin(); it != seq.rend(); ++it) {
                    Move inv{it->face, it->depth, it->turns == 2 ? 2 : -it->turns};
                    work.apply(inv);
                }
                if (gain > bestGain) {
                    bestGain = gain;
                    bestSeq = seq;
                }
            }
        }

        if (bestGain > 0 && !bestSeq.empty()) {
            append(bestSeq);
        } else {
            // Reorient face to change alignment when no progressive commutator found
            append({Move{face, 0, 1}});
        }

        if (centerScore(work, face) >= 100) break;
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
