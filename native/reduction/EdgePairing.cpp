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

void EdgePairing::wingFacelets(int n, int edgeIndex, int d,
                               WingFacelet& wa, WingFacelet& wb) {
    int f1, f2;
    edgeFaces(edgeIndex, f1, f2);
    int mid = n / 2;
    int offset = (d <= mid) ? d : (n - 1 - d);
    wa = {f1, 0, 0};
    wb = {f2, 0, 0};

    if ((f1 == U && f2 == F) || (f1 == F && f2 == U)) {
        wa = {U, n - 1, offset};
        wb = {F, 0, offset};
    } else if ((f1 == U && f2 == R) || (f1 == R && f2 == U)) {
        wa = {U, offset, n - 1};
        wb = {R, 0, offset};
    } else if ((f1 == U && f2 == B) || (f1 == B && f2 == U)) {
        wa = {U, 0, offset};
        wb = {B, 0, offset};
    } else if ((f1 == U && f2 == L) || (f1 == L && f2 == U)) {
        wa = {U, offset, 0};
        wb = {L, 0, offset};
    } else if ((f1 == D && f2 == F) || (f1 == F && f2 == D)) {
        wa = {D, 0, offset};
        wb = {F, n - 1, offset};
    } else if ((f1 == D && f2 == R) || (f1 == R && f2 == D)) {
        wa = {D, offset, n - 1};
        wb = {R, n - 1, offset};
    } else if ((f1 == D && f2 == B) || (f1 == B && f2 == D)) {
        wa = {D, n - 1, offset};
        wb = {B, n - 1, offset};
    } else if ((f1 == D && f2 == L) || (f1 == L && f2 == D)) {
        wa = {D, offset, 0};
        wb = {L, n - 1, offset};
    } else if ((f1 == F && f2 == R) || (f1 == R && f2 == F)) {
        wa = {F, offset, n - 1};
        wb = {R, offset, 0};
    } else if ((f1 == F && f2 == L) || (f1 == L && f2 == F)) {
        wa = {F, offset, 0};
        wb = {L, offset, n - 1};
    } else if ((f1 == B && f2 == R) || (f1 == R && f2 == B)) {
        wa = {B, offset, 0};
        wb = {R, offset, n - 1};
    } else {
        wa = {B, offset, n - 1};
        wb = {L, offset, 0};
    }
}

static void wingColorsAt(const Cube& work, int edgeIndex, int d, Color& a, Color& b) {
    WingFacelet wa, wb;
    EdgePairing::wingFacelets(work.size(), edgeIndex, d, wa, wb);
    a = work.get(static_cast<Face>(wa.face), wa.row, wa.col);
    b = work.get(static_cast<Face>(wb.face), wb.row, wb.col);
}

int EdgePairing::wingOrientation(const Cube& work, int edgeIndex, int depth) {
    int f1, f2;
    edgeFaces(edgeIndex, f1, f2);
    Color a, b;
    wingColorsAt(work, edgeIndex, depth, a, b);
    Color c1 = static_cast<Color>(f1);
    Color c2 = static_cast<Color>(f2);
    if (a == c1 && b == c2) return 1;
    if (a == c2 && b == c1) return -1;
    return 0;
}

