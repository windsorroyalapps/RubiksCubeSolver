#include "ReductionSolver.h"
#include "CenterSolver.h"
#include "EdgePairing.h"
#include "ParityHandler.h"
#include "../cfop/CFOPSolver.h"
#include "../cfop/Kociemba.h"

#include <sstream>

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
        return Kociemba::solve(cube);
    }

    Cube work = cube;
    std::vector<Move> solution;

    auto append = [&](const std::vector<Move>& moves) {
        solution.insert(solution.end(), moves.begin(), moves.end());
    };

    append(solveCenters(work));
    append(pairEdges(work));

    // Even-order parity before treating as 3x3
    if (work.size() % 2 == 0) {
        append(ParityHandler::fix(work));
    }

    append(solveAs3x3(work));

    return solution;
}

std::string ReductionSolver::solveToNotation(const Cube& cube) {
    auto moves = solve(cube);
    static const char* faces = "UDFBLR";
    std::ostringstream oss;
    for (size_t i = 0; i < moves.size(); ++i) {
        const auto& m = moves[i];
        if (m.depth > 0) oss << (m.depth + 1); // 2R notation
        oss << faces[m.face];
        if (m.turns == 2) oss << '2';
        else if (m.turns == -1 || m.turns == 3) oss << '\'';
        if (i + 1 < moves.size()) oss << ' ';
    }
    return oss.str();
}
