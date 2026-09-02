#pragma once

#include "Cube.h"
#include <string>
#include <vector>

/**
 * Bound harness for working backward toward God's Number on nxn.
 *
 * Proven diameters:
 *   g(2)=11 HTM, g(3)=20 HTM / 26 QTM.
 *   g(n) for n>=4 is OPEN. This harness never claims a closed diameter.
 *
 * Constructive upper U(n) (reduction-style formula, always sufficient):
 *   odd  n: 92n^2 - 307n + 113
 *   even n: 92n^2 - 307n + 257
 *
 * Counting lower bound L(n):
 *   floor( ln|G(n)| / ln|S| ) with |S| = 6 * floor(n/2) * 3 STM generators.
 *   ln|G(n)| from Chris Hardwick's exact position-count formula (OEIS A075152):
 *     even n: 7! 3^6 (24!)^((n^2-2n)/4) / (4!)^{6((n-2)^2/4)}
 *     odd  n: 8! 3^7 12! 2^10 (24!)^((n^2-2n-3)/4) / (4!)^{6((n^2-4n+3)/4)}
 *   For n=2,3 we pin L to the proven diameters (11 / 20), not the counting floor.
 *   For n=4/5 we still lift to published community lowers when those exceed counting.
 *
 * Face-fixed / orientation-quotient lower L_fixed(n) (2026-08-31):
 *   Odd n already has fixed face-centers, so L_fixed == counting floor.
 *   Even n: ln(|G|/24) / ln|S|  (A054434-style cube-rotation quotient).
 *   This is the metric closer to how g(3)=20 was proved (space-fixed cube).
 *
 * Asymptotic shape: c * n^2 / ln(n)  (Demaine Theta)
 *   c ≈ 3.8 calibrated to community 4×4 (~40-48) / 5×5 (~55-70).
 *
 * Dual metrics:
 *   SSTM  = single-slice / every Move counts 1
 *   OBTM  = outer-block: consecutive depth==0 same-face turns collapse to 1;
 *           every inner slice (depth>0) still counts 1
 *
 * Community 4×4 OBTM published upper is 54 (not U(4)=501).
 *
 * Cascade / piece-budget family U_cas(n) (2026-09-03):
 *   8*(n-2)^2          extra-center cells, 8-move commutator budget
 * + 96*(n-2)           12 edges × (n-2) wings × 8-move pair budget
 * + 20 even-n parity   OLL+PLL wing pair
 * + 20                 3×3 God's number
 * + 6n                 slice-setup overhead
 * This is the budget we drive ReductionSolver toward. Official guarantee
 * remains U(n). U_cas is not a published diameter.
 */
struct StageLengths {
    int centers = 0;
    int edges = 0;
    int parity = 0;
    int reduced = 0;   // ReducedSearch residual IDA* (4×4/5×5)
    int reduce3x3 = 0;
    int afterBatch = 0;  // final length after BatchSolver (SSTM-style)

    int centersObtm = 0;
    int edgesObtm = 0;
    int parityObtm = 0;
    int reducedObtm = 0;
    int reduce3x3Obtm = 0;
    int finalObtm = 0;
    int finalSstm = 0;

    int totalRaw() const { return centers + edges + parity + reduced + reduce3x3; }
    int totalFinal() const { return afterBatch > 0 ? afterBatch : totalRaw(); }
};

struct BoundReport {
    int n = 0;
    StageLengths stages;
    int constructiveUpper = 0;  // U(n)
    int constructiveUpperCascade = 0; // U_cas(n) piece-budget family
    int countingLower = 0;      // L(n)
    int countingLowerFixed = 0; // L_fixed(n) face-fixed / A054434-style
    int communityObtmUpper = 0; // 0 if unpublished
    int generators = 0;         // |S| STM
    int gapUpperMinusLower = 0; // U(n) - L(n), how far the diameter window is
    double asymptoticTarget = 0;
    double lnGroupOrder = 0;    // ln|G(n)| Hardwick
    double lnGroupOrderFixed = 0;
    double log10GroupOrder = 0;
    bool withinUpper = false;
    double ratioToUpper = 0;
    double ratioToAsymptotic = 0;
    double ratioToLower = 0;
    int obtm = 0;
    int sstm = 0;

    std::string toString() const;
};

class BoundHarness {
public:
    static int constructiveUpper(int n);
    static int constructiveUpperCascade(int n);
    static int countingLowerBound(int n);
    static int countingLowerBoundFixed(int n);
    static int generatorCount(int n);
    static int communityObtmUpper(int n);
    static int gapUpperMinusLower(int n);
    static double asymptoticTarget(int n);
    static double lnGroupOrder(int n);
    static double lnGroupOrderFixed(int n);
    static double log10GroupOrder(int n);

    // Returns 0 if Hardwick checkpoints match; else the first failing n.
    static int oeisSanityFailN();

    static BoundReport report(int n, const StageLengths& stages);
    static BoundReport report(int n, const StageLengths& stages,
                              const std::vector<Move>& finalSeq);

    static int count(const std::vector<Move>& moves) {
        return static_cast<int>(moves.size());
    }
    static int countSstm(const std::vector<Move>& moves) {
        return static_cast<int>(moves.size());
    }
    static int countObtm(const std::vector<Move>& moves);
};
