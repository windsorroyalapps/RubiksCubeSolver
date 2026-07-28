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

    size_t size() const { return members.size(); }
    bool empty() const { return members.empty(); }

    const ClusterNeed& representative() const { return members.front(); }
};

class BatchGroups {
public:
    static std::vector<BatchGroup> fromScheduled(const std::vector<ClusterNeed>& ordered);
    static std::vector<BatchGroup> fromCube(const Cube& cube);

    static Move groupMove(const BatchGroup& g);
    static std::vector<Move> groupCommutator(const BatchGroup& g);
    static std::vector<Move> applyAll(Cube& work, const std::vector<BatchGroup>& groups);

    static int totalClusters(const std::vector<BatchGroup>& groups);
    static int maxGroupSize(const std::vector<BatchGroup>& groups);
    static double avgGroupSize(const std::vector<BatchGroup>& groups);
    static std::string summary(const std::vector<BatchGroup>& groups);
};
