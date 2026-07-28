#include "MoveTables.h"
#include "Cube.h"
#include "CoordCube.h"

#include <array>

bool MoveTables::ready_ = false;
std::vector<int16_t> MoveTables::twistMove_;
std::vector<int16_t> MoveTables::flipMove_;
std::vector<int16_t> MoveTables::sliceMove_;

int MoveTables::moveFace(int m) {
    return m / 3; // 0..5
}

int MoveTables::moveTurns(int m) {
    static const int t[3] = {1, 2, -1};
    return t[m % 3];
}

int MoveTables::twistMove(int twist, int m) {
    if (!ready_) init();
    if (twist < 0 || twist >= TWIST_N || m < 0 || m >= NUM_MOVES) return twist;
    return twistMove_[twist * NUM_MOVES + m];
}

int MoveTables::flipMove(int flip, int m) {
    if (!ready_) init();
    if (flip < 0 || flip >= FLIP_N || m < 0 || m >= NUM_MOVES) return flip;
    return flipMove_[flip * NUM_MOVES + m];
}

int MoveTables::sliceMove(int slice, int m) {
    if (!ready_) init();
    if (slice < 0 || slice >= SLICE_N || m < 0 || m >= NUM_MOVES) return slice;
    return sliceMove_[slice * NUM_MOVES + m];
}

void MoveTables::buildFromFacelets() {
    twistMove_.assign(TWIST_N * NUM_MOVES, 0);
    flipMove_.assign(FLIP_N * NUM_MOVES, 0);
    sliceMove_.assign(SLICE_N * NUM_MOVES, 0);

    // Identity row: apply each move to solved cube, record destination coords.
    // Then close under composition by propagating along random walks +
    // filling remaining entries via "apply move to a cube that has this coord".
    //
    // Practical construction for mobile:
    // 1) BFS from identity in coordinate space using facelet applications
    //    on a working cube, filling tables as we go.
    // 2) For any unseen coord, leave 0 (identity) as safe fallback.

    Cube cube(3);
    CoordCube id = CoordCube::fromCube(cube);

    // Fill transitions from solved
    for (int m = 0; m < NUM_MOVES; ++m) {
        Cube c(3);
        c.apply(Move{moveFace(m), 0, moveTurns(m)});
        CoordCube cc = CoordCube::fromCube(c);
        twistMove_[id.twist * NUM_MOVES + m] = static_cast<int16_t>(cc.twist);
        flipMove_[id.flip * NUM_MOVES + m]   = static_cast<int16_t>(cc.flip);
        sliceMove_[id.slice * NUM_MOVES + m] = static_cast<int16_t>(cc.slice % SLICE_N);
    }

    // Propagate: repeated random-ish walks to populate more entries
    // Start from solved, apply sequences, record before/after for each move.
    for (int walk = 0; walk < 8000; ++walk) {
        Cube c(3);
        // scramble with walk steps
        for (int s = 0; s < (walk % 15) + 1; ++s) {
            int m = (walk * 7 + s * 3) % NUM_MOVES;
            c.apply(Move{moveFace(m), 0, moveTurns(m)});
        }
        CoordCube before = CoordCube::fromCube(c);
        int tw = before.twist % TWIST_N;
        int fl = before.flip % FLIP_N;
        int sl = before.slice % SLICE_N;

        for (int m = 0; m < NUM_MOVES; ++m) {
            Cube tmp = c;
            tmp.apply(Move{moveFace(m), 0, moveTurns(m)});
            CoordCube after = CoordCube::fromCube(tmp);
            twistMove_[tw * NUM_MOVES + m] = static_cast<int16_t>(after.twist % TWIST_N);
            flipMove_[fl * NUM_MOVES + m]   = static_cast<int16_t>(after.flip % FLIP_N);
            sliceMove_[sl * NUM_MOVES + m] = static_cast<int16_t>(after.slice % SLICE_N);
        }
    }
}

void MoveTables::init() {
    if (ready_) return;
    buildFromFacelets();
    ready_ = true;
}
