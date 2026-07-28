#include "ClusterScheduler.h"

#include <algorithm>
#include <map>
#include <tuple>

std::vector<ClusterId> ClusterScheduler::unsolvedCenters(const Cube& cube) {
    std::vector<ClusterId> out;
    int n = cube.size();
    if (n < 4) return out;

    for (int face = 0; face < 6; ++face) {
        Color target = static_cast<Color>(face);
        for (int r = 1; r < n - 1; ++r) {
            for (int c = 1; c < n - 1; ++c) {
                if (n % 2 == 1 && r == n / 2 && c == n / 2) continue; // fixed center
                if (cube.get(face, r, c) != target) {
                    out.push_back(ClusterId{face, r, c});
                }
            }
        }
    }
    return out;
}

static int oppositeFace(int f) {
    switch (f) {
        case U: return D; case D: return U;
        case F: return B; case B: return F;
        case L: return R; case R: return L;
        default: return f;
    }
}

ClusterNeed ClusterScheduler::needFor(const Cube& cube, const ClusterId& id) {
    ClusterNeed need;
    need.id = id;
    need.priority = 1;

    int n = cube.size();
    // Prefer a slice depth derived from how far the cell is from the rim
    int depth = std::min({id.row, id.col, n - 1 - id.row, n - 1 - id.col});
    if (depth < 1) depth = 1;
    if (depth >= n / 2) depth = std::max(1, n / 2 - 1);

    // Preferred move: turn opposite face's inner slice (classic center gather)
    need.preferredFace = oppositeFace(id.face);
    need.preferredDepth = depth;
    need.preferredTurns = 1;

    // Priority: faces U/D first (stable axis), then belt
    static const int facePri[] = {3, 3, 2, 2, 1, 1}; // U D F B L R-ish
    need.priority = facePri[id.face % 6] * 10 + (n - depth);

    return need;
}

std::vector<ClusterNeed> ClusterScheduler::schedule(const Cube& cube) {
    auto ids = unsolvedCenters(cube);
    std::vector<ClusterNeed> needs;
    needs.reserve(ids.size());
    for (const auto& id : ids) {
        needs.push_back(needFor(cube, id));
    }

    // Primary key: preferred (face, depth, turns) so shared moves sit together
    // Secondary: priority descending
    std::sort(needs.begin(), needs.end(), [](const ClusterNeed& a, const ClusterNeed& b) {
        auto ka = std::make_tuple(a.preferredFace, a.preferredDepth, a.preferredTurns, -a.priority);
        auto kb = std::make_tuple(b.preferredFace, b.preferredDepth, b.preferredTurns, -b.priority);
        return ka < kb;
    });

    return needs;
}

std::vector<std::vector<ClusterNeed>> ClusterScheduler::batchGroups(
    const std::vector<ClusterNeed>& ordered) {

    std::vector<std::vector<ClusterNeed>> groups;
    if (ordered.empty()) return groups;

    groups.push_back({ordered[0]});
    for (size_t i = 1; i < ordered.size(); ++i) {
        const auto& prev = ordered[i - 1];
        const auto& cur = ordered[i];
        bool same = prev.preferredFace == cur.preferredFace
                 && prev.preferredDepth == cur.preferredDepth
                 && prev.preferredTurns == cur.preferredTurns;
        if (same) {
            groups.back().push_back(cur);
        } else {
            groups.push_back({cur});
        }
    }
    return groups;
}
