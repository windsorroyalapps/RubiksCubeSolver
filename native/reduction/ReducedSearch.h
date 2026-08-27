#pragma once

#include "Cube.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

/**
 * Residual search after centers + edge pairing (+ parity).
 *
 * Tightens the reduced 4x4 / 5x5 state before the 3x3 stage.
 * Uses residual packing, facelet-mismatch heuristic, IDA*, bidirectional MITM.
 *
 * Budgets: JNI setNodeBudget/setDepthCap; env RCS_MITM_NODEBUDGET4/5, RCS_MITM_DEPTHCAP4/5.
 * Exact g(n) for n>=4 remains open; this shrinks constructive length toward it.
 */
class ReducedSearch {
public:
    static std::vector<Move> improve(Cube& work);

    static void setNodeBudget(int n, size_t budget);
    static void setDepthCap(int n, int cap);
    static size_t getNodeBudget(int n);
    static int getDepthCap(int n);

    static uint64_t residualKey(const Cube& cube);
    static int residualHeuristic(const Cube& cube);
    static int lastMitmHits();
    static int lastNodesExplored();

private:
    static void loadEnvOverrides();
};
