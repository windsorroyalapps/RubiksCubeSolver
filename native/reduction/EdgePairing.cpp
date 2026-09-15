#include "EdgePairing.h"

#include <algorithm>
#include <array>

static constexpr int kBufferEdge = 0;

static void edgeFaces(int edgeIndex, int& f1, int& f2) {
    static const int map[12][2] = {
        {U, F}, {U, R}, {U, B}, {U, L},
        {D, F}, {D, R}, {D, B}, {D, L},
        {F, R}, {F, L}, {B, R}, {B, L}
    };
    f1 = map[edgeIndex][0];
    f2 = map[edgeIndex][1];
}

int EdgePairing::pairedWings(const Cube& work, int edgeIndex) {
    int n = work.size();
    if (n < 4) return 0;

    int f1, f2;
    edgeFaces(edgeIndex, f1, f2);
    Color c1 = static_cast<Color>(f1);
    Color c2 = static_cast<Color>(f2);

    int paired = 0;
    for (int d = 1; d <= n - 2; ++d) {
        Color a = Color::U, b = Color::U;
        int mid = n / 2;
        int offset = (d <= mid) ? d : (n - 1 - d);

        if ((f1 == U && f2 == F) || (f1 == F && f2 == U)) {
            a = work.get(U, n - 1, offset);
            b = work.get(F, 0, offset);
        } else if ((f1 == U && f2 == R) || (f1 == R && f2 == U)) {
            a = work.get(U, offset, n - 1);
            b = work.get(R, 0, offset);
        } else if ((f1 == U && f2 == B) || (f1 == B && f2 == U)) {
            a = work.get(U, 0, offset);
            b = work.get(B, 0, offset);
        } else if ((f1 == U && f2 == L) || (f1 == L && f2 == U)) {
            a = work.get(U, offset, 0);
            b = work.get(L, 0, offset);
        } else if ((f1 == D && f2 == F) || (f1 == F && f2 == D)) {
            a = work.get(D, 0, offset);
            b = work.get(F, n - 1, offset);
        } else if ((f1 == D && f2 == R) || (f1 == R && f2 == D)) {
            a = work.get(D, offset, n - 1);
            b = work.get(R, n - 1, offset);
        } else if ((f1 == D && f2 == B) || (f1 == B && f2 == D)) {
            a = work.get(D, n - 1, offset);
            b = work.get(B, n - 1, offset);
        } else if ((f1 == D && f2 == L) || (f1 == L && f2 == D)) {
            a = work.get(D, offset, 0);
            b = work.get(L, n - 1, offset);
        } else if ((f1 == F && f2 == R) || (f1 == R && f2 == F)) {
            a = work.get(F, offset, n - 1);
            b = work.get(R, offset, 0);
        } else if ((f1 == F && f2 == L) || (f1 == L && f2 == F)) {
            a = work.get(F, offset, 0);
            b = work.get(L, offset, n - 1);
        } else if ((f1 == B && f2 == R) || (f1 == R && f2 == B)) {
            a = work.get(B, offset, 0);
            b = work.get(R, offset, n - 1);
        } else {
            a = work.get(B, offset, n - 1);
            b = work.get(L, offset, 0);
        }

        if ((a == c1 && b == c2) || (a == c2 && b == c1))
            ++paired;
    }
    return paired;
}

int EdgePairing::leftoverUnpairedWings(const Cube& work) {
    int n = work.size();
    if (n < 4) return 0;
    int need = n - 2;
    int leftover = 0;
    for (int e = 0; e < 12; ++e) {
        int p = pairedWings(work, e);
        if (p < need) leftover += (need - p);
    }
    return leftover;
}

