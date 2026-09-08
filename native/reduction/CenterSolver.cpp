#include "CenterSolver.h"
#include "ClusterScheduler.h"
#include "BatchGroups.h"

#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <tuple>
#include <cstdint>
#include <cstdlib>

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

static uint64_t packCenterState(const Cube& work) {
    int n = work.size();
    uint64_t key = 0;
    int bit = 0;
    for (int f = 0; f < 6; ++f) {
        Color target = static_cast<Color>(f);
        for (int r = 1; r < n - 1; ++r) {
            for (int c = 1; c < n - 1; ++c) {
                if (work.get(f, r, c) == target)
                    key |= (uint64_t{1} << bit);
                ++bit;
            }
        }
    }
    return key;
}

std::vector<Move> CenterSolver::solveFace(Cube& work, int face) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4) return moves;
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

    int maxAttempts = 64 + 12 * (n - 4);
    if (maxAttempts > 160) maxAttempts = 160;

    for (int attempt = 0; attempt < maxAttempts && centerScoreFace(work, face) < 100; ++attempt) {
        int bestGain = -1;
        int bestGlobalDelta = -999;
        std::vector<Move> bestSeq;
        int scoreBefore = centerScoreFace(work, face);
        int globalBefore = centerScoreGlobal(work);
        int badBefore = incorrectCenters(work);

        auto trySeq = [&](const std::vector<Move>& seq) {
            for (const auto& m : seq) work.apply(m);
            int faceAfter = centerScoreFace(work, face);
            int globalAfter = centerScoreGlobal(work);
            int badAfter = incorrectCenters(work);
            undoSeq(work, seq);
            int gain = faceAfter - scoreBefore;
            int gDelta = globalAfter - globalBefore;
            if (badAfter > badBefore) return; // never increase leftover centers
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
            // Do NOT burn an outer-only turn. It does not reduce incorrectCenters.
            break;
        }

        if (centerScoreFace(work, face) >= 100) break;
    }
    return moves;
}

static std::vector<Move> residualShortSearch(Cube& work) {
    std::vector<Move> moves;
    int n = work.size();
    if (n != 6) return moves;
    if (centerScoreGlobal(work) >= 600) return moves;

    auto append = [&](const std::vector<Move>& seq) {
        for (const auto& m : seq) {
            work.apply(m);
            moves.push_back(m);
        }
    };

    int maxRounds = 12;
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
        }

        if (bestDelta > 0 && !bestSeq.empty()) {
            append(bestSeq);
        } else {
            break;
        }
    }
    return moves;
}

