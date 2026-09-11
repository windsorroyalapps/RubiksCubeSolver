#include "StageCap.h"

static Move invertMove(const Move& m) {
    Move inv = m;
    if (m.turns == 1) inv.turns = 3;
    else if (m.turns == 3 || m.turns == -1) inv.turns = 1;
    return inv;
}

std::vector<Move> StageCap::eightMoveCommutator(const Move& a, const Move& b) {
    std::vector<Move> seq;
    seq.reserve(8);
    Move ap = invertMove(a);
    Move bp = invertMove(b);
    seq.push_back(a);
    seq.push_back(b);
    seq.push_back(ap);
    seq.push_back(bp);
    seq.push_back(a);
    seq.push_back(b);
    seq.push_back(ap);
    seq.push_back(bp);
    return seq;
}

std::vector<Move> StageCap::capToBudget(const std::vector<Move>& seq, int budget) {
    if (budget < 0) budget = 0;
    if (static_cast<int>(seq.size()) <= budget) return seq;
    return std::vector<Move>(seq.begin(), seq.begin() + budget);
}

static Color faceCenterColor(const Cube& cube, int face) {
    const int n = cube.size();
    return cube.get(face, n / 2, n / 2);
}

int StageCap::leftoverCenterCells(const Cube& cube) {
    const int n = cube.size();
    if (n < 4) return 0;
    int bad = 0;
    for (int f = 0; f < 6; ++f) {
        Color want = faceCenterColor(cube, f);
        for (int r = 1; r < n - 1; ++r) {
            for (int c = 1; c < n - 1; ++c) {
                if (cube.get(f, r, c) != want) ++bad;
            }
        }
    }
    return bad;
}

int StageCap::leftoverUnpairedWings(const Cube& cube) {
    const int n = cube.size();
    if (n < 4) return 0;
    int bad = 0;
    auto stripMismatch = [&](int fa, int ra, int fb, int rb) {
        for (int i = 1; i < n - 1; ++i) {
            Color a = cube.get(fa, ra, i);
            Color b = cube.get(fb, rb, i);
            if (a != static_cast<Color>(fa) || b != static_cast<Color>(fb)) ++bad;
        }
    };
    stripMismatch(U, n - 1, F, 0);
    stripMismatch(U, 0, B, 0);
    stripMismatch(D, 0, F, n - 1);
    stripMismatch(D, n - 1, B, n - 1);
    return bad;
}

std::vector<Move> StageCap::leftoverCommutators(const Cube& cube, int leftoverCount, int maxLeftovers) {
    std::vector<Move> out;
    if (leftoverCount <= 0) return out;
    const int n = cube.size();
    const int k = leftoverCount < maxLeftovers ? leftoverCount : maxLeftovers;
    out.reserve(static_cast<size_t>(k) * 8);
    const int facesA[] = {R, U, F, L, D, B};
    const int facesB[] = {U, F, R, D, B, L};
    const int depth = n > 3 ? 1 : 0;
    for (int i = 0; i < k; ++i) {
        Move a{facesA[i % 6], depth, 1};
        Move b{facesB[i % 6], 0, 1};
        auto comm = eightMoveCommutator(a, b);
        out.insert(out.end(), comm.begin(), comm.end());
    }
    return out;
}

static int measure(const Cube& cube, bool centersNotEdges) {
    return centersNotEdges ? StageCap::leftoverCenterCells(cube)
                           : StageCap::leftoverUnpairedWings(cube);
}

std::pair<std::vector<Move>, int> StageCap::capThenRepair(
    const std::vector<Move>& raw, const Cube& preStage, int budget, bool centersNotEdges) {
    Cube afterFull = preStage;
    afterFull.apply(raw);
    const int leftoverFull = measure(afterFull, centersNotEdges);

    auto capped = capToBudget(raw, budget);
    Cube afterCap = preStage;
    afterCap.apply(capped);
    const int leftoverCap = measure(afterCap, centersNotEdges);

    if (leftoverCap > leftoverFull) {
        return {raw, leftoverFull};
    }
    if (leftoverCap == 0) {
        return {capped, 0};
    }

    auto repair = leftoverCommutators(afterCap, leftoverCap);
    Cube afterRepair = afterCap;
    afterRepair.apply(repair);
    const int leftoverRep = measure(afterRepair, centersNotEdges);
    // Edge repair must not destroy solved centers.
    const bool centersOk = centersNotEdges ||
        StageCap::leftoverCenterCells(afterRepair) <= StageCap::leftoverCenterCells(afterCap);
    if (leftoverRep < leftoverCap && centersOk) {
        capped.insert(capped.end(), repair.begin(), repair.end());
        return {capped, leftoverRep};
    }

    if (leftoverFull <= leftoverCap) return {raw, leftoverFull};
    return {capped, leftoverCap};
}
