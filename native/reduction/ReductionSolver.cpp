#include "ReductionSolver.h"
#include "CenterSolver.h"
#include "EdgePairing.h"
#include "ParityHandler.h"
#include "BatchSolver.h"
#include "BoundHarness.h"
#include "ReducedSearch.h"
#include "StageCap.h"
#include "../cfop/CFOPSolver.h"
#include "../cfop/Kociemba.h"

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <queue>
#include <set>

static BoundReport g_lastBoundReport{};
static int g_lastLeftoverCenters = 0;
static int g_lastLeftoverWings = 0;
static bool g_lastWorkSolved = false;

std::vector<Move> ReductionSolver::solveCenters(Cube& work) {
    Cube pre = work;
    Cube probe = work;
    auto raw = CenterSolver::solve(probe);
    auto pair = StageCap::capThenRepair(raw, pre, StageCap::budgetCenters(work.size()), true);
    g_lastLeftoverCenters = pair.second;
    work.apply(pair.first);
    return pair.first;
}

std::vector<Move> ReductionSolver::pairEdges(Cube& work) {
    Cube pre = work;
    Cube probe = work;
    auto raw = EdgePairing::pairAll(probe);
    auto pair = StageCap::capThenRepair(raw, pre, StageCap::budgetEdges(work.size()), false);
    g_lastLeftoverWings = pair.second;
    work.apply(pair.first);
    return pair.first;
}

static int absoluteCenterBad(const Cube& c) {
    const int n = c.size();
    int bad = 0;
    for (int f = 0; f < 6; ++f) {
        const Color want = static_cast<Color>(f);
        for (int r = 1; r < n - 1; ++r)
            for (int col = 1; col < n - 1; ++col)
                if (c.get(f, r, col) != want) ++bad;
    }
    return bad;
}

static Move invertMove(const Move& m) {
    return Move{m.face, m.depth, m.turns == 2 ? 2 : -m.turns};
}

static std::vector<Move> makeComm(const Move& a, const Move& b) {
    return {a, b, invertMove(a), invertMove(b)};
}