// Tight 8-move wing commutator family. Fewer, higher-quality variants.
// Designed to cycle two wings at a given inner-slice depth while protecting solid edges.
static std::vector<Move> depthCommutator(int depth, int variant) {
    std::vector<Move> seq;
    switch (variant % 4) {
        case 0: // classic R U R' F_d U2 F_d' R U' R'
            seq = {
                {R, 0, 1}, {U, 0, 1}, {R, 0, -1},
                {F, depth, 1}, {U, 0, 2}, {F, depth, -1},
                {R, 0, 1}, {U, 0, -1}, {R, 0, -1}
            };
            break;
        case 1: // L' U' L F_d' U2 F_d L' U L
            seq = {
                {L, 0, -1}, {U, 0, -1}, {L, 0, 1},
                {F, depth, -1}, {U, 0, 2}, {F, depth, 1},
                {L, 0, -1}, {U, 0, 1}, {L, 0, 1}
            };
            break;
        case 2: // B U B' R_d U2 R_d' B U' B'
            seq = {
                {B, 0, 1}, {U, 0, 1}, {B, 0, -1},
                {R, depth, 1}, {U, 0, 2}, {R, depth, -1},
                {B, 0, 1}, {U, 0, -1}, {B, 0, -1}
            };
            break;
        default: // pure 8-move A B A' B' style on R + F_d
            seq = {
                {R, 0, 1}, {F, depth, 1}, {R, 0, -1}, {F, depth, -1},
                {R, 0, 1}, {F, depth, 1}, {R, 0, -1}, {F, depth, -1}
            };
            break;
    }
    return seq;
}

std::vector<Move> EdgePairing::pairOne(Cube& work, int edgeIndex,
                                       const std::bitset<12>& solid) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4) return moves;
    if (solid.test(edgeIndex) || isSolid(work, edgeIndex))
        return moves;

    auto append = [&](Move m) {
        work.apply(m);
        moves.push_back(m);
    };
    auto appendSeq = [&](const std::vector<Move>& seq) {
        for (const auto& m : seq) append(m);
    };

    int maxWing = n - 2;
    int depthSpan = std::max(1, n / 2 - 1);

    // Targeted primary pass: limited budget, early exit on solid
    const int budget = maxWing * 3;  // hard limit to kill spam
    for (int wing = 0; wing < budget && !isSolid(work, edgeIndex); ++wing) {
        int depth = 1 + (wing % depthSpan);
        int variant = (wing / depthSpan) % 4;
        auto seq = depthCommutator(depth, variant);
        appendSeq(seq);
        // re-check solid after every commutator
        if (isSolid(work, edgeIndex)) break;
    }

    // Single buffer rotation if still not solid and not already buffer
    if (edgeIndex != kBufferEdge && !isSolid(work, edgeIndex))
        append(Move{U, 0, 1});

    return moves;
}

std::vector<Move> EdgePairing::pairAll(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4) return solution;

    std::bitset<12> solid;
    static const int order[12] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0  // non-buffer first
    };

    // Main pairing passes — early global stop
    for (int pass = 0; pass < 4; ++pass) {
        if (leftoverUnpairedWings(work) == 0) break;

        for (int e = 0; e < 12; ++e) {
            if (isSolid(work, e))
                solid.set(e);
        }

        for (int i = 0; i < 12; ++i) {
            if (leftoverUnpairedWings(work) == 0) break;
            int e = order[i];
            if (solid.test(e)) continue;
            auto stage = pairOne(work, e, solid);
            solution.insert(solution.end(), stage.begin(), stage.end());
            if (isSolid(work, e))
                solid.set(e);
        }
    }

    // Light post-repair: only if leftover remains, max 2 rounds, 4 variants
    int leftover = leftoverUnpairedWings(work);
    if (leftover > 0) {
        for (int repair = 0; repair < 2 && leftoverUnpairedWings(work) > 0; ++repair) {
            for (int e = 0; e < 12; ++e) {
                if (isSolid(work, e)) continue;
                int n = work.size();
                int depthSpan = std::max(1, n / 2 - 1);
                for (int d = 1; d <= depthSpan; ++d) {
                    auto seq = depthCommutator(d, repair + e);
                    for (const auto& m : seq) {
                        work.apply(m);
                        solution.push_back(m);
                    }
                    if (isSolid(work, e)) break;
                }
            }
        }
    }

    return solution;
}
