#include "EdgePairing.h"

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

// How many wing positions on this edge already match both colors?
static int pairedWings(const Cube& work, int edgeIndex) {
    int n = work.size();
    if (n < 4) return 0;

    int f1, f2;
    edgeFaces(edgeIndex, f1, f2);
    Color c1 = static_cast<Color>(f1);
    Color c2 = static_cast<Color>(f2);

    // Sample wing facelets along the edge (indices 1..n-2)
    int good = 0;
    int mid = n / 2;

    // Simplified check using edgeColors at the logical edge
    auto [a, b] = work.edgeColors(static_cast<Face>(f1), static_cast<Face>(f2));
    if ((a == c1 && b == c2) || (a == c2 && b == c1)) {
        // Outer-style match; count wings as partially done
        good = n - 2; // treat as paired for heuristic skip
    }
    return good;
}

std::vector<Move> EdgePairing::pairOne(Cube& work, int edgeIndex) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4) return moves;

    // Already paired? Skip
    if (pairedWings(work, edgeIndex) >= n - 2) return moves;

    auto append = [&](Move m) {
        work.apply(m);
        moves.push_back(m);
    };

    // Freeslice: for each wing depth, attempt one pairing cycle
    // Only when this edge still needs work
    int maxWing = n - 2;
    for (int wing = 0; wing < maxWing; ++wing) {
        if (pairedWings(work, edgeIndex) >= n - 2) break;

        int depth = 1 + (wing % std::max(1, n / 2 - 1));

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

    // Align
    append(Move{U, 0, 1});
    return moves;
}

std::vector<Move> EdgePairing::pairAll(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4) return solution;

    // Two passes: first pass pairs what it can, second cleans leftovers
    for (int pass = 0; pass < 2; ++pass) {
        for (int e = 0; e < 12; ++e) {
            auto stage = pairOne(work, e);
            solution.insert(solution.end(), stage.begin(), stage.end());
        }
    }
    return solution;
}
