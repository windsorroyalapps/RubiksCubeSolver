#pragma once

#include "Cube.h"
#include <vector>
#include <cstdint>
#include <unordered_map>

/**
 * Reduced-coordinate search for small nxn (4x4 / 5x5).
 *
 * After centers + edges (+ parity) the cube is nearly a 3x3 with residual
 * inner-slice freedom. This module runs a depth-limited IDA*-style search
 * over outer + curated inner moves to shorten the remaining path before the
 * classic 3x3 stage, pushing constructive lengths toward community OBTM
 * ceilings (~40–54 for 4×4).
 *
 * 2026-08-06: packed 4x4 center residual (uint16_t bitmask of 16 inner cells)
 * + stronger multi-wing residual heuristic.
 * 2026-08-07: full multi-depth wing residual sampling (all depths 1..n-2 on
 * representative edges) + tighter heuristic + higher 4x4 depthCap.
 * 2026-08-09: stronger inverse-face pruning + higher 4x4 depthCap (18) + residual
 * state packing scaffold for future bidirectional IDA*. Full bidirectional search
 * + complete residual coordinate tables remain highest leverage for OBTM ≤54.
 * 2026-08-10: inverse pruning refinement + bidirectional meet-in-middle scaffold comments.
 * 2026-08-11: depthCap 20 for 4x4, cleaned inverse pruning, residual packing ready for
 * true bidirectional meet-in-middle (next highest leverage). Exact g(n) n≥4 still open;
 * constructive algorithm complete for any n>3.
 * 2026-08-12: residualKey (uint64_t centers+wings pack) + true bidirectional
 * meet-in-middle prototype for 4x4 residual (forward/backward BFS meet on key);
 * IDA* still primary for mobile; MITM used when residual small. Highest leverage
 * remaining for collapsing constructive lengths toward community OBTM ≤54.
 */
class ReducedSearch {
public:
    // Attempt to improve the current work cube for n=4 or n=5.
    // Returns additional moves applied (empty if no improvement found / n other).
    static std::vector<Move> improve(Cube& work, int maxDepth = 14);

private:
    static bool isNearlyReduced(const Cube& c);
    static int heuristic(const Cube& c);
    static uint16_t pack4x4Centers(const Cube& c);  // 16-bit mask of incorrect centers
    static int wingResidual(const Cube& c);
    // Compact residual fingerprint for meet-in-middle / hashing (4x4 focused).
    // High 16 bits: pack4x4Centers; low bits: wing residual samples packed.
    static uint64_t residualKey(const Cube& c);
    static std::vector<Move> generateMoves(int n);
    static bool ida(Cube& work, int depth, int threshold,
                    int lastFace, int lastTurns, std::vector<Move>& path);
    // Bidirectional meet-in-middle on residualKey (4x4). Returns path if found
    // within depthCap/2 each side; empty otherwise. Mobile-safe node budget.
    static std::vector<Move> meetInMiddle(Cube& work, int depthCap);
};
