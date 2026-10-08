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
 * - pairAll stops when leftoverUnpairedWings()==0
 * - 2026-10-07: source/dest facelet map (face,row,col) drives setup+commutator
 * - 2026-10-08: flip-aware setup. Orientation +1 is home order, -1 is flipped.
 *   A flipped source gets a quarter-slice before the commutator; undone on no gain.
 * - 2026-10-09: if the single quarter does not raise pairedWings, try a 2-move
 *   setup (outer quarter of the source first face, then slice quarter on the
 *   second face). Both undone exactly on no gain. Solid bitset still protected.
 */
struct WingFacelet {
    int face;
    int row;
    int col;
};

class EdgePairing {
public:
    static std::vector<Move> pairAll(Cube& work);
    static int leftoverUnpairedWings(const Cube& work);

    // Count matching wing pairs on this edge (real facelet scan)
    static int pairedWings(const Cube& work, int edgeIndex);

    // Both stickers of the wing at depth d on edgeIndex (same coords as colour scan).
    static void wingFacelets(int n, int edgeIndex, int depth,
                             WingFacelet& a, WingFacelet& b);

    // +1 home order (a==face1 colour, b==face2 colour), -1 flipped, 0 not this pair.
    static int wingOrientation(const Cube& work, int edgeIndex, int depth);

    static bool isSolid(const Cube& work, int edgeIndex) {
        return pairedWings(work, edgeIndex) >= work.size() - 2;
    }

private:
    static std::vector<Move> pairOne(Cube& work, int edgeIndex,
                                     const std::bitset<12>& solid);
};
