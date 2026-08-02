#include "CenterSolver.h"
#include "ClusterScheduler.h"
#include "BatchGroups.h"

#include <algorithm>
#include <queue>
#include <set>
#include <tuple>

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

// Count absolute incorrect center cells (for residual targeting).
static int incorrectCenters(const Cube& work) {
    int n = work.size();
    int bad = 0;
    for (int f = 0; f < 6; ++f) {
        Color target = static_cast<Color>(f);
        for (int r = 1; r < n - 1; ++r)
            for (int c = 1; c < n - 1; ++c)
                if (work.get(f, r, c) != target) ++bad;
    }
    return bad;
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
    int maxAttempts = 64 + 12 * (n - 4);
    if (maxAttempts > 160) maxAttempts = 160;

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
                    trySeq(commutator(slice, faceTurn));
                    // 2-turn slice variants for more orbit coverage
                    Move slice2{opp, depth, 2};
                    trySeq(commutator(faceTurn, slice2));
                    trySeq(commutator(slice2, faceTurn));
                }
                for (int ai = 0; ai < 4; ++ai) {
                    Move faceTurn{face, 0, turns};
                    Move slice{adj[ai], depth, 1};
                    trySeq(commutator(faceTurn, slice));
                    trySeq(commutator(slice, faceTurn));
                    Move slice2{adj[ai], depth, 2};
                    trySeq(commutator(faceTurn, slice2));
                    trySeq(commutator(slice2, faceTurn));
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

// Residual short-search for n<=6: try short sequences that reduce absolute incorrect
// center cells while never dropping global score. Acts as a light BFS-style cleanup
// when greedy face passes leave a few cells wrong. Keeps mobile responsive (depth <=3).
static std::vector<Move> residualShortSearch(Cube& work) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4 || n > 6) return moves;
    if (centerScoreGlobal(work) >= 600) return moves;

    auto append = [&](const std::vector<Move>& seq) {
        for (const auto& m : seq) {
            work.apply(m);
            moves.push_back(m);
        }
    };

    int maxRounds = (n == 4) ? 24 : (n == 5 ? 18 : 12);
    int maxDepth = n / 2;

    for (int round = 0; round < maxRounds && incorrectCenters(work) > 0; ++round) {
        int badBefore = incorrectCenters(work);
        int globalBefore = centerScoreGlobal(work);
        int bestDelta = 0;
        std::vector<Move> bestSeq;

        auto trySeq = [&](const std::vector<Move>& seq) {
            for (const auto& m : seq) work.apply(m);
            int badAfter = incorrectCenters(work);
            int globalAfter = centerScoreGlobal(work);
            undoSeq(work, seq);
            int delta = badBefore - badAfter;
            if (delta > bestDelta && globalAfter >= globalBefore) {
                bestDelta = delta;
                bestSeq = seq;
            }
        };

        // Generate short candidate sequences (commutators + single outer turns)
        for (int face = 0; face < 6; ++face) {
            int opp = opposite(face);
            int adj[4];
            adjacentFaces(face, adj[0], adj[1], adj[2], adj[3]);
            for (int depth = 1; depth < maxDepth; ++depth) {
                for (int turns : {1, 2, -1}) {
                    Move ft{face, 0, turns};
                    Move sl{opp, depth, 1};
                    trySeq(CenterSolver::commutator(ft, sl));
                    trySeq(CenterSolver::commutator(sl, ft));
                    for (int ai = 0; ai < 4; ++ai) {
                        Move sla{adj[ai], depth, 1};
                        trySeq(CenterSolver::commutator(ft, sla));
                        trySeq(CenterSolver::commutator(sla, ft));
                    }
                }
            }
            // Single outer turn (sometimes unblocks)
            trySeq({Move{face, 0, 1}});
            trySeq({Move{face, 0, 2}});
        }

        if (bestDelta > 0 && !bestSeq.empty()) {
            append(bestSeq);
        } else {
            break; // no progress
        }
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

    // Phase C: residual short-search for n<=6 (light BFS-style cleanup of remaining cells)
    if (work.size() <= 6 && centerScoreGlobal(work) < 600) {
        append(residualShortSearch(work));
    }

    // Phase D: one more BatchGroups + face pass if any residual (rare)
    if (centerScoreGlobal(work) < 600) {
        auto groups = BatchGroups::fromCube(work);
        auto seq = BatchGroups::applyAll(work, groups);
        append(seq);
        for (int face : order) {
            if (centerScoreFace(work, face) < 100)
                append(solveFace(work, face));
        }
        if (work.size() <= 6)
            append(residualShortSearch(work));
    }

    return solution;
}
