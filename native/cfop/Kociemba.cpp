#include "Kociemba.h"
#include "CFOPSolver.h"
#include "CoordCube.h"
#include "MoveTables.h"
#include "Pruning.h"

#include <sstream>
#include <functional>

std::vector<Move> Kociemba::phase1(Cube& work) {
    MoveTables::init();
    Pruning::init();

    CoordCube start = CoordCube::fromCube(work);
    int tw = start.twist % MoveTables::TWIST_N;
    int fl = start.flip  % MoveTables::FLIP_N;
    int sl = start.slice % MoveTables::SLICE_N;

    if (tw == 0 && fl == 0 && sl == 0) return {};

    // IDA* in pure coordinate space (fast)
    const int MAX_DEPTH = 14; // push toward God's bound regime
    std::vector<int> pathMoves; // move indices 0..17
    std::vector<int> bestMoves;

    std::function<bool(int,int,int,int,int,int)> search =
        [&](int depth, int maxDepth, int tw, int fl, int sl, int lastFace) -> bool {
        if (tw == 0 && fl == 0 && sl == 0) {
            bestMoves = pathMoves;
            return true;
        }
        CoordCube cc;
        cc.twist = static_cast<int16_t>(tw);
        cc.flip  = static_cast<int16_t>(fl);
        cc.slice = static_cast<int16_t>(sl);
        if (depth + Pruning::phase1Heuristic(cc) > maxDepth) return false;

        for (int m = 0; m < MoveTables::NUM_MOVES; ++m) {
            int face = MoveTables::moveFace(m);
            if (face == lastFace) continue;
            // also skip same-axis inverse redundancy lightly: U then D ok

            int ntw = MoveTables::twistMove(tw, m);
            int nfl = MoveTables::flipMove(fl, m);
            int nsl = MoveTables::sliceMove(sl, m);

            pathMoves.push_back(m);
            if (search(depth + 1, maxDepth, ntw, nfl, nsl, face)) return true;
            pathMoves.pop_back();
        }
        return false;
    };

    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        pathMoves.clear();
        if (search(0, depth, tw, fl, sl, -1)) {
            // Convert move indices to Moves and apply to work
            std::vector<Move> result;
            for (int m : bestMoves) {
                Move mv{MoveTables::moveFace(m), 0, MoveTables::moveTurns(m)};
                work.apply(mv);
                result.push_back(mv);
            }
            return result;
        }
    }
    return {};
}

std::vector<Move> Kociemba::phase2(Cube& work) {
    if (work.isSolved()) return {};

    // Phase 2 still facelet-based with restricted generators
    const int MAX_DEPTH = 12;
    std::vector<Move> path;
    std::vector<Move> best;
    const int P2_FACES[6] = {U, D, F, B, L, R};

    std::function<bool(int, int, int)> search = [&](int depth, int maxDepth, int lastFace) -> bool {
        if (work.isSolved()) {
            best = path;
            return true;
        }
        if (depth >= maxDepth) return false;

        for (int f : P2_FACES) {
            if (f == lastFace) continue;
            std::vector<int> turns = (f == U || f == D)
                ? std::vector<int>{1, 2, -1}
                : std::vector<int>{2};
            for (int t : turns) {
                Move m{f, 0, t};
                work.apply(m);
                path.push_back(m);
                if (search(depth + 1, maxDepth, f)) return true;
                path.pop_back();
                work.apply(Move{f, 0, t == 2 ? 2 : -t});
            }
        }
        return false;
    };

    for (int depth = 1; depth <= MAX_DEPTH; ++depth) {
        path.clear();
        if (search(0, depth, -1)) return best;
    }
    return {};
}

std::vector<Move> Kociemba::solve(const Cube& cube) {
    if (cube.size() != 3) return {};

    MoveTables::init();
    Pruning::init();

    Cube work = cube;
    auto p1 = phase1(work);

    if (p1.empty()) {
        // Not in tables / depth limit — CFOP fallback
        return CFOPSolver::solve(cube);
    }

    // work is already at end of phase1
    auto p2 = phase2(work);

    if (p2.empty() && !work.isSolved()) {
        auto rest = CFOPSolver::solve(work);
        p1.insert(p1.end(), rest.begin(), rest.end());
        return p1;
    }

    p1.insert(p1.end(), p2.begin(), p2.end());

    // Soft push toward 20: if still long, caller can accept; true optimal
    // needs multiple phase-1 probes (future).
    return p1;
}

std::string Kociemba::solveToNotation(const Cube& cube) {
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
