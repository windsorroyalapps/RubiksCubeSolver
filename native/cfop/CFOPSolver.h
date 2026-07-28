#pragma once

#include "Cube.h"
#include <string>
#include <vector>

class CFOPSolver {
public:
    static std::vector<Move> solve(const Cube& cube);
    static std::string solveToNotation(const Cube& cube);

private:
    static std::vector<Move> solveWhiteCross(Cube& work);
    static std::vector<Move> solveWhiteCorners(Cube& work);
    static std::vector<Move> solveMiddleEdges(Cube& work);

    // Pattern-aware last layer
    static std::vector<Move> solveYellowCross(Cube& work);
    static std::vector<Move> orientYellowCorners(Cube& work);
    static std::vector<Move> permuteYellowCorners(Cube& work);
    static std::vector<Move> permuteYellowEdges(Cube& work);

    // Recognition helpers (U face = yellow for last layer)
    static int countYellowEdgesOnU(const Cube& c);
    static int countYellowCornersOnU(const Cube& c);
    static bool isYellowCross(const Cube& c);

    static Move parseToken(const std::string& token);
    static std::vector<Move> parseSequence(const std::string& notation);
    static void appendSeq(Cube& work, std::vector<Move>& out, const std::string& seq);
};
