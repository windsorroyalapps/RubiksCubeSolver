#include "Pruning.h"

#include <algorithm>
#include <queue>

bool Pruning::ready_ = false;
std::vector<uint8_t> Pruning::twistTable_;
std::vector<uint8_t> Pruning::flipTable_;

void Pruning::init() {
    if (ready_) return;

    // Allocate
    twistTable_.assign(2187, 15); // 15 = unknown / high
    flipTable_.assign(2048, 15);

    // Identity is depth 0
    twistTable_[0] = 0;
    flipTable_[0] = 0;

    // Lightweight approximation without full move tables:
    // mark small neighborhoods. Production would BFS with real move graphs.
    // Here we fill a distance estimate: non-zero coords need at least 1-4 moves.
    for (int i = 1; i < 2187; ++i) {
        // Rough: number of non-trivial base-3 digits / 2
        int x = i, nz = 0;
        for (int k = 0; k < 7; ++k) {
            if (x % 3 != 0) ++nz;
            x /= 3;
        }
        twistTable_[i] = static_cast<uint8_t>(std::min(11, (nz + 1) / 1));
    }
    for (int i = 1; i < 2048; ++i) {
        int bits = __builtin_popcount(static_cast<unsigned>(i));
        flipTable_[i] = static_cast<uint8_t>(std::min(10, (bits + 1) / 2));
    }

    ready_ = true;
}

int Pruning::twistPrune(int twist) {
    if (!ready_) init();
    if (twist < 0 || twist >= 2187) return 0;
    return twistTable_[twist];
}

int Pruning::flipPrune(int flip) {
    if (!ready_) init();
    if (flip < 0 || flip >= 2048) return 0;
    return flipTable_[flip];
}

int Pruning::phase1Heuristic(const CoordCube& cc) {
    if (!ready_) init();
    int h = std::max(twistPrune(cc.twist), flipPrune(cc.flip));
    if (cc.slice != 0) h = std::max(h, 1);
    return h;
}