static std::vector<Move> repairCentersPreserveEdges(Cube& work, int rounds = 12) {
    std::vector<Move> out;
    static const int kFaces[6] = {U, D, F, B, L, R};

    auto envInt = [](const char* name, int def) -> int {
        if (const char* e = std::getenv(name)) {
            int v = std::atoi(e);
            if (v > 0) return v;
        }
        return def;
    };
    const int bfsDepthEnv = envInt("RCS_CENTER_BFS_DEPTH", 7);
    const int bfsNodesEnv = envInt("RCS_CENTER_BFS_NODES", 40000);

    auto bfsOnce = [&](int maxDepth, size_t nodeCap) -> bool {
        const int c0 = absoluteCenterBad(work);
        const int e0 = StageCap::leftoverUnpairedWings(work);
        if (c0 == 0) return false;

        std::vector<Move> gens;
        for (int f : kFaces)
            for (int turns : {1, -1, 2})
                gens.push_back(Move{f, 1, turns});
        for (int turns : {1, -1, 2}) {
            gens.push_back(Move{L, 0, turns});
            gens.push_back(Move{R, 0, turns});
        }
        for (int f : {U, D, F, B})
            for (int turns : {1, -1, 2})
                gens.push_back(Move{f, 0, turns});

        std::vector<std::vector<Move>> macros;
        for (int face : kFaces) {
            const int opp = face ^ 1;
            for (int turns : {1, -1, 2}) {
                Move ft{face, 0, turns};
                Move sl{opp, 1, 1};
                macros.push_back(makeComm(ft, sl));
                macros.push_back(makeComm(sl, ft));
                for (int adj : kFaces) {
                    if (adj == face || adj == opp) continue;
                    Move sla{adj, 1, 1};
                    macros.push_back(makeComm(ft, sla));
                }
            }
        }

        struct Node {
            Cube cube;
            std::vector<Move> path;
        };
        std::queue<Node> q;
        q.push(Node{work, {}});

        auto keyOf = [&](const Cube& c) -> uint64_t {
            uint64_t k = 1469598103934665603ULL;
            const int n = c.size();
            auto feed = [&](unsigned v) {
                k ^= static_cast<uint64_t>(v) + 0x9e3779b97f4a7c15ULL;
                k *= 1099511628211ULL;
            };
            for (int f = 0; f < 6; ++f)
                for (int r = 1; r < n - 1; ++r)
                    for (int col = 1; col < n - 1; ++col)
                        feed(static_cast<unsigned>(c.get(f, r, col)));
            for (int i = 1; i < n - 1; ++i) {
                feed(static_cast<unsigned>(c.get(U, n - 1, i)));
                feed(static_cast<unsigned>(c.get(F, 0, i)));
                feed(static_cast<unsigned>(c.get(U, 0, i)));
                feed(static_cast<unsigned>(c.get(B, 0, i)));
                feed(static_cast<unsigned>(c.get(D, 0, i)));
                feed(static_cast<unsigned>(c.get(F, n - 1, i)));
                feed(static_cast<unsigned>(c.get(D, n - 1, i)));
                feed(static_cast<unsigned>(c.get(B, n - 1, i)));
            }
            return k;
        };

        std::set<uint64_t> seen;
        seen.insert(keyOf(work));
        size_t nodes = 0;
        int bestC = c0;
        std::vector<Move> bestPath;

        auto consider = [&](Cube nxt, std::vector<Move> path) {
            if (StageCap::leftoverUnpairedWings(nxt) > e0 + 2) return;
            if (absoluteCenterBad(nxt) > c0 + 4) return;
            const uint64_t k = keyOf(nxt);
            if (seen.count(k)) return;
            seen.insert(k);
            Node nn;
            nn.cube = std::move(nxt);
            nn.path = std::move(path);
            q.push(std::move(nn));
        };

        while (!q.empty() && nodes < nodeCap) {
            Node cur = std::move(q.front());
            q.pop();
            ++nodes;
            const int c = absoluteCenterBad(cur.cube);
            const int e = StageCap::leftoverUnpairedWings(cur.cube);
            if (e <= e0 && c < bestC) {
                bestC = c;
                bestPath = cur.path;
                if (c == 0) break;
            }
            if (static_cast<int>(cur.path.size()) >= maxDepth) continue;
            const int lf = cur.path.empty() ? -1 : cur.path.back().face;
            const int ld = cur.path.empty() ? -1 : cur.path.back().depth;
            for (const Move& m : gens) {
                if (m.face == lf && m.depth == ld) continue;
                Cube nxt = cur.cube;
                nxt.apply(m);
                auto p = cur.path;
                p.push_back(m);
                consider(std::move(nxt), std::move(p));
            }
            if (static_cast<int>(cur.path.size()) + 4 <= maxDepth) {
                for (const auto& mac : macros) {
                    Cube nxt = cur.cube;
                    auto p = cur.path;
                    for (const Move& m : mac) {
                        nxt.apply(m);
                        p.push_back(m);
                    }
                    consider(std::move(nxt), std::move(p));
                }
            }
        }

        if (bestPath.empty() || bestC >= c0) return false;
        for (const Move& m : bestPath) {
            work.apply(m);
            out.push_back(m);
        }
        return true;
    };

    auto greedyCommutators = [&]() -> bool {
        const int c0 = absoluteCenterBad(work);
        const int e0 = StageCap::leftoverUnpairedWings(work);
        if (c0 == 0) return false;
        bool found = false;
        int bestC = c0;
        std::vector<Move> best;
        for (int af : kFaces) {
            for (int bf : kFaces) {
                if (af == bf || (af ^ 1) == bf) continue;
                for (int ad = 0; ad < 2; ++ad) {
                    for (int bd = 0; bd < 2; ++bd) {
                        for (int at : {1, -1, 2}) {
                            for (int bt : {1, -1, 2}) {
                                Move mA{af, ad, at};
                                Move mB{bf, bd, bt};
                                std::vector<Move> seq = makeComm(mA, mB);
                                for (int rep = 0; rep < 2; ++rep) {
                                    Cube probe = work;
                                    for (const Move& m : seq) probe.apply(m);
                                    if (rep == 1) {
                                        for (const Move& m : seq) probe.apply(m);
                                    }
                                    const int c = absoluteCenterBad(probe);
                                    const int e = StageCap::leftoverUnpairedWings(probe);
                                    if (e <= e0 && c < bestC) {
                                        bestC = c;
                                        best = seq;
                                        if (rep == 1) {
                                            best.insert(best.end(), seq.begin(), seq.end());
                                        }
                                        found = true;
                                    }
                                }
                                for (int xf : kFaces) {
                                    for (int xt : {1, -1, 2}) {
                                        Move mX{xf, 0, xt};
                                        std::vector<Move> conj = {mX};
                                        conj.insert(conj.end(), seq.begin(), seq.end());
                                        conj.push_back(invertMove(mX));
                                        Cube probe = work;
                                        for (const Move& m : conj) probe.apply(m);
                                        const int c = absoluteCenterBad(probe);
                                        const int e = StageCap::leftoverUnpairedWings(probe);
                                        if (e <= e0 && c < bestC) {
                                            bestC = c;
                                            best = conj;
                                            found = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!found) return false;
        for (const Move& m : best) {
            work.apply(m);
            out.push_back(m);
        }
        return true;
    };

    for (int r = 0; r < rounds; ++r) {
        if (absoluteCenterBad(work) == 0) break;
        if (greedyCommutators()) continue;
        const int depth = (r < 3) ? std::max(bfsDepthEnv, 7) : std::max(bfsDepthEnv + 2, 9);
        const size_t cap = (r < 3)
            ? static_cast<size_t>(std::max(bfsNodesEnv, 80000))
            : static_cast<size_t>(std::max(bfsNodesEnv * 2, 200000));
        if (!bfsOnce(depth, cap)) break;
    }
    return out;
}

static Cube extractVirtual3x3(const Cube& big) {
    Cube s(3);
    const int n = big.size();
    // Prefer a solid wing slot; fall back to index 1.
    int wing = 1;
    for (int slot = 1; slot <= n - 2; ++slot) {
        bool ok = true;
        for (int e = 0; e < 12 && ok; ++e) {
            if (EdgePairing::pairedWings(big, e) < (n - 2)) { ok = false; break; }
        }
        if (ok) { wing = slot; break; }
    }
    for (int f = 0; f < 6; ++f) {
        s.set(f, 1, 1, static_cast<Color>(f));
        s.set(f, 0, 0, big.get(f, 0, 0));
        s.set(f, 0, 2, big.get(f, 0, n - 1));
        s.set(f, 2, 0, big.get(f, n - 1, 0));
        s.set(f, 2, 2, big.get(f, n - 1, n - 1));
        s.set(f, 0, 1, big.get(f, 0, wing));
        s.set(f, 2, 1, big.get(f, n - 1, wing));
        s.set(f, 1, 0, big.get(f, wing, 0));
        s.set(f, 1, 2, big.get(f, wing, n - 1));
    }
    return s;
}

static int faceMismatch(const Cube& c) {
    int bad = 0;
    const int n = c.size();
    for (int f = 0; f < 6; ++f)
        for (int r = 0; r < n; ++r)
            for (int col = 0; col < n; ++col)
                if (c.get(f, r, col) != static_cast<Color>(f)) ++bad;
    return bad;
}

static std::vector<Move> outerLayerIda(Cube& work, int maxDepth, int nodeCap) {
    if (work.isSolved()) return {};
    std::vector<Move> path, best;
    int nodes = 0;
    std::function<bool(int, int, int)> search = [&](int depth, int limit, int lastFace) -> bool {
        ++nodes;
        if (nodes > nodeCap) return false;
        if (work.isSolved()) { best = path; return true; }
        const int h = (faceMismatch(work) + 7) / 8;
        if (depth + h > limit) return false;
        if (depth >= limit) return false;
        for (int f = 0; f < 6; ++f) {
            if (f == lastFace) continue;
            for (int turns : {1, 2, -1}) {
                Move m{f, 0, turns};
                work.apply(m);
                path.push_back(m);
                if (search(depth + 1, limit, f)) return true;
                path.pop_back();
                work.apply(invertMove(m));
                if (nodes > nodeCap) return false;
            }
        }
        return false;
    };
    const int h0 = (faceMismatch(work) + 7) / 8;
    for (int depth = std::max(1, h0); depth <= maxDepth; ++depth) {
        path.clear();
        nodes = 0;
        if (search(0, depth, -1)) return best;
        if (nodes > nodeCap) break;
    }
    return {};
}

static std::vector<Move> nearSolvedFinish(Cube& work, int maxDepth, int nodeCap) {
    if (work.isSolved()) return {};
    const int startBad = faceMismatch(work);
    if (startBad > 56) return {};

    static const int kFaces[6] = {U, D, F, B, L, R};
    std::vector<Move> gens;
    for (int f : kFaces)
        for (int d = 0; d < 2; ++d)
            for (int turns : {1, 2, -1})
                gens.push_back(Move{f, d, turns});

    std::vector<Move> path, best;
    int nodes = 0;
    std::function<bool(int, int, int, int)> search =
        [&](int depth, int limit, int lastFace, int lastDepth) -> bool {
        ++nodes;
        if (nodes > nodeCap) return false;
        if (work.isSolved()) { best = path; return true; }
        const int h = (faceMismatch(work) + 7) / 8;
        if (depth + h > limit) return false;
        if (depth >= limit) return false;
        for (const Move& m : gens) {
            if (m.face == lastFace && m.depth == lastDepth) continue;
            work.apply(m);
            path.push_back(m);
            if (search(depth + 1, limit, m.face, m.depth)) return true;
            path.pop_back();
            work.apply(invertMove(m));
            if (nodes > nodeCap) return false;
        }
        return false;
    };

    const int h0 = (startBad + 7) / 8;
    for (int depth = std::max(1, h0); depth <= maxDepth; ++depth) {
        path.clear();
        nodes = 0;
        if (search(0, depth, -1, -1)) return best;
        if (nodes > nodeCap) break;
    }
    return {};
}

std::vector<Move> ReductionSolver::solveAs3x3(Cube& work) {
    if (work.isSolved()) return {};

    if (work.size() == 3) {
        auto sol = Kociemba::solve(work);
        if (sol.empty()) sol = CFOPSolver::solve(work);
        work.apply(sol);
        return sol;
    }

    std::vector<Move> solution;
    const int cLeft = absoluteCenterBad(work);
    const int eStage = StageCap::leftoverUnpairedWings(work);
    const int eAll = EdgePairing::leftoverUnpairedWings(work);
    // Virtual 3x3 requires ALL 12 dedges paired, not only StageCap's 4 strips.
    const bool reducedEnough = (cLeft == 0 && eAll == 0);

    if (reducedEnough) {
        Cube small = extractVirtual3x3(work);
        if (faceMismatch(small) <= 48) {
            auto sol = Kociemba::solve(small);
            if (sol.empty()) sol = CFOPSolver::solve(small);
            if (!sol.empty()) {
                Cube probe = work;
                probe.apply(sol);
                if (probe.isSolved() || faceMismatch(probe) < faceMismatch(work)) {
                    work.apply(sol);
                    solution = std::move(sol);
                }
            }
        }
    }

    if (work.isSolved()) return solution;

    if (reducedEnough) {
        auto rest = outerLayerIda(work, 14, 500000);
        if (!rest.empty())
            solution.insert(solution.end(), rest.begin(), rest.end());
    }

    if (!work.isSolved()) {
        const int mm = faceMismatch(work);
        if (eStage <= 1 && cLeft <= 4 && mm <= 56) {
            auto fin = nearSolvedFinish(work, 16, 800000);
            if (!fin.empty())
                solution.insert(solution.end(), fin.begin(), fin.end());
        }
    }
    return solution;
}

std::vector<Move> ReductionSolver::solve(const Cube& cube) {
    g_lastLeftoverCenters = 0;
    g_lastLeftoverWings = 0;
    g_lastWorkSolved = false;
    if (cube.size() < 4) {
        g_lastBoundReport = {};
        return Kociemba::solve(cube);
    }

    Cube work = cube;
    std::vector<Move> solution;
    StageLengths stages;

    auto append = [&](const std::vector<Move>& moves, int* sstmCounter, int* obtmCounter) {
        solution.insert(solution.end(), moves.begin(), moves.end());
        if (sstmCounter) *sstmCounter += BoundHarness::count(moves);
        if (obtmCounter) *obtmCounter += BoundHarness::countObtm(moves);
    };

    append(solveCenters(work), &stages.centers, &stages.centersObtm);
    append(pairEdges(work), &stages.edges, &stages.edgesObtm);

    {
        auto repaired = repairCentersPreserveEdges(work, 14);
        if (!repaired.empty()) {
            append(repaired, &stages.centers, &stages.centersObtm);
        }
        g_lastLeftoverCenters = absoluteCenterBad(work);
        g_lastLeftoverWings = StageCap::leftoverUnpairedWings(work);
    }

    if (work.size() % 2 == 0) {
        const int cLeft = absoluteCenterBad(work);
        const int eLeft = StageCap::leftoverUnpairedWings(work);
        const int eAll = EdgePairing::leftoverUnpairedWings(work);
        if (cLeft == 0 && eLeft == 0 && eAll == 0) {
            append(ParityHandler::fix(work), &stages.parity, &stages.parityObtm);
        }
    }

    if ((work.size() == 4 || work.size() == 5) &&
        StageCap::leftoverUnpairedWings(work) <= 2) {
        Cube probe = work;
        auto improved = ReducedSearch::improve(probe);
        if (!improved.empty()) {
            const int eBefore = StageCap::leftoverUnpairedWings(work);
            const int cBefore = absoluteCenterBad(work);
            if (StageCap::leftoverUnpairedWings(probe) <= eBefore &&
                (absoluteCenterBad(probe) < cBefore || probe.isSolved())) {
                work.apply(improved);
                append(improved, &stages.reduced, &stages.reducedObtm);
                g_lastLeftoverCenters = absoluteCenterBad(work);
                g_lastLeftoverWings = StageCap::leftoverUnpairedWings(work);
            }
        }
    }

    if (StageCap::leftoverUnpairedWings(work) == 0 && absoluteCenterBad(work) > 0) {
        auto more = repairCentersPreserveEdges(work, 16);
        if (!more.empty()) {
            append(more, &stages.centers, &stages.centersObtm);
            g_lastLeftoverCenters = absoluteCenterBad(work);
            g_lastLeftoverWings = StageCap::leftoverUnpairedWings(work);
        }
    }

    append(solveAs3x3(work), &stages.reduce3x3, &stages.reduce3x3Obtm);

    // Last-ditch finish when nearly reduced.
    if (!work.isSolved() && absoluteCenterBad(work) == 0 &&
        EdgePairing::leftoverUnpairedWings(work) == 0) {
        auto fin = nearSolvedFinish(work, 18, 1000000);
        if (!fin.empty()) {
            append(fin, &stages.reduce3x3, &stages.reduce3x3Obtm);
        }
    }

    auto optimized = BatchSolver::optimize(solution);
    if (work.isSolved()) {
        Cube verify = cube;
        verify.apply(optimized);
        if (!verify.isSolved()) {
            optimized = BatchSolver::compress(solution);
            verify = cube;
            verify.apply(optimized);
            if (!verify.isSolved()) optimized = solution;
        }
    }
    solution = std::move(optimized);
    stages.afterBatch = BoundHarness::count(solution);

    g_lastWorkSolved = work.isSolved();
    g_lastLeftoverCenters = absoluteCenterBad(work);
    g_lastLeftoverWings = StageCap::leftoverUnpairedWings(work);
    g_lastBoundReport = BoundHarness::report(cube.size(), stages, solution);
    return solution;
}

std::string ReductionSolver::solveToNotation(const Cube& cube) {
    return Cube::movesToNotation(solve(cube));
}

std::string ReductionSolver::lastBoundReportString() {
    std::string s = g_lastBoundReport.toString();
    s += " leftoverC=";
    s += std::to_string(g_lastLeftoverCenters);
    s += " leftoverE=";
    s += std::to_string(g_lastLeftoverWings);
    s += " workSolved=";
    s += (g_lastWorkSolved ? "yes" : "no");
    return s;
}
