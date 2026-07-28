#include "MoveTables.h"
#include "Cube.h"
#include "CoordCube.h"

bool MoveTables::ready_ = false;
std::vector<int16_t> MoveTables::twistMove_;
std::vector<int16_t> MoveTables::flipMove_;
std::vector<int16_t> MoveTables::sliceMove_;

int MoveTables::moveFace(int m) { return m / 3; }

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

    // Mark which entries we've filled (for denser coverage)
    std::vector<uint8_t> seenTwist(TWIST_N, 0), seenFlip(FLIP_N, 0), seenSlice(SLICE_N, 0);

    auto record = [&](const Cube& c) {
        CoordCube before = CoordCube::fromCube(c);
        int tw = before.twist % TWIST_N;
        int fl = before.flip % FLIP_N;
        int sl = before.slice % SLICE_N;
        seenTwist[tw] = 1;
        seenFlip[fl] = 1;
        seenSlice[sl] = 1;

        for (int m = 0; m < NUM_MOVES; ++m) {
            Cube tmp = c;
            tmp.apply(Move{moveFace(m), 0, moveTurns(m)});
            CoordCube after = CoordCube::fromCube(tmp);
            twistMove_[tw * NUM_MOVES + m] = static_cast<int16_t>(after.twist % TWIST_N);
            flipMove_[fl * NUM_MOVES + m]   = static_cast<int16_t>(after.flip % FLIP_N);
            sliceMove_[sl * NUM_MOVES + m] = static_cast<int16_t>(after.slice % SLICE_N);
        }
    };

    // 1) Solved
    record(Cube(3));

    // 2) Single moves + two-move sequences from solved
    for (int m1 = 0; m1 < NUM_MOVES; ++m1) {
        Cube c1(3);
        c1.apply(Move{moveFace(m1), 0, moveTurns(m1)});
        record(c1);
        for (int m2 = 0; m2 < NUM_MOVES; ++m2) {
            if (moveFace(m1) == moveFace(m2)) continue;
            Cube c2 = c1;
            c2.apply(Move{moveFace(m2), 0, moveTurns(m2)});
            record(c2);
        }
    }

    // 3) Dense random walks covering more of the graph
    for (int walk = 0; walk < 25000; ++walk) {
        Cube c(3);
        int len = 1 + (walk % 20);
        for (int s = 0; s < len; ++s) {
            int m = (walk * 17 + s * 11 + 3) % NUM_MOVES;
            c.apply(Move{moveFace(m), 0, moveTurns(m)});
        }
        record(c);
    }

    // 4) From each seen twist/flip/slice we already filled all 18 moves
    // Unseen coords keep 0 (identity transition) as safe fallback.
    (void)seenTwist;
    (void)seenFlip;
    (void)seenSlice;
}

void MoveTables::init() {
    if (ready_) return;
    buildFromFacelets();
    ready_ = true;
}
