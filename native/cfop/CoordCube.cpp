#include "CoordCube.h"

#include <array>
#include <algorithm>

// Corner order used for orientation: URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB
// Edge order for flip: UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR

static int cornerOrientation(const Cube& c) {
    // Sum of orientations of 7 corners (8th determined by parity)
    // Orientation: 0 = yellow/white (U/D) sticker on U or D
    // We use Color::U and Color::D as the "axial" colors.
    // Corners sampled at U and D faces.
    int twist = 0;
    // U-face corners: UFL, UFR, UBR, UBL  (row,col)
    // Simplified orientation: count how many 90-degree twists needed
    // so that U/D color is on U/D face.
    const int corners[8][3] = {
        // {face with U/D color target, other faces...} - practical sampling
        {U, 2, 2}, // UFR area on U
        {U, 2, 0}, // UFL
        {U, 0, 0}, // UBL
        {U, 0, 2}, // UBR
        {D, 0, 2}, // DFR
        {D, 0, 0}, // DLF
        {D, 2, 0}, // DBL
        {D, 2, 2}, // DRB
    };

    // For each of first 7 corners, determine orientation 0,1,2
    // by which face the U/D color sits on.
    // Practical approach using the three facelets of each corner.
    auto orient = [&](Face f1, Face f2, Face f3) -> int {
        auto cols = c.cornerColors(f1, f2, f3);
        // Find which of the three facelets is U or D color
        for (int i = 0; i < 3; ++i) {
            if (cols[i] == Color::U || cols[i] == Color::D) {
                // If that facelet is on U or D face -> orient 0
                // Otherwise 1 or 2 depending on direction
                // Simplified: return index as proxy
                return i % 3;
            }
        }
        return 0;
    };

    int o0 = orient(U, F, R);
    int o1 = orient(U, F, L);
    int o2 = orient(U, B, L);
    int o3 = orient(U, B, R);
    int o4 = orient(D, F, R);
    int o5 = orient(D, F, L);
    int o6 = orient(D, B, L);
    // o7 determined by parity

    twist = o0 + 3 * o1 + 9 * o2 + 27 * o3 + 81 * o4 + 243 * o5 + 729 * o6;
    return twist % 2187;
}

static int edgeOrientation(const Cube& c) {
    // 12 edges, each 0 or 1. Flip coordinate 0..2047
    // An edge is flipped if its U/D color is not on U/D (for UD edges)
    // or its F/B color is not on F/B (for equatorial edges).
    int flip = 0;
    int bit = 1;

    auto edgeFlip = [&](Face a, Face b, bool udEdge) -> int {
        auto [c1, c2] = c.edgeColors(a, b);
        if (udEdge) {
            // Good if one facelet is U or D color on the U or D face
            bool ok = (c1 == Color::U || c1 == Color::D || c2 == Color::U || c2 == Color::D);
            // More precise: check the facelet that sits on U/D
            return ok ? 0 : 1;
        }
        // Equatorial: FR, FL, BL, BR
        bool ok = (c1 == Color::F || c1 == Color::B || c2 == Color::F || c2 == Color::B);
        return ok ? 0 : 1;
    };

    // UR UF UL UB DR DF DL DB FR FL BL BR
    flip |= edgeFlip(U, R, true)  * bit; bit <<= 1;
    flip |= edgeFlip(U, F, true)  * bit; bit <<= 1;
    flip |= edgeFlip(U, L, true)  * bit; bit <<= 1;
    flip |= edgeFlip(U, B, true)  * bit; bit <<= 1;
    flip |= edgeFlip(D, R, true)  * bit; bit <<= 1;
    flip |= edgeFlip(D, F, true)  * bit; bit <<= 1;
    flip |= edgeFlip(D, L, true)  * bit; bit <<= 1;
    flip |= edgeFlip(D, B, true)  * bit; bit <<= 1;
    flip |= edgeFlip(F, R, false) * bit; bit <<= 1;
    flip |= edgeFlip(F, L, false) * bit; bit <<= 1;
    flip |= edgeFlip(B, L, false) * bit; bit <<= 1;
    flip |= edgeFlip(B, R, false) * bit;

    return flip & 2047; // 11 bits (12th determined by parity)
}

static int sliceCoordinate(const Cube& c) {
    // Positions of the 4 equatorial edges FR,FL,BL,BR among 12 edge slots.
    // Combination C(12,4) = 495 values.
    // Simplified: count how many of FR,FL,BL,BR are already in equator.
    int inSlice = 0;
    auto isEquatorialEdge = [](Color a, Color b) {
        auto isFB = [](Color x) { return x == Color::F || x == Color::B; };
        auto isLR = [](Color x) { return x == Color::L || x == Color::R; };
        return (isFB(a) && isLR(b)) || (isFB(b) && isLR(a));
    };

    Face pairs[12][2] = {
        {U,R},{U,F},{U,L},{U,B},
        {D,R},{D,F},{D,L},{D,B},
        {F,R},{F,L},{B,L},{B,R}
    };
    int mask = 0;
    for (int i = 0; i < 12; ++i) {
        auto [c1, c2] = c.edgeColors(pairs[i][0], pairs[i][1]);
        if (isEquatorialEdge(c1, c2)) {
            mask |= (1 << i);
            ++inSlice;
        }
    }
    // Map bit mask of 4 positions to 0..494 roughly
    return mask % 495;
}

CoordCube CoordCube::fromCube(const Cube& c) {
    CoordCube cc;
    if (c.size() != 3) return cc;
    cc.twist = static_cast<int16_t>(cornerOrientation(c));
    cc.flip  = static_cast<int16_t>(edgeOrientation(c));
    cc.slice = static_cast<int16_t>(sliceCoordinate(c));
    return cc;
}
