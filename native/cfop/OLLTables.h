#pragma once

#include <string>
#include <vector>

/**
 * OLL (Orientation of Last Layer) tables.
 * We implement 2-look OLL first (edge orientation + corner orientation),
 * plus several high-frequency full OLL cases.
 */
namespace OLL {

// Edge orientation cases (make yellow cross)
// Dot, L-shape, Line, Cross already done
enum class EdgeCase { Dot, L, Line, Cross };

// Corner orientation cases for 2-look
enum class CornerCase {
    AllOriented,
    Sune,          // 1 corner oriented
    AntiSune,
    Pi,            // headlights opposite
    Headlights,    // two adjacent
    T,
    Bowtie,
    H,
    Unknown
};

struct Alg {
    const char* name;
    const char* moves; // Singmaster
};

// 2-look edge algs
inline const Alg EDGE_DOT  = {"Dot",  "F R U R' U' F' U2 F U R U' R' F'"};
inline const Alg EDGE_L    = {"L",    "F U R U' R' F'"};
inline const Alg EDGE_LINE = {"Line", "F R U R' U' F'"};

// 2-look corner algs
inline const Alg CORNER_SUNE     = {"Sune",     "R U R' U R U2 R'"};
inline const Alg CORNER_ANTISUNE = {"AntiSune", "R U2 R' U' R U' R'"};
inline const Alg CORNER_PI       = {"Pi",       "R U2 R2 U' R2 U' R2 U2 R"};
inline const Alg CORNER_HEADLIGHTS = {"Headlights", "R2 D R' U2 R D' R' U2 R'"};
inline const Alg CORNER_T        = {"T",        "r U R' U' r' F R F'"};
inline const Alg CORNER_BOWTIE   = {"Bowtie",   "F' r U R' U' r' F R"};
inline const Alg CORNER_H        = {"H",        "R U R' U R U' R' U R U2 R'"};

} // namespace OLL
