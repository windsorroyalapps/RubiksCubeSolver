#pragma once

#include "Cube.h"
#include <string>
#include <vector>

/**
 * Reduction method for n x n x n (n >= 4).
 * 1. Solve centers
 * 2. Pair edges
 * 3. Solve as 3x3 (CFOP)
 *
 * Skeleton ready for full center/edge logic.
 */
class ReductionSolver {
public:
    static std::vector<Move> solve(const Cube& cube);
    static std::string solveToNotation(const Cube& cube);

private:
    static std::vector<Move> solveCenters(Cube& work);
    static std::vector<Move> pairEdges(Cube& work);
    static std::vector<Move> solveAs3x3(Cube& work);
};
