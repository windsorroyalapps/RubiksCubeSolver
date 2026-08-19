#pragma once

#include "Cube.h"
#include <string>
#include <vector>

/**
 * Bound harness for working backward toward God's Number on nxn.
 *
 * Constructive upper U(n) (reduction-style formula):
 *   odd  n: 92n^2 - 307n + 113
 *   even n: 92n^2 - 307n + 257
 *
 * Asymptotic shape target: c * n^2 / ln(n)  (Demaine Theta)
 *   c ≈ 3.8 calibrated to community 4×4 (~40-48) / 5×5 (~55-70) estimates.
 *   Note: published 4×4 OBTM upper is 55 (not the same as this constructive U(4)=501).
 *
 * Dual metrics:
 *   SSTM  = single-slice / every Move counts 1
 *   OBTM  = outer-block: consecutive depth==0 same-face turns collapse to 1;
 *           every inner slice (depth>0) still counts 1
 *
 * Harness records per-stage move counts (SSTM) + per-stage OBTM and checks length <= U(n).
 * finalObtm lets us compare against the community 4×4 OBTM ≤55 ceiling.
 * Per-stage OBTM is the highest remaining diagnostic leverage: which phase is furthest from the ceiling.
 */
struct StageLengths {
    int centers = 0;
    int edges = 0;
    int parity = 0;
    int reduced = 0;   // ReducedSearch residual IDA* (4×4/5×5)
    int reduce3x3 = 0;
    int afterBatch = 0;  // final length after BatchSolver (SSTM-style)

    // Per-stage OBTM (computed on each stage subsequence independently)
    int centersObtm = 0;
    int edgesObtm = 0;
    int parityObtm = 0;
    int reducedObtm = 0;
    int reduce3x3Obtm = 0;

    // Optional dual-metric finals (filled by report when sequence available)
    int finalObtm = 0;
    int finalSstm = 0;

    int totalRaw() const { return centers + edges + parity + reduced + reduce3x3; }
    int totalFinal() const { return afterBatch > 0 ? afterBatch : totalRaw(); }
};

struct BoundReport {
    int n = 0;
    StageLengths stages;
    int constructiveUpper = 0;  // U(n)
    double asymptoticTarget = 0; // c * n^2 / ln(n)
    bool withinUpper = false;   // final <= U(n)
    double ratioToUpper = 0;    // final / U(n)
    double ratioToAsymptotic = 0;
    int obtm = 0;               // OBTM of final sequence (if provided)
    int sstm = 0;               // SSTM of final sequence

    std::string toString() const;
};

class BoundHarness {
public:
    // Constructive upper bound U(n)
    static int constructiveUpper(int n);

    // Demaine-shaped target (c≈3.8 fitted to community estimates)
    static double asymptoticTarget(int n);

    // Build report from measured stage lengths
    static BoundReport report(int n, const StageLengths& stages);

    // Build report and attach dual metrics from the final move sequence
    static BoundReport report(int n, const StageLengths& stages,
                              const std::vector<Move>& finalSeq);

    // Convenience: count moves in a sequence (SSTM)
    static int count(const std::vector<Move>& moves) {
        return static_cast<int>(moves.size());
    }

    // Single-slice turn metric: every Move is one turn
    static int countSstm(const std::vector<Move>& moves) {
        return static_cast<int>(moves.size());
    }

    // Outer-block turn metric approximation:
    // consecutive depth==0 moves on the same face collapse into one block turn;
    // every inner-slice (depth > 0) still counts as its own move.
    static int countObtm(const std::vector<Move>& moves);
};
