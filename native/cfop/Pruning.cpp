#include "Pruning.h"
#include "MoveTables.h"

#include <algorithm>
#include <queue>

bool Pruning::ready_ = false;
std::vector<uint8_t> Pruning::twistTable_;
std::vector<uint8_t> Pruning::flipTable_;
std::vector<uint8_t> Pruning::sliceTable_;

int Pruning::popcount(unsigned x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_popcount(x);
#else
    int c = 0; while (x) { c += x & 1; x >>= 1; } return c;
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

    MoveTables::init();

    twistTable_.assign(2187, 15);
    flipTable_.assign(2048, 15);
    sliceTable_.assign(495, 15);

    // ---- BFS on twist alone ----
    {
        std::queue<int> q;
        twistTable_[0] = 0;
        q.push(0);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            int d = twistTable_[v];
            if (d >= 11) continue;
            for (int m = 0; m < MoveTables::NUM_MOVES; ++m) {
                int n = MoveTables::twistMove(v, m);
                if (n < 0 || n >= 2187) continue;
                if (twistTable_[n] > d + 1) {
                    twistTable_[n] = static_cast<uint8_t>(d + 1);
                    q.push(n);
                }
            }
        }
        // Fill any unreachable with combinatorial fallback
        for (int i = 0; i < 2187; ++i) {
            if (twistTable_[i] >= 15) {
                int nz = base3Nonzero(i, 7);
                static const uint8_t map[] = {0,1,2,2,3,4,5,6};
                twistTable_[i] = map[std::min(nz, 7)];
            }
        }
    }

    // ---- BFS on flip alone ----
    {
        std::queue<int> q;
        flipTable_[0] = 0;
        q.push(0);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            int d = flipTable_[v];
            if (d >= 10) continue;
            for (int m = 0; m < MoveTables::NUM_MOVES; ++m) {
                int n = MoveTables::flipMove(v, m);
                if (n < 0 || n >= 2048) continue;
                if (flipTable_[n] > d + 1) {
                    flipTable_[n] = static_cast<uint8_t>(d + 1);
                    q.push(n);
                }
            }
        }
        for (int i = 0; i < 2048; ++i) {
            if (flipTable_[i] >= 15) {
                int bits = popcount(static_cast<unsigned>(i));
                static const uint8_t map[] = {0,1,1,2,2,3,3,4,4,5,5,6};
                flipTable_[i] = map[std::min(bits, 11)];
            }
        }
    }

    // ---- BFS on slice alone ----
    {
        std::queue<int> q;
        sliceTable_[0] = 0;
        q.push(0);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            int d = sliceTable_[v];
            if (d >= 8) continue;
            for (int m = 0; m < MoveTables::NUM_MOVES; ++m) {
                int n = MoveTables::sliceMove(v, m);
                if (n < 0 || n >= 495) continue;
                if (sliceTable_[n] > d + 1) {
                    sliceTable_[n] = static_cast<uint8_t>(d + 1);
                    q.push(n);
                }
            }
        }
        for (int i = 0; i < 495; ++i) {
            if (sliceTable_[i] >= 15) {
                int score = popcount(static_cast<unsigned>(i) & 0xFFu);
                static const uint8_t map[] = {0,1,2,2,3,3,4,4,5};
                sliceTable_[i] = map[std::min(score, 8)];
            }
        }
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
    int tw = cc.twist % 2187;
    int fl = cc.flip % 2048;
    int sl = cc.slice % 495;
    return std::max({twistPrune(tw), flipPrune(fl), slicePrune(sl)});
}
