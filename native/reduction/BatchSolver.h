#pragma once

#include "Cube.h"
#include <vector>

/**
 * Demaine-inspired batching layer for nxn reduction.
 *
 * Naive reduction: O(1) moves per cluster => O(n^2) total.
 * Parallel insight: one slice turn affects Theta(n) pieces;
 * when many clusters "want" the same move, do it once.
 *
 * BatchSolver groups pending face/depth/turn requests and
 * emits each unique move only once per batch window.
 * That is a practical step toward O(n^2 / log n)-style savings.
 */
class BatchSolver {
public:
    // Deduplicate consecutive identical moves and merge runs
    static std::vector<Move> compress(const std::vector<Move>& raw);

    // Batch: partition sequence into windows; within each window,
    // keep only unique (face, depth, turns) while preserving a
    // stable order (first occurrence wins).
    static std::vector<Move> batchWindow(const std::vector<Move>& raw, int windowSize);

    // Full post-process: compress + multi-pass window batching
    static std::vector<Move> optimize(const std::vector<Move>& raw);

    // Estimate "work units" for logging / comparison to n^2 vs n^2/log n
    static int estimatedClusters(int n);   // Theta(n^2)
    static double asymptoticTarget(int n); // c * n^2 / log(n)
};
