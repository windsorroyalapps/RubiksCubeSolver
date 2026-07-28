#pragma once

#include "Cube.h"
#include <vector>

/**
 * Solve centers for n x n x n (n >= 4).
 * Odd n: fixed centers define colors.
 * Even n: choose a consistent color scheme.
 *
 * Uses commutator-style sequences to place center pieces
 * without destroying already-solved centers.
 */
class CenterSolver {
public:
    static std::vector<Move> solve(Cube& work);

private:
    // Build one face center (all facelets of that face matching center color)
    static std::vector<Move> solveFace(Cube& work, int face);

    // Simple commutator: bring a piece into position
    // [A, B] = A B A' B'
    static std::vector<Move> commutator(const Move& a, const Move& b);
};
