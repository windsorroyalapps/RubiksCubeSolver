#pragma once

#include "Cube.h"
#include <vector>

/**
 * Even-order cubes (4x4, 6x6, ...) can show two extra parities
 * after reduction that never appear on odd-order or 3x3:
 *
 * 1. OLL parity  – single edge flipped (relative to 3x3)
 * 2. PLL parity  – two edges swapped (or equivalent)
 *
 * These are fixed with known algorithms before the final 3x3 stage.
 */
class ParityHandler {
public:
    // Detect + fix both parities. Returns moves applied.
    static std::vector<Move> fix(Cube& work);

private:
    static bool hasOLLParity(const Cube& c);
    static bool hasPLLParity(const Cube& c);

    static std::vector<Move> fixOLLParity(Cube& work);
    static std::vector<Move> fixPLLParity(Cube& work);
};
