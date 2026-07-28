#pragma once

#include "CoordCube.h"
#include <cstdint>
#include <vector>

/**
 * Pattern-database style pruning for Kociemba phase 1.
 *
 * Tables:
 *   twist  [2187]  - corner orientation distance
 *   flip   [2048]  - edge orientation distance
 *   slice  [495]   - UD-slice edge combination distance
 *
 * Heuristic = max(twist, flip, slice)  (admissible)
 *
 * Values are filled with a combinatorial lower-bound model that
 * correlates well with real God's distances for phase-1 coords.
 * Full move-table BFS can replace init() later without API change.
 */
class Pruning {
public:
    static int twistPrune(int twist);
    static int flipPrune(int flip);
    static int slicePrune(int slice);
    static int phase1Heuristic(const CoordCube& cc);

    static void init();

private:
    static bool ready_;
    static std::vector<uint8_t> twistTable_;
    static std::vector<uint8_t> flipTable_;
    static std::vector<uint8_t> sliceTable_;

    static int popcount(unsigned x);
    static int base3Nonzero(int x, int digits);
};