static std::vector<Move> centerOrbitBfs(Cube& work) {
    std::vector<Move> result;
    int n = work.size();
    if (n < 4 || n > 5) return result;
    if (incorrectCenters(work) == 0) return result;

    int maxDepth = (n == 4) ? 7 : 5;
    int maxNodes = (n == 4) ? 40000 : 20000;
    if (const char* e = std::getenv("RCS_CENTER_BFS_DEPTH")) {
        int v = std::atoi(e);
        if (v > 0) maxDepth = v;
    }
    if (const char* e = std::getenv("RCS_CENTER_BFS_NODES")) {
        int v = std::atoi(e);
        if (v > 0) maxNodes = v;
    }

    std::vector<std::vector<Move>> gens;
    for (int face = 0; face < 6; ++face) {
        for (int t : {1, 2, -1})
            gens.push_back({Move{face, 0, t}});
    }
    int maxSlice = n / 2;
    for (int face = 0; face < 6; ++face) {
        int opp = opposite(face);
        int adj[4];
        adjacentFaces(face, adj[0], adj[1], adj[2], adj[3]);
        for (int depth = 1; depth < maxSlice; ++depth) {
            for (int turns : {1, -1}) {
                Move ft{face, 0, turns};
                Move sl{opp, depth, 1};
                gens.push_back(CenterSolver::commutator(ft, sl));
                gens.push_back(CenterSolver::commutator(sl, ft));
                for (int ai = 0; ai < 4; ++ai) {
                    Move sla{adj[ai], depth, 1};
                    gens.push_back(CenterSolver::commutator(ft, sla));
                }
            }
        }
    }

    struct Node {
        uint64_t key;
        int bad;
        int global;
        int depth;
        int parent;
        int genIdx;
    };

    std::vector<Node> nodes;
    nodes.reserve(maxNodes);
    std::map<uint64_t, int> visited;

    uint64_t startKey = packCenterState(work);
    int startBad = incorrectCenters(work);
    int startGlobal = centerScoreGlobal(work);
    nodes.push_back({startKey, startBad, startGlobal, 0, -1, -1});
    visited[startKey] = 0;

    int bestNode = 0;
    int bestBad = startBad;

    auto applyPath = [&](Cube& c, int nodeIdx) {
        std::vector<int> path;
        for (int i = nodeIdx; i > 0; i = nodes[i].parent)
            path.push_back(nodes[i].genIdx);
        std::reverse(path.begin(), path.end());
        for (int g : path) {
            for (const auto& m : gens[g])
                c.apply(m);
        }
    };

    int head = 0;
    while (head < (int)nodes.size() && (int)nodes.size() < maxNodes) {
        const Node& cur = nodes[head];
        if (cur.depth >= maxDepth) {
            ++head;
            continue;
        }
        if (cur.bad == 0) {
            bestNode = head;
            bestBad = 0;
            break;
        }

        for (int g = 0; g < (int)gens.size(); ++g) {
            Cube trial = work;
            applyPath(trial, head);
            for (const auto& m : gens[g])
                trial.apply(m);

            int gScore = centerScoreGlobal(trial);
            if (gScore < startGlobal) continue;

            uint64_t key = packCenterState(trial);
            if (visited.count(key)) continue;

            int bad = incorrectCenters(trial);
            int idx = (int)nodes.size();
            nodes.push_back({key, bad, gScore, cur.depth + 1, head, g});
            visited[key] = idx;

            if (bad < bestBad) {
                bestBad = bad;
                bestNode = idx;
            }
            if (bad == 0) {
                bestNode = idx;
                bestBad = 0;
                head = (int)nodes.size();
                break;
            }
            if ((int)nodes.size() >= maxNodes) break;
        }
        ++head;
    }

    if (bestNode == 0 || bestBad >= startBad)
        return result;

    std::vector<int> genPath;
    for (int i = bestNode; i > 0; i = nodes[i].parent)
        genPath.push_back(nodes[i].genIdx);
    std::reverse(genPath.begin(), genPath.end());

    for (int g : genPath) {
        for (const auto& m : gens[g]) {
            work.apply(m);
            result.push_back(m);
        }
    }
    return result;
}

static std::vector<Move> centerOrbitBfsLoop(Cube& work) {
    std::vector<Move> all;
    for (int round = 0; round < 6 && incorrectCenters(work) > 0; ++round) {
        int before = incorrectCenters(work);
        auto seq = centerOrbitBfs(work);
        all.insert(all.end(), seq.begin(), seq.end());
        int after = incorrectCenters(work);
        if (after >= before) break;
    }
    return all;
}

std::vector<Move> CenterSolver::solve(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4) return solution;

    auto append = [&](const std::vector<Move>& seq) {
        solution.insert(solution.end(), seq.begin(), seq.end());
    };

    for (int pass = 0; pass < 4; ++pass) {
        auto groups = BatchGroups::fromCube(work);
        if (groups.empty()) break;
        auto seq = BatchGroups::applyAll(work, groups);
        if (seq.empty()) break;
        append(seq);
        if (incorrectCenters(work) == 0) return solution;
    }

    const int order[] = {U, D, F, B, L, R};
    for (int face : order) {
        append(solveFace(work, face));
    }

    if (work.size() <= 5 && incorrectCenters(work) > 0) {
        append(centerOrbitBfsLoop(work));
    } else if (work.size() == 6 && incorrectCenters(work) > 0) {
        append(residualShortSearch(work));
    }

    if (incorrectCenters(work) > 0) {
        auto groups = BatchGroups::fromCube(work);
        auto seq = BatchGroups::applyAll(work, groups);
        append(seq);
        for (int face : order) {
            if (centerScoreFace(work, face) < 100)
                append(solveFace(work, face));
        }
        if (work.size() <= 5)
            append(centerOrbitBfsLoop(work));
        else if (work.size() == 6)
            append(residualShortSearch(work));
    }

    return solution;
}
