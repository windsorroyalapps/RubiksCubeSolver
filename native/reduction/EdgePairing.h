#pragma once

#include "Cube.h"
#include <vector>

/**
 * Pair wing edges so each of the 12 edges becomes a solid "dedge"
 * (all wing pieces matching), reducing the cube to a 3x3.
 *
 * Supports freeslice-style pairing for n >= 4.
 */
class EdgePairing {
public:
    static std::vector<Move> pairAll(Cube& work);

private:
    // Pair one edge (match all wings on that edge slot)
    static std::vector<Move> pairOne(Cube& work, int edgeIndex);
};
