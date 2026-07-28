#include "ReductionSolver.h"
#include "../cfop/CFOPSolver.h"

#include <sstream>

std::vector<Move> ReductionSolver::solveCenters(Cube& /*work*/) {
    // TODO: center commutators for arbitrary n
    // For odd n centers are fixed; for even n we choose color scheme.
    return {};
}

std::vector<Move> ReductionSolver::pairEdges(Cube& /*work*/) {
    // TODO: freeslice / pair-by-pair edge matching
    return {};
}

std::vector<Move> ReductionSolver::solveAs3x3(Cube& work) {
    // Once centers + edges are done, treat outer layer as 3x3
    return CFOPSolver::solve(work);
}

std::vector<Move> ReductionSolver::solve(const Cube& cube) {
    if (cube.size() < 4) {
        // Fall back to CFOP for 3x3
        return CFOPSolver::solve(cube);
    }

    Cube work = cube;
    std::vector<Move> solution;

    auto append = [&](const std::vector<Move>& moves) {
        for (const auto& m : moves) {
            work.apply(m);
            solution.push_back(m);
        }
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
        oss << faces[m.face];
        if (m.turns == 2) oss << '2';
        else if (m.turns == -1 || m.turns == 3) oss << '\'';
        if (i + 1 < moves.size()) oss << ' ';
    }
    return oss.str();
}
