#include "EdgePairing.h"

#include <algorithm>

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

static Move inverse(const Move& m) {
    return Move{m.face, m.depth, -m.turns};
}

/** Apply seq; keep if pairedWings(edge) increases, else undo via inverses. */
static bool trySequence(Cube& work, int edgeIndex, const std::vector<Move>& seq,
                        std::vector<Move>& out) {
    if (seq.empty()) return false;
    const int before = EdgePairing::pairedWings(work, edgeIndex);
    for (const Move& m : seq)
        work.apply(m);
    const int after = EdgePairing::pairedWings(work, edgeIndex);
    if (after > before) {
        out.insert(out.end(), seq.begin(), seq.end());
        return true;
    }
    for (int i = static_cast<int>(seq.size()) - 1; i >= 0; --i)
        work.apply(inverse(seq[static_cast<size_t>(i)]));
    return false;
}

static bool sameAxisOpposite(int f1, int f2) {
    return (f1 ^ 1) == f2 && (f1 / 2) == (f2 / 2);
}

/**
 * Wing commutator variants: A = outer face turn (depth 0), B = inner slice
 * at exact depth d. Emit A B A' B' and B A B' A' (4-move), plus 8-move
 * doubles and conjugated X A B A' B' X' A Ap forms that stay 8 moves.
 */
static void appendWingCommutators(int aFace, int bFace, int depth, int turns,
                                  std::vector<std::vector<Move>>& out) {
    const Move A{aFace, 0, turns};
    const Move B{bFace, depth, turns};
    const Move Ap = inverse(A);
    const Move Bp = inverse(B);

    // 4-move commutators: A B A' B' and B A B' A'
    out.push_back({A, B, Ap, Bp});
    out.push_back({B, A, Bp, Ap});

    // 8-move: double application of each 4-move form
    out.push_back({A, B, Ap, Bp, A, B, Ap, Bp});
    out.push_back({B, A, Bp, Ap, B, A, Bp, Ap});

    // 8-move interleaved: B A C A' B' A C' A'  (slice + two outers)
    // and conjugate X A B A' B' X' (6) kept as a lighter option.
    static const int kFaces[6] = {U, D, F, B, L, R};
    for (int cFace : kFaces) {
        if (cFace == aFace || cFace == bFace) continue;
        if (sameAxisOpposite(cFace, aFace) || sameAxisOpposite(cFace, bFace)) continue;
        const Move C{cFace, 0, turns};
        const Move Cp = inverse(C);
        out.push_back({B, A, C, Ap, Bp, A, Cp, Ap});
        out.push_back({A, B, C, Bp, Ap, B, Cp, Bp});
        const Move X{cFace, 0, 1};
        const Move Xp = inverse(X);
        out.push_back({X, A, B, Ap, Bp, Xp});
        break; // one helper face per (A,B,turns)
    }
}

static std::vector<std::vector<Move>> wingCommutatorsForDepth(int n, int depth,
                                                              int preferA,
                                                              int preferB) {
    std::vector<std::vector<Move>> variants;
    if (depth < 1 || depth > n - 2) return variants;

    auto addPair = [&](int aFace, int bFace) {
        for (int turns : {1, -1, 2})
            appendWingCommutators(aFace, bFace, depth, turns, variants);
    };

    // Prefer edge-local faces first
    if (preferA >= 0 && preferB >= 0) {
        addPair(preferA, preferB);
        addPair(preferB, preferA);
    }

    static const int kOuter[6] = {U, D, F, B, L, R};
    for (int aFace : kOuter) {
        for (int bFace : kOuter) {
            if (aFace == bFace) continue;
            if (sameAxisOpposite(aFace, bFace)) continue;
            if ((aFace == preferA && bFace == preferB) ||
                (aFace == preferB && bFace == preferA))
                continue;
            addPair(aFace, bFace);
        }
    }
    return variants;
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

std::vector<Move> EdgePairing::pairOne(Cube& work, int edgeIndex,
                                       const std::bitset<12>& solid) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4) return moves;
    if (solid.test(edgeIndex) || isSolid(work, edgeIndex))
        return moves;

    int ef1, ef2;
    edgeFaces(edgeIndex, ef1, ef2);

    for (int depth = 1; depth <= n - 2; ++depth) {
        if (isSolid(work, edgeIndex)) break;
        auto variants = wingCommutatorsForDepth(n, depth, ef1, ef2);
        // Prefer shorter sequences first
        std::stable_sort(variants.begin(), variants.end(),
                         [](const std::vector<Move>& a, const std::vector<Move>& b) {
                             return a.size() < b.size();
                         });
        for (const auto& seq : variants) {
            if (isSolid(work, edgeIndex)) break;
            trySequence(work, edgeIndex, seq, moves);
        }
    }

    if (!isSolid(work, edgeIndex) && edgeIndex != kBufferEdge) {
        Move u{U, 0, 1};
        work.apply(u);
        moves.push_back(u);
    }
    return moves;
}

static std::vector<Move> repairLeftovers(Cube& work, std::bitset<12>& solid) {
    std::vector<Move> moves;
    const int n = work.size();
    if (n < 4) return moves;

    static const int order[12] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0
    };

    for (int round = 0; round < 24; ++round) {
        const int beforeLeftover = EdgePairing::leftoverUnpairedWings(work);
        if (beforeLeftover == 0) break;

        bool progress = false;
        for (int i = 0; i < 12; ++i) {
            const int e = order[i];
            if (EdgePairing::isSolid(work, e)) {
                solid.set(e);
                continue;
            }

            int ef1, ef2;
            edgeFaces(e, ef1, ef2);

            for (int depth = 1; depth <= n - 2; ++depth) {
                if (EdgePairing::isSolid(work, e)) break;
                auto variants = wingCommutatorsForDepth(n, depth, ef1, ef2);
                std::stable_sort(variants.begin(), variants.end(),
                                 [](const std::vector<Move>& a, const std::vector<Move>& b) {
                                     return a.size() < b.size();
                                 });
                for (const auto& seq : variants) {
                    if (EdgePairing::isSolid(work, e)) break;
                    if (trySequence(work, e, seq, moves))
                        progress = true;
                }
            }
            if (EdgePairing::isSolid(work, e))
                solid.set(e);
        }

        const int afterLeftover = EdgePairing::leftoverUnpairedWings(work);
        if (!progress || afterLeftover >= beforeLeftover)
            break;
    }
    return moves;
}

std::vector<Move> EdgePairing::pairAll(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4) return solution;

    std::bitset<12> solid;
    static const int order[12] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0
    };

    for (int pass = 0; pass < 4; ++pass) {
        if (leftoverUnpairedWings(work) == 0) break;

        for (int e = 0; e < 12; ++e) {
            if (isSolid(work, e))
                solid.set(e);
        }

        for (int i = 0; i < 12; ++i) {
            if (leftoverUnpairedWings(work) == 0) break;
            int e = order[i];
            auto stage = pairOne(work, e, solid);
            solution.insert(solution.end(), stage.begin(), stage.end());
            if (isSolid(work, e))
                solid.set(e);
        }
    }

    auto repair = repairLeftovers(work, solid);
    solution.insert(solution.end(), repair.begin(), repair.end());
    return solution;
}
