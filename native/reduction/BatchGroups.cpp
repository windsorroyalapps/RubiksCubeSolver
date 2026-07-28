#include "BatchGroups.h"

#include <sstream>

std::vector<BatchGroup> BatchGroups::fromScheduled(const std::vector<ClusterNeed>& ordered) {
    std::vector<BatchGroup> groups;
    if (ordered.empty()) return groups;

    BatchGroup cur;
    cur.preferredFace = ordered[0].preferredFace;
    cur.preferredDepth = ordered[0].preferredDepth;
    cur.preferredTurns = ordered[0].preferredTurns;
    cur.members.push_back(ordered[0]);

    for (size_t i = 1; i < ordered.size(); ++i) {
        const auto& n = ordered[i];
        bool same = n.preferredFace == cur.preferredFace
                 && n.preferredDepth == cur.preferredDepth
                 && n.preferredTurns == cur.preferredTurns;
        if (same) {
            cur.members.push_back(n);
        } else {
            groups.push_back(std::move(cur));
            cur = BatchGroup{};
            cur.preferredFace = n.preferredFace;
            cur.preferredDepth = n.preferredDepth;
            cur.preferredTurns = n.preferredTurns;
            cur.members.push_back(n);
        }
    }
    groups.push_back(std::move(cur));
    return groups;
}

std::vector<BatchGroup> BatchGroups::fromCube(const Cube& cube) {
    return fromScheduled(ClusterScheduler::schedule(cube));
}

Move BatchGroups::groupMove(const BatchGroup& g) {
    return Move{g.preferredFace, g.preferredDepth, g.preferredTurns};
}

std::vector<Move> BatchGroups::groupCommutator(const BatchGroup& g) {
    if (g.empty()) return {};
    const auto& need = g.representative();
    Move a{need.id.face, 0, 1};
    Move b = groupMove(g);
    Move aInv{a.face, a.depth, a.turns == 2 ? 2 : -a.turns};
    Move bInv{b.face, b.depth, b.turns == 2 ? 2 : -b.turns};
    return {a, b, aInv, bInv};
}

std::vector<Move> BatchGroups::applyAll(Cube& work, const std::vector<BatchGroup>& groups) {
    std::vector<Move> all;
    for (const auto& g : groups) {
        if (g.empty()) continue;
        auto seq = groupCommutator(g);
        for (const auto& m : seq) {
            work.apply(m);
            all.push_back(m);
        }
    }
    return all;
}

int BatchGroups::totalClusters(const std::vector<BatchGroup>& groups) {
    int t = 0;
    for (const auto& g : groups) t += static_cast<int>(g.size());
    return t;
}

int BatchGroups::maxGroupSize(const std::vector<BatchGroup>& groups) {
    int m = 0;
    for (const auto& g : groups)
        m = std::max(m, static_cast<int>(g.size()));
    return m;
}

double BatchGroups::avgGroupSize(const std::vector<BatchGroup>& groups) {
    if (groups.empty()) return 0.0;
    return static_cast<double>(totalClusters(groups)) / static_cast<double>(groups.size());
}

std::string BatchGroups::summary(const std::vector<BatchGroup>& groups) {
    std::ostringstream oss;
    oss << "groups=" << groups.size()
        << " clusters=" << totalClusters(groups)
        << " max=" << maxGroupSize(groups)
        << " avg=" << avgGroupSize(groups);
    return oss.str();
}
