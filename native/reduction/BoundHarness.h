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
 *
 * Harness records per-stage move counts and checks length <= U(n).
 */
struct StageLengths {
    int centers = 0;
    int edges = 0;
    int parity = 0;
    int reduce3x3 = 0;
    int afterBatch = 0;  // final length after BatchSolver

    int totalRaw() const { return centers + edges + parity + reduce3x3; }
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

    std::string toString() const;
};

class BoundHarness {
public:
    // Constructive upper bound U(n)
    static int constructiveUpper(int n);

    // Demaine-shaped target (placeholder constant c=4)
    static double asymptoticTarget(int n);

    // Build report from measured stage lengths
    static BoundReport report(int n, const StageLengths& stages);

    // Convenience: count moves in a sequence
    static int count(const std::vector<Move>& moves) {
        return static_cast<int>(moves.size());
    }
};
