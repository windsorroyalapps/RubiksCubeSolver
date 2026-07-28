#pragma once

#include "Cube.h"
#include <string>
#include <vector>

/**
 * CFOP solver for 3x3.
 * Stages: Cross → F2L → OLL → PLL
 * Currently implements a working beginner-style layer-by-layer path
 * that returns a sequence of Moves. Full optimal tables come next.
 */
class CFOPSolver {
public:
    // Solve the given 3x3 cube. Returns move sequence that solves it.
    // Cube is left unmodified; caller can apply the returned moves.
    static std::vector<Move> solve(const Cube& cube);

    // Convenience: solve and return human-readable Singmaster string
    static std::string solveToNotation(const Cube& cube);

private:
    // Internal helpers (beginner method first, upgrade to full CFOP later)
    static std::vector<Move> solveWhiteCross(Cube& work);
    static std::vector<Move> solveWhiteCorners(Cube& work);
    static std::vector<Move> solveMiddleEdges(Cube& work);
    static std::vector<Move> solveYellowCross(Cube& work);
    static std::vector<Move> orientYellowCorners(Cube& work);
    static std::vector<Move> permuteYellowCorners(Cube& work);
    static std::vector<Move> permuteYellowEdges(Cube& work);

    // Utility
    static Move parseToken(const std::string& token);
    static std::vector<Move> parseSequence(const std::string& notation);
};
