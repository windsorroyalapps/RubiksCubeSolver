#include "ReductionSolver.h"
#include "CenterSolver.h"
#include "EdgePairing.h"
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
    // After reduction the outer layer behaves like 3x3
    // Prefer Kociemba, fall back to CFOP
    if (work.size() >= 3) {
        // Create a logical 3x3 view by using Kociemba/CFOP on the cube
        // (outer facelets only matter for the 3x3 stage)
        auto sol = Kociemba::solve(work);
        if (!sol.empty()) return sol;
        return CFOPSolver::solve(work);
    }
    return {};
}

std::vector<Move> ReductionSolver::solve(const Cube& cube) {
    if (cube.size() < 4) {
        return Kociemba::solve(cube);
    }

    Cube work = cube;
    std::vector<Move> solution;

    auto append = [&](const std::vector<Move>& moves) {
        // moves already applied inside solvers that take Cube&
        solution.insert(solution.end(), moves.begin(), moves.end());
    };

    append(solveCenters(work));
    append(pairEdges(work));
    append(solveAs3x3(work));

    return solution;
}

std::string ReductionSolver::solveToNotation(const Cube& cube) {
    auto moves = solve(cube);
    static const char* faces = "UDFBLR";
    std::ostringstream oss;
    for (size_t i = 0; i < moves.size(); ++i) {
        const auto& m = moves[i];
        // Include depth marker for inner slices when depth > 0
        if (m.depth > 0) oss << m.depth;
        oss << faces[m.face];
        if (m.turns == 2) oss << '2';
        else if (m.turns == -1 || m.turns == 3) oss << '\'';
        if (i + 1 < moves.size()) oss << ' ';
    }
    return oss.str();
}