int EdgePairing::pairedWings(const Cube& work, int edgeIndex) {
    int n = work.size();
    if (n < 4) return 0;
    int paired = 0;
    for (int d = 1; d <= n - 2; ++d) {
        if (wingOrientation(work, edgeIndex, d) != 0)
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

static std::vector<Move> depthCommutator(int depth, int variant) {
    std::vector<Move> seq;
    switch (variant % 12) {
        case 0:
            seq = {
                {R, 0, 1}, {U, 0, 1}, {R, 0, -1},
                {F, depth, 1}, {U, 0, 2}, {F, depth, -1},
                {R, 0, 1}, {U, 0, -1}, {R, 0, -1}
            };
            break;
        case 1:
            seq = {
                {L, 0, -1}, {U, 0, -1}, {L, 0, 1},
                {F, depth, -1}, {U, 0, 2}, {F, depth, 1},
                {L, 0, -1}, {U, 0, 1}, {L, 0, 1}
            };
            break;
        case 2:
            seq = {
                {B, 0, 1}, {U, 0, 1}, {B, 0, -1},
                {R, depth, 1}, {U, 0, 2}, {R, depth, -1},
                {B, 0, 1}, {U, 0, -1}, {B, 0, -1}
            };
            break;
        case 3:
            seq = {
                {R, 0, 1}, {F, depth, 1}, {R, 0, -1}, {F, depth, -1},
                {R, 0, 1}, {F, depth, 1}, {R, 0, -1}, {F, depth, -1}
            };
            break;
        case 4:
            seq = {
                {F, 0, 1}, {U, 0, 1}, {F, 0, -1},
                {R, depth, 1}, {U, 0, 2}, {R, depth, -1},
                {F, 0, 1}, {U, 0, -1}, {F, 0, -1}
            };
            break;
        case 5:
            seq = {
                {R, 0, -1}, {U, 0, -1}, {R, 0, 1},
                {B, depth, -1}, {U, 0, 2}, {B, depth, 1},
                {R, 0, -1}, {U, 0, 1}, {R, 0, 1}
            };
            break;
        case 6:
            seq = {
                {L, 0, 1}, {U, 0, 1}, {L, 0, -1},
                {F, depth, 1}, {U, 0, 2}, {F, depth, -1},
                {L, 0, 1}, {U, 0, -1}, {L, 0, -1}
            };
            break;
        case 7:
            seq = {
                {L, 0, 1}, {B, depth, 1}, {L, 0, -1}, {B, depth, -1},
                {L, 0, 1}, {B, depth, 1}, {L, 0, -1}, {B, depth, -1}
            };
            break;
        case 8:
            seq = {
                {D, 0, 1}, {U, 0, 1}, {D, 0, -1},
                {F, depth, 1}, {U, 0, 2}, {F, depth, -1},
                {D, 0, 1}, {U, 0, -1}, {D, 0, -1}
            };
            break;
        case 9:
            seq = {
                {F, 0, 1}, {R, depth, 1}, {F, 0, -1}, {R, depth, -1},
                {F, 0, 1}, {R, depth, 1}, {F, 0, -1}, {R, depth, -1}
            };
            break;
        case 10:
            seq = {
                {B, 0, -1}, {U, 0, -1}, {B, 0, 1},
                {L, depth, -1}, {U, 0, 2}, {L, depth, 1},
                {B, 0, -1}, {U, 0, 1}, {B, 0, 1}
            };
            break;
        default:
            seq = {
                {R, 0, 1}, {B, depth, 1}, {R, 0, -1}, {B, depth, -1},
                {R, 0, 1}, {B, depth, 1}, {R, 0, -1}, {B, depth, -1}
            };
            break;
    }
    return seq;
}

static std::vector<int> unpairedDepths(const Cube& work, int edgeIndex) {
    std::vector<int> depths;
    int n = work.size();
    if (n < 4) return depths;
    for (int d = 1; d <= n - 2; ++d) {
        if (EdgePairing::wingOrientation(work, edgeIndex, d) == 0)
            depths.push_back(d);
    }
    return depths;
}

struct WingHit {
    int edge;
    int orient; // +1 home colours in face order, -1 flipped
};

static std::vector<WingHit> locateTargetWings(const Cube& work, int destEdge, int depth) {
    std::vector<WingHit> sources;
    int f1, f2;
    edgeFaces(destEdge, f1, f2);
    Color c1 = static_cast<Color>(f1);
    Color c2 = static_cast<Color>(f2);
    for (int e = 0; e < 12; ++e) {
        if (e == destEdge) continue;
        Color a, b;
        wingColorsAt(work, e, depth, a, b);
        if (a == c1 && b == c2) sources.push_back({e, 1});
        else if (a == c2 && b == c1) sources.push_back({e, -1});
    }
    return sources;
}

// Outer setup that tries to bring a located source edge toward the UF buffer.
static Move setupFromSource(int sourceEdge, int turnIndex) {
    static const Face faces[12] = {
        U, U, U, U,
        D, D, D, D,
        F, F, B, B
    };
    static const int turns[4] = {1, -1, 2, 1};
    Face face = faces[sourceEdge % 12];
    int turnsAmt = turns[turnIndex % 4];
    if (sourceEdge >= 8 && turnIndex % 2 == 1)
        face = (sourceEdge % 2 == 0) ? R : L;
    return Move{face, 0, turnsAmt};
}

// Quarter-slice on the source face. Undone if the commutator does not raise pairedWings.
static Move flipSliceSetup(int sourceEdge, int depth) {
    int f1, f2;
    edgeFaces(sourceEdge, f1, f2);
    Face slice = static_cast<Face>(f2);
    int turn = (sourceEdge % 2 == 0) ? 1 : -1;
    return Move{slice, depth, turn};
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
    auto undoLast = [&](int count) {
        for (int i = 0; i < count; ++i) {
            if (moves.empty()) break;
            Move m = moves.back();
            moves.pop_back();
            work.apply(Move{m.face, m.depth, m.turns == 2 ? 2 : -m.turns});
        }
    };

    int maxWing = n - 2;
    int depthSpan = std::max(1, n / 2 - 1);
    int stagnant = 0;
    const int stagnantLimit = 4;

    static const Face setupFaces[] = {U, D, L, R};
    static const int setupTurns[] = {0, 1, -1, 2};

    auto targets = unpairedDepths(work, edgeIndex);
    if (!targets.empty()) {
        for (int t = 0; t < (int)targets.size() && !isSolid(work, edgeIndex); ++t) {
            int depth = targets[t];
            int before = pairedWings(work, edgeIndex);
            auto sources = locateTargetWings(work, edgeIndex, depth);

            bool gainedDepth = false;
            int sourceTries = sources.empty() ? 1 : (int)sources.size();
            for (int si = 0; si < sourceTries && !isSolid(work, edgeIndex); ++si) {
                int sourceEdge = sources.empty() ? edgeIndex : sources[si].edge;
                int orient = sources.empty() ? 1 : sources[si].orient;
                if (!sources.empty() && solid.test(sourceEdge)) continue;

                for (int s = 0; s < 4 && !isSolid(work, edgeIndex); ++s) {
                    Move setup = sources.empty()
                        ? Move{setupFaces[s], 0, setupTurns[s]}
                        : setupFromSource(sourceEdge, s);
                    int setupCount = 0;
                    if (setup.turns != 0) {
                        append(setup);
                        setupCount = 1;
                    }
                    // Flipped colour pair: quarter the source slice, then the commutator.
                    if (orient < 0) {
                        append(flipSliceSetup(sourceEdge, depth));
                        ++setupCount;
                    }
                    bool gained = false;
                    int variantBase = sourceEdge * 3 + t + s + (orient < 0 ? 6 : 0);
                    for (int v = 0; v < 4 && !isSolid(work, edgeIndex); ++v) {
                        auto seq = depthCommutator(depth, variantBase + v);
                        int seqLen = (int)seq.size();
                        appendSeq(seq);
                        int after = pairedWings(work, edgeIndex);
                        if (after > before) {
                            before = after;
                            stagnant = 0;
                            gained = true;
                            gainedDepth = true;
                            break;
                        } else {
                            undoLast(seqLen);
                            ++stagnant;
                            if (stagnant >= stagnantLimit) break;
                        }
                    }
                    if (!gained && setupCount > 0)
                        undoLast(setupCount);
                    if (gained) break;
                    if (stagnant >= stagnantLimit) break;
                }
                if (gainedDepth || stagnant >= stagnantLimit) break;
            }
            if (stagnant >= stagnantLimit) break;
        }
    }

    stagnant = 0;
    const int budget = maxWing * 3;
    int before = pairedWings(work, edgeIndex);
    for (int wing = 0; wing < budget && !isSolid(work, edgeIndex); ++wing) {
        int depth = 1 + (wing % depthSpan);
        int variant = (wing / depthSpan) % 12;
        // Alternate a flip-slice attempt on odd wings when the slot is still empty.
        int pre = 0;
        if ((wing % 2) == 1 && wingOrientation(work, edgeIndex, depth) == 0) {
            append(flipSliceSetup(edgeIndex, depth));
            pre = 1;
        }
        auto seq = depthCommutator(depth, variant);
        int seqLen = (int)seq.size();
        appendSeq(seq);
        int after = pairedWings(work, edgeIndex);
        if (after > before) {
            before = after;
            stagnant = 0;
        } else {
            undoLast(seqLen + pre);
            ++stagnant;
            if (stagnant >= stagnantLimit * 2) break;
        }
        if (isSolid(work, edgeIndex)) break;
    }

    // No trailing no-gain U: that move unpaired already-solid wings.
    (void)kBufferEdge;
    return moves;
}

std::vector<Move> EdgePairing::pairAll(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4) return solution;

    std::bitset<12> solid;
    static const int order[12] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0
    };

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

    int leftover = leftoverUnpairedWings(work);
    if (leftover > 0) {
        for (int repair = 0; repair < 4 && leftoverUnpairedWings(work) > 0; ++repair) {
            for (int e = 0; e < 12; ++e) {
                if (isSolid(work, e)) continue;
                int n = work.size();
                int depthSpan = std::max(1, n / 2 - 1);
                for (int d = 1; d <= depthSpan; ++d) {
                    if (wingOrientation(work, e, d) == 0) {
                        Move flip = flipSliceSetup(e, d);
                        work.apply(flip);
                        solution.push_back(flip);
                    }
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
