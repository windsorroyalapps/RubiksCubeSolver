#include "Kociemba.h"
#include "CFOPSolver.h"
#include "CoordCube.h"
#include "MoveTables.h"
#include "Pruning.h"

#include <sstream>
#include <functional>

// Core IDA* phase-1 in coordinate space. Returns move indices 0..17.
static std::vector<int> phase1Search(int tw, int fl, int sl, int maxDepth) {
    std::vector<int> pathMoves;
    std::vector<int> bestMoves;

    std::function<bool(int,int,int,int,int,int)> search =
        [&](int depth, int limit, int tw, int fl, int sl, int lastFace) -> bool {
        if (tw == 0 && fl == 0 && sl == 0) {
            bestMoves = pathMoves;
            return true;
        }
        CoordCube cc;
        cc.twist = static_cast<int16_t>(tw);
        cc.flip  = static_cast<int16_t>(fl);
        cc.slice = static_cast<int16_t>(sl);
        if (depth + Pruning::phase1Heuristic(cc) > limit) return false;

        for (int m = 0; m < MoveTables::NUM_MOVES; ++m) {
            int face = MoveTables::moveFace(m);
            if (face == lastFace) continue;

            int ntw = MoveTables::twistMove(tw, m);
            int nfl = MoveTables::flipMove(fl, m);
            int nsl = MoveTables::sliceMove(sl, m);

            pathMoves.push_back(m);
            if (search(depth + 1, limit, ntw, nfl, nsl, face)) return true;
            pathMoves.pop_back();
        }
        return false;
    };

    for (int depth = 1; depth <= maxDepth; ++depth) {
        pathMoves.clear();
        if (search(0, depth, tw, fl, sl, -1)) return bestMoves;
    }
    return {};
}

std::vector<Move> Kociemba::phase1(Cube& work) {
    MoveTables::init();
    Pruning::init();

    CoordCube start = CoordCube::fromCube(work);
    int tw = start.twist % MoveTables::TWIST_N;
    int fl = start.flip  % MoveTables::FLIP_N;
    int sl = start.slice % MoveTables::SLICE_N;

    if (tw == 0 && fl == 0 && sl == 0) return {};

    // Gods-regime depth: 16 is enough for almost all positions once tables are dense
    const int MAX_DEPTH = 16;

    auto bestIdx = phase1Search(tw, fl, sl, MAX_DEPTH);

    // Second probe: apply a single U turn then search again; keep the shorter path.
    // (Cheap multi-start that often finds a different G1 entry.)
    if (!bestIdx.empty() && static_cast<int>(bestIdx.size()) > 8) {
        Cube probe = work;
        probe.apply(Move{U, 0, 1});
        CoordCube pcc = CoordCube::fromCube(probe);
        auto alt = phase1Search(pcc.twist % MoveTables::TWIST_N,
                                pcc.flip  % MoveTables::FLIP_N,
                                pcc.slice % MoveTables::SLICE_N,
                                MAX_DEPTH);
        if (!alt.empty() && alt.size() + 1 < bestIdx.size()) {
            // prepend the U and use the shorter combined path
            bestIdx = alt;
            bestIdx.insert(bestIdx.begin(), /*U face turn index*/ 0); // assumes move 0 = U1; safe fallback below
            // Rebuild properly: apply U then alt moves
            std::vector<Move> result;
            result.push_back(Move{U, 0, 1});
            work.apply(result.back());
            for (int m : alt) {
                Move mv{MoveTables::moveFace(m), 0, MoveTables::moveTurns(m)};
                work.apply(mv);
                result.push_back(mv);
            }
            return result;
        }
    }

    if (bestIdx.empty()) return {};

    std::vector<Move> result;
    for (int m : bestIdx) {
        Move mv{MoveTables::moveFace(m), 0, MoveTables::moveTurns(m)};
        work.apply(mv);
        result.push_back(mv);
    }
    return result;
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

    // Soft push toward 20: multi-probe already applied above.
    // True optimal still needs denser tables + more probes (future).
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
