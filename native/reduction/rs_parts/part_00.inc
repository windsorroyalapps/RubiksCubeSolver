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

static std::vector<Move> repairCentersPreserveEdges(Cube& work, int rounds = 12, bool preserveFloating = true) {
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
        const int eAll0 = preserveFloating ? EdgePairing::leftoverUnpairedWings(work) : 999;
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
