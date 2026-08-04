#pragma once

#include "Cube.h"
#include <vector>
#include <bitset>

/**
 * Pair wing edges so each of the 12 edges becomes a solid "dedge"
 * (all wing pieces matching), reducing the cube to a 3x3.
 *
 * Supports freeslice-style pairing for n >= 4 with Yau-style buffer tracking:
 * - Explicit buffer edge (UF = 0) holds temporary wings
 * - Solid edges (pairedWings == n-2) are never touched again
 * - Cross edges prioritized first (Yau spirit for large n)
 */
class EdgePairing {
public:
    static std::vector<Move> pairAll(Cube& work);

private:
    // Pair one edge (match all wings on that edge slot), respecting solid set
    static std::vector<Move> pairOne(Cube& work, int edgeIndex,
                                     const std::bitset<12>& solid);

    // Count matching wing pairs on this edge (real facelet scan)
    static int pairedWings(const Cube& work, int edgeIndex);

    // True if edge is fully solid
    static bool isSolid(const Cube& work, int edgeIndex) {
        return pairedWings(work, edgeIndex) >= work.size() - 2;
    }
};
