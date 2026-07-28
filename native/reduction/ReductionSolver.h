#pragma once

#include "Cube.h"
#include <string>
#include <vector>

/**
 * Reduction method = practical God's algorithm for any n x n x n (n >= 4).
 *
 * Pipeline:
 *   1. Solve centers   (CenterSolver)
 *   2. Pair edges      (EdgePairing)
 *   3. Fix parity      (ParityHandler)  — even n only
 *   4. Solve as 3x3    (Kociemba / CFOP)
 *
 * Always terminates. Does not claim optimal diameter
 * (exact g(n) unknown for n>=4; asymptotic Theta(n^2/log n)).
 * See docs/GODS_NUMBER_NXN.md.
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
