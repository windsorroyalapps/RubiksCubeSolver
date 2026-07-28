#pragma once

#include "ClusterScheduler.h"
#include "Cube.h"
#include <string>
#include <vector>

/**
 * Explicit batch-group machinery.
 *
 * A BatchGroup is a set of clusters that share the same preferred move.
 * Applying one Move (or commutator) serves the whole group — Demaine parallelism.
 */
struct BatchGroup {
    int preferredFace = 0;
    int preferredDepth = 0;
    int preferredTurns = 1;
    std::vector<ClusterNeed> members;

    size_t size() const { return members.size();
    bool empty() const { return members.empty(); }

    // Representative cluster (first member)
    const ClusterNeed& representative() const { return members.front(); }
};

class BatchGroups {
public:
    // Build groups from a scheduled need list
    static std::vector<BatchGroup> fromScheduled(const std::vector<ClusterNeed>& ordered);

    // Build groups directly from a cube state
    static std::vector<BatchGroup> fromCube(const Cube& cube);

    // Shared move for a group
    static Move groupMove(const BatchGroup& g);

    // Commutator that uses the group's preferred slice
    static std::vector<Move> groupCommutator(const BatchGroup& g);

    // Apply one commutator per group to the cube; return all moves
    static std::vector<Move> applyAll(Cube& work, const std::vector<BatchGroup>& groups);

    // Stats for logging / UI
    static int totalClusters(const std::vector<BatchGroup>& groups);
    static int maxGroupSize(const std::vector<BatchGroup>& groups);
    static double avgGroupSize(const std::vector<BatchGroup>& groups);
    static std::string summary(const std::vector<BatchGroup>& groups);
};
