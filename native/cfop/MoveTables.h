#pragma once

#include <cstdint>
#include <vector>

/**
 * Precomputed coordinate transition tables for Kociemba phase 1.
 *
 * 18 moves = 6 faces * 3 turns (1, 2, -1).
 * Indices: twist 0..2186, flip 0..2047, slice 0..494.
 *
 * Built once via MoveTables::init() from the facelet model,
 * then IDA* steps in pure integer space (fast path to ~20 moves).
 */
class MoveTables {
public:
    static constexpr int NUM_MOVES = 18;
    static constexpr int TWIST_N   = 2187;
    static constexpr int FLIP_N    = 2048;
    static constexpr int SLICE_N   = 495;

    // move -> face (0..5), turns (1,2,-1)
    static int moveFace(int m);
    static int moveTurns(int m);

    static int twistMove(int twist, int m);
    static int flipMove(int flip, int m);
    static int sliceMove(int slice, int m);

    static void init();
    static bool ready() { return ready_; }

private:
    static bool ready_;
    static std::vector<int16_t> twistMove_; // [TWIST_N * NUM_MOVES]
    static std::vector<int16_t> flipMove_;
    static std::vector<int16_t> sliceMove_;

    static void buildFromFacelets();
};
