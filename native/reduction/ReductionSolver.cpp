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

// Repair centers without increasing StageCap leftoverE (end-state).
static std::vector<Move> repairCentersPreserveEdges(Cube& work, int rounds = 10) {
    std::vector<Move> out;
    static const int kFaces[6] = {U, D, F, B, L, R};

    auto bfsOnce = [&](int maxDepth, size_t nodeCap) -> bool {
        const int c0 = absoluteCenterBad(work);
        const int e0 = StageCap::leftoverUnpairedWings(work);
        if (c0 == 0) return false;

        std::vector<Move> gens;
        for (int turns : {1, -1, 2}) {
            gens.push_back(Move{L, 0, turns});
            gens.push_back(Move{R, 0, turns});
        }
        for (int f : kFaces)
            for (int turns : {1, -1, 2})
                gens.push_back(Move{f, 1, turns});
        for (int f : {U, D, F, B})
            for (int turns : {1, -1, 2})
                gens.push_back(Move{f, 0, turns});

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
                if (StageCap::leftoverUnpairedWings(nxt) > e0 + 4) continue;
                if (absoluteCenterBad(nxt) > c0 + 3) continue;
                const uint64_t k = keyOf(nxt);
                if (seen.count(k)) continue;
                seen.insert(k);
                Node nn;
                nn.cube = std::move(nxt);
                nn.path = cur.path;
                nn.path.push_back(m);
                q.push(std::move(nn));
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
                                std::vector<Move> seq = {mA, mB, invertMove(mA), invertMove(mB)};
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
        const int depth = (r < 4) ? 7 : 9;
        const size_t cap = (r < 4) ? 200000 : 400000;
        if (!bfsOnce(depth, cap)) break;
    }
    return out;
}

static Cube extractVirtual3x3(const Cube& big) {
    Cube s(3);
    const int n = big.size();
    const int wing = 1;
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
    if (startBad > 48) return {};

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
    const int eLeft = StageCap::leftoverUnpairedWings(work);
    const bool reducedEnough = (cLeft == 0 && eLeft == 0);

    if (reducedEnough) {
        Cube small = extractVirtual3x3(work);
        if (faceMismatch(small) <= 40) {
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
        auto rest = outerLayerIda(work, 10, 200000);
        if (!rest.empty())
            solution.insert(solution.end(), rest.begin(), rest.end());
    }

    if (!work.isSolved()) {
        const int mm = faceMismatch(work);
        if (eLeft <= 1 && cLeft <= 8 && mm <= 48) {
            auto fin = nearSolvedFinish(work, 14, 600000);
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
        auto repaired = repairCentersPreserveEdges(work);
        if (!repaired.empty()) {
            append(repaired, &stages.centers, &stages.centersObtm);
        }
        g_lastLeftoverCenters = absoluteCenterBad(work);
        g_lastLeftoverWings = StageCap::leftoverUnpairedWings(work);
    }

    if (work.size() % 2 == 0) {
        const int cLeft = absoluteCenterBad(work);
        const int eLeft = StageCap::leftoverUnpairedWings(work);
        if (cLeft == 0 && eLeft <= 2) {
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

    if (StageCap::leftoverUnpairedWings(work) == 0 && absoluteCenterBad(work) > 0 &&
        absoluteCenterBad(work) <= 8) {
        auto more = repairCentersPreserveEdges(work, 10);
        if (!more.empty()) {
            append(more, &stages.centers, &stages.centersObtm);
            g_lastLeftoverCenters = absoluteCenterBad(work);
            g_lastLeftoverWings = StageCap::leftoverUnpairedWings(work);
        }
    }

    append(solveAs3x3(work), &stages.reduce3x3, &stages.reduce3x3Obtm);

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
