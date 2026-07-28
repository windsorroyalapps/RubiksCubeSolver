#include "GodsAlgorithm.h"
#include "Kociemba.h"
#include "CFOPSolver.h"
#include "CoordCube.h"
#include "MoveTables.h"
#include "Pruning.h"

#include <sstream>
#include <functional>
#include <algorithm>

std::vector<Move> GodsAlgorithm::kociembaOnce(const Cube& cube, int /*phase1MaxDepth*/) {
    // Delegate to existing Kociemba (uses depth 14 phase-1 internally)
    return Kociemba::solve(cube);
}

std::vector<Move> GodsAlgorithm::multiProbeKociemba(const Cube& cube) {
    // Probe 1: standard Kociemba
    auto best = kociembaOnce(cube, 14);
    size_t bestLen = best.size();

    // Probe 2..N: pre-apply a short scramble of axis-alternating moves,
    // solve, then prepend the inverse of the pre-moves.
    // This explores different phase-1 entry paths (classic multi-probe idea).
    static const int preMoves[][2] = {
        {U, 1}, {D, 1}, {F, 1}, {B, 1}, {L, 1}, {R, 1},
        {U, 2}, {R, 2}, {F, 2},
        {U, -1}, {R, -1}, {F, -1},
    };
    const int nPre = sizeof(preMoves) / sizeof(preMoves[0]);

    for (int i = 0; i < nPre; ++i) {
        Cube c = cube;
        Move pre{preMoves[i][0], 0, preMoves[i][1]};
        c.apply(pre);

        auto sol = Kociemba::solve(c);
        if (sol.empty()) continue;

        // Full solution = pre + sol, but we need inverse(pre) at the end
        // Actually: solve(pre*state) = M means M solves pre*state,
        // so inverse(pre) * M solves state? 
        // state --pre--> pre*state --M--> solved
        // so state --pre--> --M--> solved, solution is pre + M
        std::vector<Move> full;
        full.push_back(pre);
        full.insert(full.end(), sol.begin(), sol.end());

        if (full.size() < bestLen || best.empty()) {
            best = full;
            bestLen = full.size();
        }
        if (bestLen <= static_cast<size_t>(GODS_NUMBER)) break; // good enough
    }

    return best;
}

std::vector<Move> GodsAlgorithm::optimalCleanup(const Cube& cube, int maxDepth) {
    // Bounded optimal IDA* in facelet space (HTM, 18 moves).
    // Only used when multi-probe still exceeds 20; depth capped for mobile.
    MoveTables::init();
    Pruning::init();

    if (cube.isSolved()) return {};

    std::vector<Move> path;
    std::vector<Move> best;
    Cube work = cube;

    auto heuristic = [&](const Cube& c) -> int {
        CoordCube cc = CoordCube::fromCube(c);
        return Pruning::phase1Heuristic(cc);
    };

    std::function<bool(int, int, int)> search = [&](int depth, int limit, int lastFace) -> bool {
        if (work.isSolved()) {
            best = path;
            return true;
        }
        if (depth + heuristic(work) > limit) return false;
        if (depth >= limit) return false;

        for (int m = 0; m < MoveTables::NUM_MOVES; ++m) {
            int face = MoveTables::moveFace(m);
            if (face == lastFace) continue;

            Move mv{face, 0, MoveTables::moveTurns(m)};
            work.apply(mv);
            path.push_back(mv);
            if (search(depth + 1, limit, face)) return true;
            path.pop_back();
            work.apply(Move{face, 0, mv.turns == 2 ? 2 : -mv.turns});
        }
        return false;
    };

    int cap = std::min(maxDepth, GODS_NUMBER);
    for (int depth = 1; depth <= cap; ++depth) {
        path.clear();
        work = cube;
        if (search(0, depth, -1)) return best;
    }
    return {};
}

std::vector<Move> GodsAlgorithm::solve(const Cube& cube) {
    if (cube.size() != 3) return {};
    if (cube.isSolved()) return {};

    MoveTables::init();
    Pruning::init();

    // 1) Multi-probe Kociemba
    auto sol = multiProbeKociemba(cube);

    // 2) If empty, CFOP
    if (sol.empty()) {
        sol = CFOPSolver::solve(cube);
    }

    // 3) If still longer than God's Number, try optimal cleanup
    if (!sol.empty() && static_cast<int>(sol.size()) > GODS_NUMBER) {
        auto opt = optimalCleanup(cube, GODS_NUMBER);
        if (!opt.empty() && opt.size() <= sol.size()) {
            sol = opt;
        }
    }

    // 4) If still empty, last-resort optimal with slightly higher cap
    if (sol.empty()) {
        sol = optimalCleanup(cube, GODS_NUMBER);
    }

    return sol;
}

std::string GodsAlgorithm::solveToNotation(const Cube& cube) {
    auto moves = solve(cube);
    static const char* faces = "UDFBLR";
    std::ostringstream oss;
    for (size_t i = 0; i < moves.size(); ++i) {
        const auto& m = moves[i];
        oss << faces[m.face];
        if (m.turns == 2) oss << '2';
        else if (m.turns == -1 || m.turns == 3) oss << '\'';
        if (i + 1 < moves.size()) oss << ' ';
    }
    return oss.str();
}
