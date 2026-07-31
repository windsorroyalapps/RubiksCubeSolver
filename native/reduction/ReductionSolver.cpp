#include "ReductionSolver.h"
#include "CenterSolver.h"
#include "EdgePairing.h"
#include "ParityHandler.h"
#include "BatchSolver.h"
#include "BoundHarness.h"
#include "../cfop/CFOPSolver.h"
#include "../cfop/Kociemba.h"

#include <sstream>

static BoundReport g_lastBoundReport{};

std::vector<Move> ReductionSolver::solveCenters(Cube& work) {
    return CenterSolver::solve(work);
}

std::vector<Move> ReductionSolver::pairEdges(Cube& work) {
    return EdgePairing::pairAll(work);
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

    auto append = [&](const std::vector<Move>& moves, int* counter) {
        solution.insert(solution.end(), moves.begin(), moves.end());
        if (counter) *counter += BoundHarness::count(moves);
    };

    append(solveCenters(work), &stages.centers);
    append(pairEdges(work), &stages.edges);

    if (work.size() % 2 == 0) {
        append(ParityHandler::fix(work), &stages.parity);
    }

    append(solveAs3x3(work), &stages.reduce3x3);

    solution = BatchSolver::optimize(solution);
    stages.afterBatch = BoundHarness::count(solution);

    // Dual metrics (SSTM + OBTM) for comparison against community ceilings
    g_lastBoundReport = BoundHarness::report(cube.size(), stages, solution);
    return solution;
}

std::string ReductionSolver::solveToNotation(const Cube& cube) {
    auto moves = solve(cube);
    static const char* faces = "UDFBLR";
    std::ostringstream oss;
    for (size_t i = 0; i < moves.size(); ++i) {
        const auto& m = moves[i];
        if (m.depth > 0) oss << (m.depth + 1);
        oss << faces[m.face];
        if (m.turns == 2) oss << '2';
        else if (m.turns == -1 || m.turns == 3) oss << '\'';
        if (i + 1 < moves.size()) oss << ' ';
    }
    return oss.str();
}

std::string ReductionSolver::lastBoundReportString() {
    return g_lastBoundReport.toString();
}
