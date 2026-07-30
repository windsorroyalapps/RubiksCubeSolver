#include "CenterSolver.h"
#include "ClusterScheduler.h"
#include "BatchGroups.h"

#include <algorithm>

std::vector<Move> CenterSolver::commutator(const Move& a, const Move& b) {
    Move aInv{a.face, a.depth, a.turns == 2 ? 2 : -a.turns};
    Move bInv{b.face, b.depth, b.turns == 2 ? 2 : -b.turns};
    return {a, b, aInv, bInv};
}

static int centerScoreFace(const Cube& work, int face) {
    int n = work.size();
    Color target = static_cast<Color>(face);
    int correct = 0, total = 0;
    for (int r = 1; r < n - 1; ++r) {
        for (int c = 1; c < n - 1; ++c) {
            ++total;
            if (work.get(face, r, c) == target) ++correct;
        }
    }
    return total == 0 ? 100 : (correct * 100) / total;
}

// Sum of all six face center scores (0..600). Used to reject sequences that
// improve one face while wrecking others — key "never break solved cells" guard.
static int centerScoreGlobal(const Cube& work) {
    int s = 0;
    for (int f = 0; f < 6; ++f) s += centerScoreFace(work, f);
    return s;
}

static int opposite(int face) {
    switch (face) {
        case U: return D; case D: return U;
        case F: return B; case B: return F;
        case L: return R; case R: return L;
        default: return face;
    }
}

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

static void undoSeq(Cube& work, const std::vector<Move>& seq) {
    for (auto it = seq.rbegin(); it != seq.rend(); ++it)
        work.apply(Move{it->face, it->depth, it->turns == 2 ? 2 : -it->turns});
}

std::vector<Move> CenterSolver::solveFace(Cube& work, int face) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4) return moves;

    // Already solid? Never touch.
    if (centerScoreFace(work, face) >= 100) return moves;

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

    // More attempts for larger n; still bounded so mobile stays responsive.
    int maxAttempts = 48 + 8 * (n - 4);
    if (maxAttempts > 120) maxAttempts = 120;

    for (int attempt = 0; attempt < maxAttempts && centerScoreFace(work, face) < 100; ++attempt) {
        int bestGain = -1;
        int bestGlobalDelta = -999;
        std::vector<Move> bestSeq;
        int scoreBefore = centerScoreFace(work, face);
        int globalBefore = centerScoreGlobal(work);

        auto trySeq = [&](const std::vector<Move>& seq) {
            for (const auto& m : seq) work.apply(m);
            int faceAfter = centerScoreFace(work, face);
            int globalAfter = centerScoreGlobal(work);
            undoSeq(work, seq);
            int gain = faceAfter - scoreBefore;
            int gDelta = globalAfter - globalBefore;
            // Accept only if target face improves (or holds at high score) AND
            // global score does not drop — protects already-solved faces.
            if (gain > bestGain && gDelta >= 0) {
                bestGain = gain;
                bestGlobalDelta = gDelta;
                bestSeq = seq;
            } else if (gain == bestGain && gDelta > bestGlobalDelta) {
                bestGlobalDelta = gDelta;
                bestSeq = seq;
            }
        };

        for (int depth = 1; depth < maxDepth; ++depth) {
            for (int turns : {1, 2, -1}) {
                {
                    Move faceTurn{face, 0, turns};
                    Move slice{opp, depth, 1};
                    trySeq(commutator(faceTurn, slice));
                }
                for (int ai = 0; ai < 4; ++ai) {
                    Move faceTurn{face, 0, turns};
                    Move slice{adj[ai], depth, 1};
                    trySeq(commutator(faceTurn, slice));
                    // Also try slice as the outer of the commutator for variety
                    trySeq(commutator(slice, faceTurn));
                }
            }
        }

        if (bestGain > 0 && !bestSeq.empty()) {
            append(bestSeq);
        } else {
            // Safe fallback: outer turn only (does not change relative center layout much)
            append({Move{face, 0, 1}});
        }

        if (centerScoreFace(work, face) >= 100) break;
    }
    return moves;
}

std::vector<Move> CenterSolver::solve(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4) return solution;

    auto append = [&](const std::vector<Move>& seq) {
        solution.insert(solution.end(), seq.begin(), seq.end());
    };

    // Phase A: BatchGroups — one commutator per shared-move group (Demaine spirit)
    for (int pass = 0; pass < 4; ++pass) {
        auto groups = BatchGroups::fromCube(work);
        if (groups.empty()) break;
        auto seq = BatchGroups::applyAll(work, groups);
        if (seq.empty()) break;
        append(seq);
        // Early exit if centers already perfect
        if (centerScoreGlobal(work) >= 600) return solution;
    }

    // Phase B: score-guided face cleanup with never-break global guard
    // Order: opposite pairs first (U/D then F/B then L/R) so mid-slice conflicts reduce
    const int order[] = {U, D, F, B, L, R};
    for (int face : order) {
        append(solveFace(work, face));
    }

    // Phase C: one more BatchGroups pass if any residual (rare)
    if (centerScoreGlobal(work) < 600) {
        auto groups = BatchGroups::fromCube(work);
        auto seq = BatchGroups::applyAll(work, groups);
        append(seq);
        for (int face : order) {
            if (centerScoreFace(work, face) < 100)
                append(solveFace(work, face));
        }
    }

    return solution;
}
