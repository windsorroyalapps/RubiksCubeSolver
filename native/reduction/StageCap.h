#pragma once

#include "Cube.h"
#include "BoundHarness.h"
#include <utility>
#include <vector>

/**
 * Stage-length cap against Ucas per-stage budgets.
 *
 * After capToBudget, leftover unsolved center cells / unpaired wings get
 * one 8-move commutator each (never-break family). Measurement + repair,
 * not a diameter proof.
 */
class StageCap {
public:
    static std::vector<Move> eightMoveCommutator(const Move& a, const Move& b);

    /** Keep at most `budget` moves. */
    static std::vector<Move> capToBudget(const std::vector<Move>& seq, int budget);

    static int budgetCenters(int n) { return BoundHarness::cascadeStageBudget(n).centers; }
    static int budgetEdges(int n) { return BoundHarness::cascadeStageBudget(n).edges; }

    /** Interior facelets whose color != face center color. */
    static int leftoverCenterCells(const Cube& cube);

    /** Approximate unpaired wings: mismatched edge-strip pairs on U/D/F/B. */
    static int leftoverUnpairedWings(const Cube& cube);

    /**
     * After a clip, emit one 8-move commutator per leftover (capped at `maxLeftovers`).
     * A is inner-slice of the target face; B is an orthogonal quarter.
     */
    static std::vector<Move> leftoverCommutators(const Cube& cube, int leftoverCount, int maxLeftovers = 64);

    /** Cap raw stage then append leftover commutators. Returns {seq, leftoverCount}. */
    static std::pair<std::vector<Move>, int> capThenRepair(
        const std::vector<Move>& raw, const Cube& afterRawApply, int budget, bool centersNotEdges);
};
