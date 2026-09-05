#pragma once

#include "Cube.h"
#include "BoundHarness.h"
#include <vector>

/**
 * Stage-length cap against Ucas per-stage budgets.
 *
 * NEXT.md 2026-09-05 item 2: if overC or overE > 0, keep only budgeted
 * length from that stage and prefer 8-move commutators for remaining cells.
 * This is a *solver budget* tool, not a diameter proof.
 */
class StageCap {
public:
    /** 8-move commutator [A,B] = A B A' B' expanded as two quarter-pairs. */
    static std::vector<Move> eightMoveCommutator(const Move& a, const Move& b);

    /** Keep at most `budget` moves. Prefer dropping trailing inner-slice noise. */
    static std::vector<Move> capToBudget(const std::vector<Move>& seq, int budget);

    static int budgetCenters(int n) { return BoundHarness::cascadeStageBudget(n).centers; }
    static int budgetEdges(int n) { return BoundHarness::cascadeStageBudget(n).edges; }
};
