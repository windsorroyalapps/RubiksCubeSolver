#include "Kociemba.h"
#include "CFOPSolver.h"
#include "CoordCube.h"
#include "Pruning.h"

#include <sstream>
#include <functional>

static const int FACES[6] = {U, D, F, B, L, R};
static const int TURNS[3] = {1, 2, -1};

std::vector<Move> Kociemba::phase1(Cube& work) {
    Pruning::init();
    CoordCube start = CoordCube::fromCube(work);
    if (start.isG1()) return {};

    const int MAX_DEPTH = 12;
    std::vector<Move> path;
    std::vector<Move> best;

    std::function<bool(int, int, int)> search = [&](int depth, int maxDepth, int lastFace) -> bool {
        CoordCube cc = CoordCube::fromCube(work);
        if (cc.isG1()) {
            best = path;
            return true;
        }
        if (depth + Pruning::phase1Heuristic(cc) > maxDepth) return false;

        for (int f : FACES) {
            if (f == lastFace) continue;
            for (int t : TURNS) {
                Move m{f, 0, t};
                work.apply(m);
                path.push_back(m);
                if (search(depth + 1, maxDepth, f)) return true;
                path.pop_back();
                Move inv{f, 0, t == 2 ? 2 : -t};
                work.apply(inv);
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

std::vector<Move> Kociemba::phase2(Cube& work) {
    if (work.isSolved()) return {};

    const int MAX_DEPTH = 10;
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
            std::vector<int> turns = (f == U || f == D) ? std::vector<int>{1, 2, -1} : std::vector<int>{2};
            for (int t : turns) {
                Move m{f, 0, t};
                work.apply(m);
                path.push_back(m);
                if (search(depth + 1, maxDepth, f)) return true;
                path.pop_back();
                Move inv{f, 0, t == 2 ? 2 : -t};
                work.apply(inv);
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

    Cube work = cube;
    auto p1 = phase1(work);

    if (p1.empty()) {
        return CFOPSolver::solve(cube);
    }

    Cube work2 = cube;
    work2.apply(p1);
    auto p2 = phase2(work2);

    if (p2.empty() && !work2.isSolved()) {
        auto rest = CFOPSolver::solve(work2);
        p1.insert(p1.end(), rest.begin(), rest.end());
        return p1;
    }

    p1.insert(p1.end(), p2.begin(), p2.end());
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
