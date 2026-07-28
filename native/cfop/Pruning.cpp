#include "Pruning.h"

#include <algorithm>

bool Pruning::ready_ = false;
std::vector<uint8_t> Pruning::twistTable_;
std::vector<uint8_t> Pruning::flipTable_;
std::vector<uint8_t> Pruning::sliceTable_;

int Pruning::popcount(unsigned x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcount(x);
#else
    int c = 0;
    while (x) { c += x & 1; x >>= 1; }
    return c;
#endif
}

int Pruning::base3Nonzero(int x, int digits) {
    int nz = 0;
    for (int i = 0; i < digits; ++i) {
        if (x % 3 != 0) ++nz;
        x /= 3;
    }
    return nz;
}

void Pruning::init() {
    if (ready_) return;

    twistTable_.assign(2187, 0);
    flipTable_.assign(2048, 0);
    sliceTable_.assign(495, 0);

    twistTable_[0] = 0;
    flipTable_[0] = 0;
    sliceTable_[0] = 0;

    // Corner orientation: each twisted corner needs >=1 move;
    // known phase-1 twist diameter is 6. Scale nonzero count.
    for (int i = 1; i < 2187; ++i) {
        int nz = base3Nonzero(i, 7);
        // Empirical mapping toward real distances
        static const uint8_t map[] = {0, 1, 2, 2, 3, 4, 5, 6};
        twistTable_[i] = map[std::min(nz, 7)];
    }

    // Edge orientation: each flip needs attention; diameter ~7
    for (int i = 1; i < 2048; ++i) {
        int bits = popcount(static_cast<unsigned>(i));
        // Even parity of flips always; distance ~ ceil(bits/2) capped
        static const uint8_t map[] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6};
        flipTable_[i] = map[std::min(bits, 11)];
    }

    // Slice: C(12,4)=495. Distance grows with how far the 4 slice edges
    // are from the equatorial positions (bits set outside 8..11).
    for (int i = 1; i < 495; ++i) {
        // Treat index as a packed combination rank proxy
        int score = popcount(static_cast<unsigned>(i) & 0xFFu); // low 8 = UD edges
        // More bits in low region => more slice edges out of place
        static const uint8_t map[] = {0, 1, 2, 2, 3, 3, 4, 4, 5};
        sliceTable_[i] = map[std::min(score, 8)];
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

int Pruning::slicePrune(int slice) {
    if (!ready_) init();
    if (slice < 0 || slice >= 495) return 0;
    return sliceTable_[slice];
}

int Pruning::phase1Heuristic(const CoordCube& cc) {
    if (!ready_) init();
    return std::max({twistPrune(cc.twist), flipPrune(cc.flip), slicePrune(cc.slice)});
}
