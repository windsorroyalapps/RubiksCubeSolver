#include "CenterSolver.h"
#include "ClusterScheduler.h"

#include <algorithm>

std::vector<Move> CenterSolver::commutator(const Move& a, const Move& b) {
    Move aInv{a.face, a.depth, a.turns == 2 ? 2 : -a.turns};
    Move bInv{b.face, b.depth, b.turns == 2 ? 2 : -b.turns};
    return {a, b, aInv, bInv};
}

static int centerScore(const Cube& work, int face) {
    int n = work.size();
    Color target = static_cast<Color>(face);
    int correct = 0, total = 0;
    for (int r = 1; r < n - 1; ++r) {
        for (int c = 1; c < n - 1; ++c) {
            ++total;
            if (work.get(face, r, c) == target) ++correct;
        }
    }
    return total == 0 ? 100 : (correct * 100) / total;
}

static int opposite(int face) {
    switch (face) {
        case U: return D; case D: return U;
        case F: return B; case B: return F;
        case L: return R; case R: return L;
        default: return face;
    }
}

static void adjacentFaces(int face, int& a1, int& a2, int& a3, int& a4) {
    switch (face) {
        case U: a1 = F; a2 = R; a3 = B; a4 = L; break;
        case D: a1 = F; a2 = L; a3 = B; a4 = R; break;
        case F: a1 = U; a2 = R; a3 = D; a4 = L; break;
        case B: a1 = U; a2 = L; a3 = D; a4 = R; break;
        case L: a1 = U; a2 = F; a3 = D; a4 = B; break;
        case R: a1 = U; a2 = B; a3 = D; a4 = F; break;
        default: a1 = a2 = a3 = a4 = face; break;
    }
}

std::vector<Move> CenterSolver::solveFace(Cube& work, int face) {
    std::vector<Move> moves;
    int n = work.size();
    if (n < 4) return moves;

    auto append = [&](const std::vector<Move>& seq) {
        for (const auto& m : seq) {
            work.apply(m);
            moves.push_back(m);
        }
    };

    int maxDepth = n / 2;
    int opp = opposite(face);
    int adj[4];
    adjacentFaces(face, adj[0], adj[1], adj[2], adj[3]);

    for (int attempt = 0; attempt < 48 && centerScore(work, face) < 100; ++attempt) {
        int bestGain = -1;
        std::vector<Move> bestSeq;
        int scoreBefore = centerScore(work, face);

        for (int depth = 1; depth < maxDepth; ++depth) {
            {
                Move faceTurn{face, 0, 1};
                Move slice{opp, depth, 1};
                auto seq = commutator(faceTurn, slice);
                for (auto& m : seq) work.apply(m);
                int gain = centerScore(work, face) - scoreBefore;
                for (auto it = seq.rbegin(); it != seq.rend(); ++it)
                    work.apply(Move{it->face, it->depth, it->turns == 2 ? 2 : -it->turns});
                if (gain > bestGain) { bestGain = gain; bestSeq = seq; }
            }
            for (int ai = 0; ai < 4; ++ai) {
                Move faceTurn{face, 0, 1};
                Move slice{adj[ai], depth, 1};
                auto seq = commutator(faceTurn, slice);
                for (auto& m : seq) work.apply(m);
                int gain = centerScore(work, face) - scoreBefore;
                for (auto it = seq.rbegin(); it != seq.rend(); ++it)
                    work.apply(Move{it->face, it->depth, it->turns == 2 ? 2 : -it->turns});
                if (gain > bestGain) { bestGain = gain; bestSeq = seq; }
            }
        }

        if (bestGain > 0 && !bestSeq.empty()) append(bestSeq);
        else append({Move{face, 0, 1}});

        if (centerScore(work, face) >= 100) break;
    }
    return moves;
}

std::vector<Move> CenterSolver::solve(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4) return solution;

    auto append = [&](const std::vector<Move>& seq) {
        solution.insert(solution.end(), seq.begin(), seq.end());
    };

    // ---- Phase A: cluster scheduling (Demaine parallel setup) ----
    // Group unsolved center facelets by shared preferred (face,depth,turns).
    // One commutator per group serves many clusters at once.
    for (int pass = 0; pass < 3; ++pass) {
        auto ordered = ClusterScheduler::schedule(work);
        if (ordered.empty()) break;

        auto groups = ClusterScheduler::batchGroups(ordered);
        for (const auto& group : groups) {
            if (group.empty()) continue;
            const auto& need = group.front();
            Move faceTurn{need.id.face, 0, 1};
            Move slice{need.preferredFace, need.preferredDepth, need.preferredTurns};
            auto seq = commutator(faceTurn, slice);
            for (const auto& m : seq) work.apply(m);
            append(seq);
        }
    }

    // ---- Phase B: score-guided face cleanup ----
    const int order[] = {U, D, F, B, L, R};
    for (int face : order) {
        append(solveFace(work, face));
    }

    return solution;
}
