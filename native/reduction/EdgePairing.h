#pragma once

#include "Cube.h"
#include <vector>
#include <bitset>

/**
 * Pair wing edges so each of the 12 edges becomes a solid "dedge"
 * (wing pieces on that edge share the same color pair — floating OK).
 * Home-face placement is left to the virtual 3x3 stage.
 *
 * Supports freeslice-style pairing for n >= 4 with Yau-style buffer tracking:
 * - StageCap leftoverE still measures home UF/UB/DF/DB strips for harness
 * - Solid edges (pairedWings == n-2) are preferred not to break
 * - pairAll stops when leftoverUnpairedWings()==0 (all 12 floating-paired)
 */
class EdgePairing {
public:
    static std::vector<Move> pairAll(Cube& work);
    static int leftoverUnpairedWings(const Cube& work);

    // Count matching floating wing slots on this edge (same color pair)
    static int pairedWings(const Cube& work, int edgeIndex);

    static bool isSolid(const Cube& work, int edgeIndex) {
        return pairedWings(work, edgeIndex) >= work.size() - 2;
    }

private:
    static std::vector<Move> pairOne(Cube& work, int edgeIndex,
                                     const std::bitset<12>& solid);
};
