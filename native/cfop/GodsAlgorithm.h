#pragma once

#include "Cube.h"
#include <string>
#include <vector>

/**
 * God's-algorithm oriented solver for 3x3 (half-turn metric).
 *
 * Target: every position solved in <= 20 moves (proven God's Number).
 *
 * Strategy:
 *  1. Multi-probe Kociemba phase-1 (several short paths into G1)
 *  2. Phase-2 for each probe; keep shortest total
 *  3. If still > 20, run bounded optimal IDA* from start
 *  4. CFOP only as last-resort fallback (should be rare)
 */
class GodsAlgorithm {
public:
    static constexpr int GODS_NUMBER = 20;

    static std::vector<Move> solve(const Cube& cube);
    static std::string solveToNotation(const Cube& cube);

private:
    // Single Kociemba attempt with optional max phase-1 depth
    static std::vector<Move> kociembaOnce(const Cube& cube, int phase1MaxDepth);

    // Try several phase-1 depths / probes; return shortest
    static std::vector<Move> multiProbeKociemba(const Cube& cube);

    // Bounded optimal search if length still > 20
    static std::vector<Move> optimalCleanup(const Cube& cube, int maxDepth);
};
