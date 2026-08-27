// Thin desktop harness: scramble -> reduce-solve -> print BoundHarness + MITM stats.
// RCS_MITM_NODEBUDGET4=150000 RCS_MITM_DEPTHCAP4=28 ./rcs_harness 4 10

#include "Cube.h"
#include "ReductionSolver.h"
#include "BoundHarness.h"
#include "ReducedSearch.h"
#include "GodsAlgorithm.h"

#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

static std::vector<Move> randomScramble(int n, int len, std::mt19937& rng) {
    std::uniform_int_distribution<int> face(0, 5);
    std::uniform_int_distribution<int> depth(0, std::max(0, n / 2 - 1));
    const int turns[3] = {1, 2, -1};
    std::uniform_int_distribution<int> ti(0, 2);
    std::vector<Move> seq;
    seq.reserve(static_cast<size_t>(len));
    int lastFace = -1;
    for (int i = 0; i < len; ++i) {
        int f = face(rng);
        while (f == lastFace) f = face(rng);
        lastFace = f;
        seq.push_back(Move{f, depth(rng), turns[ti(rng)]});
    }
    return seq;
}

int main(int argc, char** argv) {
    const int n = (argc > 1) ? std::atoi(argv[1]) : 4;
    const int trials = (argc > 2) ? std::atoi(argv[2]) : 5;
    const int scrambleLen = (argc > 3) ? std::atoi(argv[3]) : (n <= 3 ? 25 : n * 8);
    if (n < 2 || n > 20 || trials < 1) {
        std::cerr << "usage: rcs_harness [n] [trials] [scrambleLen]\n";
        return 2;
    }

    std::mt19937 rng(20260828);
    long long sumHits = 0, sumNodes = 0;

    std::cout << "n=" << n
              << " U(n)=" << BoundHarness::constructiveUpper(n)
              << " asym~" << static_cast<int>(BoundHarness::asymptoticTarget(n))
              << " mitmBudget=" << ReducedSearch::getNodeBudget(n)
              << " depthCap=" << ReducedSearch::getDepthCap(n)
              << " trials=" << trials << "\n";

    for (int t = 0; t < trials; ++t) {
        Cube cube(n);
        cube.apply(randomScramble(n, scrambleLen, rng));
        std::string notation;
        if (n == 3) notation = GodsAlgorithm::solveToNotation(cube);
        else notation = ReductionSolver::solveToNotation(cube);

        const std::string report = (n >= 4)
            ? ReductionSolver::lastBoundReportString()
            : std::string("3x3 notation");
        const int hits = ReducedSearch::lastMitmHits();
        const int nodes = ReducedSearch::lastNodesExplored();
        sumHits += hits;
        sumNodes += nodes;

        std::cout << "trial=" << (t + 1)
                  << " moves_str_len=" << notation.size()
                  << " mitmHits=" << hits
                  << " nodes=" << nodes
                  << " " << report << "\n";
    }

    std::cout << "avgMitmHits=" << (trials ? (double)sumHits / trials : 0.0)
              << " avgNodes=" << (trials ? (double)sumNodes / trials : 0.0)
              << "\n";
    std::cout << "NOTE: exact g(n) proven only for n=2,3. Harness measures constructive length vs U(n)/OBTM.\n";
    return 0;
}
