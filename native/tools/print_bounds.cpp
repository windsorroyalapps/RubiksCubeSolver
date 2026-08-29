// Print L(n)/U(n)/|G| table for n=2..Nmax. No solver link required beyond BoundHarness + Cube.h.
// g++ -O2 -std=c++17 -Inative/common -Inative/reduction \
//   native/tools/print_bounds.cpp native/reduction/BoundHarness.cpp -o artifacts/print_bounds

#include "BoundHarness.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {
    const int nmax = (argc > 1) ? std::atoi(argv[1]) : 20;
    std::printf("# Hardwick |G(n)| counting lower vs constructive U(n)\n");
    std::printf("# n  gens  log10|G|   L(n)   U(n)   asym  commOBTM\n");
    for (int n = 2; n <= nmax && n <= 40; ++n) {
        std::printf("%2d  %4d  %9.2f  %5d  %5d  %5d  %5d\n",
                    n,
                    BoundHarness::generatorCount(n),
                    BoundHarness::log10GroupOrder(n),
                    BoundHarness::countingLowerBound(n),
                    BoundHarness::constructiveUpper(n),
                    static_cast<int>(BoundHarness::asymptoticTarget(n)),
                    BoundHarness::communityObtmUpper(n));
    }
    std::printf("# exact g(n) proven only for n=2 (11) and n=3 (20 HTM). n>=4 open.\n");
    return 0;
}
