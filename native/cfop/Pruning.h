#pragma once

#include "CoordCube.h"
#include <cstdint>
#include <vector>

/**
 * Lightweight pruning for Kociemba IDA*.
 * Full tables are huge (generated offline); here we keep small
 * pattern databases that still cut search dramatically.
 */
class Pruning {
public:
    // Minimum moves to solve twist (corner orientation) alone
    static int twistPrune(int twist);

    // Minimum moves to solve flip (edge orientation) alone
    static int flipPrune(int flip);

    // Combined phase-1 heuristic (max of components — admissible)
    static int phase1Heuristic(const CoordCube& cc);

    // Init small baked tables (call once)
    static void init();

private:
    static bool ready_;
    // Compact tables: index -> lower bound depth
    static std::vector<uint8_t> twistTable_; // size 2187
    static std::vector<uint8_t> flipTable_;  // size 2048
};
