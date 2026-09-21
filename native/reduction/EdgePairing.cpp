#include "EdgePairing.h"

#include <algorithm>
#include <array>
#include <vector>

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

// 12-variant 8-move wing commutator family (2026-09-19 Teegan session).
// Cycles two wings at a given inner-slice depth while protecting solid edges.
// Variants 0-7 prior, 8-11 additional orthogonal / slice-priority for better coverage.
static std::vector<Move> depthCommutator(int depth, int variant) {
    std::vector<Move> seq;
    switch (variant % 12) {
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
        case 3: // pure 8-move A B A' B' style on R + F_d
            seq = {
                {R, 0, 1}, {F, depth, 1}, {R, 0, -1}, {F, depth, -1},
                {R, 0, 1}, {F, depth, 1}, {R, 0, -1}, {F, depth, -1}
            };
            break;
        case 4: // F U F' R_d U2 R_d' F U' F'  (owning-face F oriented)
            seq = {
                {F, 0, 1}, {U, 0, 1}, {F, 0, -1},
                {R, depth, 1}, {U, 0, 2}, {R, depth, -1},
                {F, 0, 1}, {U, 0, -1}, {F, 0, -1}
            };
            break;
        case 5: // R' U' R B_d' U2 B_d R' U R
            seq = {
                {R, 0, -1}, {U, 0, -1}, {R, 0, 1},
                {B, depth, -1}, {U, 0, 2}, {B, depth, 1},
                {R, 0, -1}, {U, 0, 1}, {R, 0, 1}
            };
            break;
        case 6: // L U L' F_d U2 F_d' L U' L'
            seq = {
                {L, 0, 1}, {U, 0, 1}, {L, 0, -1},
                {F, depth, 1}, {U, 0, 2}, {F, depth, -1},
                {L, 0, 1}, {U, 0, -1}, {L, 0, -1}
            };
            break;
        case 7: // pure A B A' B' on L + B_d
            seq = {
                {L, 0, 1}, {B, depth, 1}, {L, 0, -1}, {B, depth, -1},
                {L, 0, 1}, {B, depth, 1}, {L, 0, -1}, {B, depth, -1}
            };
            break;
        case 8: // D U D' F_d U2 F_d' D U' D'  (D-oriented for lower edges)
            seq = {
                {D, 0, 1}, {U, 0, 1}, {D, 0, -1},
                {F, depth, 1}, {U, 0, 2}, {F, depth, -1},
                {D, 0, 1}, {U, 0, -1}, {D, 0, -1}
            };
            break;
        case 9: // pure on F + R_d
            seq = {
                {F, 0, 1}, {R, depth, 1}, {F, 0, -1}, {R, depth, -1},
                {F, 0, 1}, {R, depth, 1}, {F, 0, -1}, {R, depth, -1}
            };
            break;
        case 10: // B' U' B L_d' U2 L_d B' U B
            seq = {
                {B, 0, -1}, {U, 0, -1}, {B, 0, 1},
                {L, depth, -1}, {U, 0, 2}, {L, depth, 1},
                {B, 0, -1}, {U, 0, 1}, {B, 0, 1}
            };
            break;
        default: // pure on R + B_d
            seq = {
                {R, 0, 1}, {B, depth, 1}, {R, 0, -1}, {B, depth, -1},
                {R, 0, 1}, {B, depth, 1}, {R, 0, -1}, {B, depth, -1}
            };
            break;
    }
    return seq;
}

// Collect unpaired depth offsets for a given edge (facelet scan).
static std::vector<int> unpairedDepths(const Cube& work, int edgeIndex) {
    std::vector<int> depths;
    int n = work.size();
    if (n < 4) return depths;

    int f1, f2;
    edgeFaces(edgeIndex, f1, f2);
    Color c1 = static_cast<Color>(f1);
    Color c2 = static_cast<Color>(f2);

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

        if (!((a == c1 && b == c2) || (a == c2 && b == c1)))
            depths.push_back(d);
    }
    return depths;
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
    int stagnant = 0;
    const int stagnantLimit = 3;  // abort variant loops if no pairedWings gain

    // 2026-09-22 Teegan: progress-check + deeper unpairedDepths targeting
    // Real gate remains full source/dest facelet locator; this reduces spam.
    auto targets = unpairedDepths(work, edgeIndex);
    if (!targets.empty()) {
        for (int t = 0; t < (int)targets.size() && !isSolid(work, edgeIndex); ++t) {
            int depth = targets[t];
            int before = pairedWings(work, edgeIndex);
            for (int v = 0; v < 6 && !isSolid(work, edgeIndex); ++v) {
                auto seq = depthCommutator(depth, v + t * 2);
                appendSeq(seq);
                int after = pairedWings(work, edgeIndex);
                if (after > before) {
                    before = after;
                    stagnant = 0;
                } else {
                    ++stagnant;
                    if (stagnant >= stagnantLimit) break;
                }
            }
            if (stagnant >= stagnantLimit) break;
        }
    }

    // Fallback: broad coverage with 12 variants, hard budget + progress abort
    stagnant = 0;
    const int budget = maxWing * 4;  // tightened from *5
    int before = pairedWings(work, edgeIndex);
    for (int wing = 0; wing < budget && !isSolid(work, edgeIndex); ++wing) {
        int depth = 1 + (wing % depthSpan);
        int variant = (wing / depthSpan) % 12;
        auto seq = depthCommutator(depth, variant);
        appendSeq(seq);
        int after = pairedWings(work, edgeIndex);
        if (after > before) {
            before = after;
            stagnant = 0;
        } else {
            ++stagnant;
            if (stagnant >= stagnantLimit * 2) break;
        }
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

    // Main pairing passes — early global stop when leftover==0
    for (int pass = 0; pass < 6; ++pass) {
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

    // Light post-repair: only if leftover remains, max 4 rounds, 12 variants
    int leftover = leftoverUnpairedWings(work);
    if (leftover > 0) {
        for (int repair = 0; repair < 4 && leftoverUnpairedWings(work) > 0; ++repair) {
            for (int e = 0; e < 12; ++e) {
                if (isSolid(work, e)) continue;
                int n = work.size();
                int depthSpan = std::max(1, n / 2 - 1);
                for (int d = 1; d <= depthSpan; ++d) {
                    auto seq = depthCommutator(d, repair * 3 + e);
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
