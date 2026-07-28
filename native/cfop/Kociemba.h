#pragma once

#include "Cube.h"
#include <string>
#include <vector>

/**
 * Kociemba two-phase algorithm skeleton.
 *
 * Phase 1: reduce to G1 subgroup (edges oriented, corners oriented,
 *          middle-slice edges in middle layer).
 * Phase 2: solve within G1 using only moves that preserve the subgroup.
 *
 * This is the production path for near-optimal 3x3 solutions.
 * Full pruning tables will be generated / embedded in a later pass.
 */
class Kociemba {
public:
    // Returns a short solution (target <= 20 moves in production).
    static std::vector<Move> solve(const Cube& cube);
    static std::string solveToNotation(const Cube& cube);

private:
    static std::vector<Move> phase1(Cube& work);
    static std::vector<Move> phase2(Cube& work);
};
