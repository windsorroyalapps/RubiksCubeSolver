// Print L(n)/U(n)/|G| table for n=2..Nmax. No solver link required beyond BoundHarness + Cube.h.
// g++ -O2 -std=c++17 -Inative/common -Inative/reduction \
//   native/tools/print_bounds.cpp native/reduction/BoundHarness.cpp -o artifacts/print_bounds

#include "BoundHarness.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {
    const int nmax = (argc > 1) ? std::atoi(argv[1]) : 20;
    const int fail = BoundHarness::oeisSanityFailN();
    if (fail != 0) {
        std::fprintf(stderr, "OEIS/U(n) sanity FAILED at n=%d (log10|G|=%.4f U=%d)\n",
                     fail,
                     BoundHarness::log10GroupOrder(fail),
                     BoundHarness::constructiveUpper(fail));
        return 1;
    }
    std::printf("# Hardwick |G(n)| counting lower vs U(n) + U_cas + face-fixed L_fixed\n");
    std::printf("# n  gens  log10|G|   L(n)  Lfix   U(n)  Ucas   gap   asym  commOBTM\n");
    for (int n = 2; n <= nmax && n <= 40; ++n) {
        std::printf("%2d  %4d  %9.2f  %5d  %5d  %5d  %5d  %5d  %5d  %5d\n",
                    n,
                    BoundHarness::generatorCount(n),
                    BoundHarness::log10GroupOrder(n),
                    BoundHarness::countingLowerBound(n),
                    BoundHarness::countingLowerBoundFixed(n),
                    BoundHarness::constructiveUpper(n),
                    BoundHarness::constructiveUpperCascade(n),
                    BoundHarness::gapUpperMinusLower(n),
                    static_cast<int>(BoundHarness::asymptoticTarget(n)),
                    BoundHarness::communityObtmUpper(n));
    }
    std::printf("# exact g(n) proven only for n=2 (11) and n=3 (20 HTM). n>=4 open.\n");
    std::printf("# L_fixed = face-fixed counting (even n: |G|/24). gap = U(n)-L(n).\n");
    std::printf("# Ucas = piece-budget cascade family (drive solver toward; not diameter).\n");
    return 0;
}
