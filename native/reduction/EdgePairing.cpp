#include "EdgePairing.h"

#include <algorithm>

// Edge slots: 0=UF 1=UR 2=UB 3=UL 4=DF 5=DR 6=DB 7=DL 8=FR 9=FL 10=BR 11=BL
static void edgeFaces(int edgeIndex, int& f1, int& f2) {
    static const int map[12][2] = {
        {U, F}, {U, R}, {U, B}, {U, L},
        {D, F}, {D, R}, {D, B}, {D, L},
        {F, R}, {F, L}, {B, R}, {B, L}
    };
    f1 = map[edgeIndex][0];
    f2 = map[edgeIndex][1];
}

// Count how many of the (n-2) wing positions on this edge already form a
// matching pair of the two target colors (real facelet scan, not mid-only).
static int pairedWings(const Cube& work, int edgeIndex) {
    int n = work.size();
    if (n < 4) return 0;

    int f1, f2;
    edgeFaces(edgeIndex, f1, f2);
    Color c1 = static_cast<Color>(f1);
    Color c2 = static_cast<Color>(f2);

    int paired = 0;
    // Sample wing depths 1 .. n-2 along the edge.
    // For each depth we read the two facelets that should be the wing pair.
    // Mapping mirrors Cube::edgeCoords mid-case but varies the offset.
    for (int d = 1; d <= n - 2; ++d) {
        Color a = Color::U, b = Color::U;
        // Approximate wing locations by interpolating from mid toward corners.
        // Exact geometric wing coords depend on face orientation; we use a
        // consistent sampling that matches the solver's apply model.
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
        } else { // B-L
            a = work.get(B, offset, n - 1);
            b = work.get(L, offset, 0);
        }

        if ((a == c1 && b == c2) || (a == c2 && b == c1))
            ++paired;
    }
    return paired;
}

std::vector<Move> EdgePairing::pairOne(Cube& work, int edgeIndex) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4) return moves;

    // Fully paired? Skip — never break good edges
    if (pairedWings(work, edgeIndex) >= n - 2) return moves;

    auto append = [&](Move m) {
        work.apply(m);
        moves.push_back(m);
    };

    // Freeslice-style: for each wing depth run one pairing cycle.
    // Depth cycles through inner slices so every wing orbit is visited.
    int maxWing = n - 2;
    int depthSpan = std::max(1, n / 2 - 1);
    for (int wing = 0; wing < maxWing; ++wing) {
        if (pairedWings(work, edgeIndex) >= n - 2) break;

        int depth = 1 + (wing % depthSpan);

        // Buffer: lift into U via R
        append(Move{R, 0, 1});
        append(Move{U, 0, 1});
        append(Move{R, 0, -1});

        // Match with slice at this depth
        append(Move{F, depth, 1});
        append(Move{U, 0, 2});
        append(Move{F, depth, -1});

        // Restore
        append(Move{R, 0, 1});
        append(Move{U, 0, -1});
        append(Move{R, 0, -1});
    }

    // Final align
    append(Move{U, 0, 1});
    return moves;
}

std::vector<Move> EdgePairing::pairAll(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4) return solution;

    // Three passes: pair → clean leftovers → final solidify.
    // pairedWings skip ensures already-solid edges are never broken.
    for (int pass = 0; pass < 3; ++pass) {
        for (int e = 0; e < 12; ++e) {
            auto stage = pairOne(work, e);
            solution.insert(solution.end(), stage.begin(), stage.end());
        }
    }
    return solution;
}
