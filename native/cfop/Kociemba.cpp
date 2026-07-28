#include "Kociemba.h"
#include "CFOPSolver.h"

#include <sstream>

std::vector<Move> Kociemba::phase1(Cube& /*work*/) {
    // TODO: IDA* search with edge-orientation + corner-orientation + UD-slice coordinates
    // and pruning tables. For now empty — caller falls back.
    return {};
}

std::vector<Move> Kociemba::phase2(Cube& /*work*/) {
    // TODO: IDA* in G1 using only <U,D,F2,B2,L2,R2>
    return {};
}

std::vector<Move> Kociemba::solve(const Cube& cube) {
    if (cube.size() != 3) return {};

    Cube work = cube;
    auto p1 = phase1(work);
    auto p2 = phase2(work);

    if (!p1.empty() || !p2.empty()) {
        std::vector<Move> result = p1;
        result.insert(result.end(), p2.begin(), p2.end());
        return result;
    }

    // Tables not ready yet — fall back to CFOP beginner method
    return CFOPSolver::solve(cube);
}

std::string Kociemba::solveToNotation(const Cube& cube) {
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
