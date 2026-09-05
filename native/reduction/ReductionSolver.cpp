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

static BoundReport g_lastBoundReport{};

std::vector<Move> ReductionSolver::solveCenters(Cube& work) {
    auto raw = CenterSolver::solve(work);
    const int cap = StageCap::budgetCenters(work.size());
    return StageCap::capToBudget(raw, cap);
}

std::vector<Move> ReductionSolver::pairEdges(Cube& work) {
    auto raw = EdgePairing::pairAll(work);
    const int cap = StageCap::budgetEdges(work.size());
    return StageCap::capToBudget(raw, cap);
}

std::vector<Move> ReductionSolver::solveAs3x3(Cube& work) {
    auto sol = Kociemba::solve(work);
    if (!sol.empty()) return sol;
    return CFOPSolver::solve(work);
}

std::vector<Move> ReductionSolver::solve(const Cube& cube) {
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

    if (work.size() % 2 == 0) {
        append(ParityHandler::fix(work), &stages.parity, &stages.parityObtm);
    }

    if (work.size() == 4 || work.size() == 5) {
        auto improved = ReducedSearch::improve(work);
        if (!improved.empty()) {
            append(improved, &stages.reduced, &stages.reducedObtm);
        }
    }

    append(solveAs3x3(work), &stages.reduce3x3, &stages.reduce3x3Obtm);

    solution = BatchSolver::optimize(solution);
    stages.afterBatch = BoundHarness::count(solution);

    g_lastBoundReport = BoundHarness::report(cube.size(), stages, solution);
    return solution;
}

std::string ReductionSolver::solveToNotation(const Cube& cube) {
    return Cube::movesToNotation(solve(cube));
}

std::string ReductionSolver::lastBoundReportString() {
    return g_lastBoundReport.toString();
}
