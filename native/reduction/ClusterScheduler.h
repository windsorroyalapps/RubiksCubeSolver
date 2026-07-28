#pragma once

#include "Cube.h"
#include <vector>
#include <utility>

/**
 * Cluster scheduling for Demaine-style parallelism.
 *
 * Cubie clusters = constant-size groups that never mix under moves.
 * For centers we treat each inner facelet (or small block) as a work unit.
 *
 * Scheduling goal: order unsolved clusters so that consecutive ones
 * share the same preferred (face, depth, turns) move when possible.
 * That lets BatchSolver (and the solver itself) serve many clusters
 * with one slice turn — the log-factor idea from Demaine et al.
 */
struct ClusterId {
    int face;   // 0..5
    int row;
    int col;

    bool operator==(const ClusterId& o) const {
        return face == o.face && row == o.row && col == o.col;
    }
};

struct ClusterNeed {
    ClusterId id;
    int preferredFace;  // move face that helps this cluster
    int preferredDepth;
    int preferredTurns;
    int priority;       // higher = schedule sooner
};

class ClusterScheduler {
public:
    // List unsolved center facelets as clusters
    static std::vector<ClusterId> unsolvedCenters(const Cube& cube);

    // Estimate which move helps a center cluster most (greedy)
    static ClusterNeed needFor(const Cube& cube, const ClusterId& id);

    // Sort needs so identical preferred moves are adjacent (batch-friendly)
    static std::vector<ClusterNeed> schedule(const Cube& cube);

    // Group consecutive needs that share the same preferred move
    static std::vector<std::vector<ClusterNeed>> batchGroups(const std::vector<ClusterNeed>& ordered);
};
