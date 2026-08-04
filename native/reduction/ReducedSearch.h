#pragma once

#include "Cube.h"
#include <vector>

/**
 * Reduced-coordinate search for small nxn (4x4 / 5x5).
 *
 * After centers + edges (+ parity) the cube is nearly a 3x3 with residual
 * inner-slice freedom. This module runs a depth-limited IDA*-style search
 * over outer + curated inner moves to shorten the remaining path before the
 * classic 3x3 stage, pushing constructive lengths toward community OBTM
 * ceilings (~40–54 for 4×4).
 *
 * Scaffolding (2026-08-05): generators + depth limit + never-break guard
 * placeholder. Full reduced coordinates (edge/center residual packing) and
 * bidirectional search come next.
 */
class ReducedSearch {
public:
    // Attempt to improve the current work cube for n=4 or n=5.
    // Returns additional moves applied (empty if no improvement found / n other).
    static std::vector<Move> improve(Cube& work, int maxDepth = 12);

private:
    static bool isNearlyReduced(const Cube& c);
    static int heuristic(const Cube& c);
    static std::vector<Move> generateMoves(int n);
    static bool ida(Cube& work, int depth, int threshold,
                    int lastFace, std::vector<Move>& path);
};
