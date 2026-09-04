#pragma once

#include "Cube.h"
#include <string>
#include <vector>

/**
 * Reduction method = practical God's algorithm for any n x n x n (n >= 4).
 *
 * Pipeline:
 *   1. Centers  2. Edges  3. Parity (even)  4. 3x3  5. Batch optimize
 *
 * BoundHarness records stage lengths vs U(n), Ucas, L(n), L_fixed and
 * per-stage cascade budgets (centers 8(n-2)^2, wings 96(n-2), ...).
 */
class ReductionSolver {
public:
    static std::vector<Move> solve(const Cube& cube);
    static std::string solveToNotation(const Cube& cube);

    // Last BoundHarness report after solve() (empty if not yet run / 3x3)
    static std::string lastBoundReportString();

private:
    static std::vector<Move> solveCenters(Cube& work);
    static std::vector<Move> pairEdges(Cube& work);
    static std::vector<Move> solveAs3x3(Cube& work);
};
