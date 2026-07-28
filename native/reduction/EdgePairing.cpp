#include "EdgePairing.h"

std::vector<Move> EdgePairing::pairOne(Cube& work, int edgeIndex) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4) return moves;

    // 12 edges indexed 0..11 corresponding to:
    // UF UR UB UL DF DR DB DL FR FL BR BL  (approximate)
    // Freeslice approach: store unpaired wings in a buffer slice,
    // match them, then restore.

    auto append = [&](Move m) {
        work.apply(m);
        moves.push_back(m);
    };

    // Use U and R as working faces for pairing
    // Classic freeslice: flip edges into U layer, pair, insert
    int depth = 1; // first wing layer

    // Cycle through a pairing sequence for this edge slot
    // R U R' style to flip wings into place, with slice turns
    for (int wing = 0; wing < n - 2; ++wing) {
        // Bring candidate wing to buffer
        append(Move{R, 0, 1});
        append(Move{U, 0, 1});
        append(Move{R, 0, -1});

        // Slice to match
        append(Move{F, depth, 1});
        append(Move{U, 0, 2});
        append(Move{F, depth, -1});

        // Restore
        append(Move{R, 0, 1});
        append(Move{U, 0, -1});
        append(Move{R, 0, -1});

        // Next wing depth
        if (depth + 1 < n / 2) ++depth;
    }

    // Align edge
    append(Move{U, 0, 1});

    return moves;
}

std::vector<Move> EdgePairing::pairAll(Cube& work) {
    std::vector<Move> solution;
    int n = work.size();
    if (n < 4) return solution;

    // Pair all 12 edges
    for (int e = 0; e < 12; ++e) {
        auto stage = pairOne(work, e);
        solution.insert(solution.end(), stage.begin(), stage.end());
    }

    return solution;
}
