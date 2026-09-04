#include "BoundHarness.h"

#include <algorithm>
#include <cmath>
#include <sstream>

int BoundHarness::constructiveUpper(int n) {
    if (n < 2) return 0;
    if (n == 2) return 11;
    if (n == 3) return 20;
    int nn = n * n;
    if (n % 2 == 0)
        return 92 * nn - 307 * n + 257;
    return 92 * nn - 307 * n + 113;
}

int BoundHarness::constructiveUpperCascade(int n) {
    return cascadeStageBudget(n).total;
}

CascadeStageBudget BoundHarness::cascadeStageBudget(int n) {
    CascadeStageBudget b;
    if (n < 2) return b;
    if (n == 2) { b.kernel = 11; b.total = 11; return b; }
    if (n == 3) { b.kernel = 20; b.total = 20; return b; }
    const int extra = n - 2;
    b.centers = 8 * extra * extra;
    b.edges = 8 * 12 * extra;
    b.parity = (n % 2 == 0) ? 20 : 0;
    b.kernel = 20;
    b.setup = 6 * n;
    b.total = b.centers + b.edges + b.parity + b.kernel + b.setup;
    return b;
}

int BoundHarness::generatorCount(int n) {
    if (n < 2) return 0;
    const int depths = std::max(1, n / 2);
    return 6 * depths * 3;  // faces × slice depths × {90,180,270}
}

static double logFactorial(int k) {
    return std::lgamma(static_cast<double>(k) + 1.0);
}

double BoundHarness::lnGroupOrder(int n) {
    if (n <= 1) return 0.0;
    // Chris Hardwick / speedsolving exact |G(n)| (not-s, not-m, not-i) = OEIS A075152.
    if (n % 2 == 0) {
        const double exp24 = (static_cast<double>(n) * n - 2.0 * n) / 4.0;
        const double exp4f = 6.0 * ((n - 2.0) * (n - 2.0) / 4.0);
        return logFactorial(7) + 6.0 * std::log(3.0)
             + exp24 * logFactorial(24)
             - exp4f * std::log(24.0);
    }
    const double exp24 = (static_cast<double>(n) * n - 2.0 * n - 3.0) / 4.0;
    const double exp4f = 6.0 * ((static_cast<double>(n) * n - 4.0 * n + 3.0) / 4.0);
    return logFactorial(8) + 7.0 * std::log(3.0) + logFactorial(12)
         + 10.0 * std::log(2.0)
         + exp24 * logFactorial(24)
         - exp4f * std::log(24.0);
}

double BoundHarness::lnGroupOrderFixed(int n) {
    // Odd n: fixed centres already pin spatial orientation.
    // Even n: quotient by the 24 rotations of the whole cube (A054434 spirit).
    if (n % 2 == 0) return lnGroupOrder(n) - std::log(24.0);
    return lnGroupOrder(n);
}

double BoundHarness::log10GroupOrder(int n) {
    return lnGroupOrder(n) / std::log(10.0);
}

static int countingFloorFromLnG(int n, double lnG) {
    const int gens = BoundHarness::generatorCount(n);
    const double lb = lnG / std::log(static_cast<double>(std::max(2, gens)));
    int fromCount = static_cast<int>(std::floor(lb));
    if (fromCount < 1) fromCount = 1;
    return fromCount;
}

int BoundHarness::countingLowerBound(int n) {
    if (n < 2) return 0;
    if (n == 2) return 11;  // proven g(2)
    if (n == 3) return 20;  // proven g(3) HTM

    int fromCount = countingFloorFromLnG(n, lnGroupOrder(n));

    // Lift to published community lowers where they exceed counting.
    if (n == 4) return std::max(fromCount, 35);  // OBTM lower 35 (Tronto/Sheu)
    if (n == 5) return std::max(fromCount, 52);  // speedsolving wiki OBTM lower
    return fromCount;
}

int BoundHarness::countingLowerBoundFixed(int n) {
    if (n < 2) return 0;
    if (n == 2) return 11;
    if (n == 3) return 20;
    int fromCount = countingFloorFromLnG(n, lnGroupOrderFixed(n));
    if (n == 4) return std::max(fromCount, 32);  // face-fixed literature floor is weaker than OBTM 35
    if (n == 5) return std::max(fromCount, 47);  // Hardwick counting, not OBTM
    return fromCount;
}

int BoundHarness::communityObtmUpper(int n) {
    if (n == 3) return 20;
    if (n == 4) return 54;   // cubezzz / speedsolving 2015-era tightened bound
    if (n == 5) return 130;  // claimed OBTM-style upper, not a diameter proof
    return 0;
}

int BoundHarness::gapUpperMinusLower(int n) {
    return constructiveUpper(n) - countingLowerBound(n);
}

double BoundHarness::asymptoticTarget(int n) {
    if (n < 3) return static_cast<double>(constructiveUpper(n));
    double nn = static_cast<double>(n);
    return 3.8 * (nn * nn) / std::log(nn);
}

int BoundHarness::oeisSanityFailN() {
    // Checkpoints: log10|G| rounded to 2 decimals against published Hardwick/wiki values.
    struct CP { int n; double log10; };
    const CP cps[] = {
        {2, 6.56},   // 3,674,160 → 6.565
        {3, 19.64},  // 4.3252e19
        {4, 45.87},  // 7.4012e45
        {5, 74.45},
    };
    for (const auto& c : cps) {
        const double got = log10GroupOrder(c.n);
        if (std::fabs(got - c.log10) > 0.02) return c.n;
    }
    // U(n) arithmetic lock so n>=6 cannot silently regress again.
    if (constructiveUpper(4) != 501) return 4;
    if (constructiveUpper(5) != 878) return 5;
    if (constructiveUpper(6) != 1727) return 6;
    if (constructiveUpper(7) != 2472) return 7;
    if (constructiveUpper(10) != 6387) return 10;
    if (constructiveUpperCascade(4) != 288) return 4;
    if (constructiveUpperCascade(5) != 410) return 5;
    if (constructiveUpperCascade(10) != 1380) return 10;
    return 0;
}

int BoundHarness::countObtm(const std::vector<Move>& moves) {
    if (moves.empty()) return 0;
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        if (i > 0 && moves[i].depth == 0 && moves[i - 1].depth == 0 &&
            moves[i].face == moves[i - 1].face) {
            continue;
        }
        ++count;
    }
    return count;
}

BoundReport BoundHarness::report(int n, const StageLengths& stages) {
    BoundReport r;
    r.n = n;
    r.stages = stages;
    r.constructiveUpper = constructiveUpper(n);
    r.constructiveUpperCascade = constructiveUpperCascade(n);
    r.countingLower = countingLowerBound(n);
    r.countingLowerFixed = countingLowerBoundFixed(n);
    r.communityObtmUpper = communityObtmUpper(n);
    r.generators = generatorCount(n);
    r.gapUpperMinusLower = gapUpperMinusLower(n);
    r.asymptoticTarget = asymptoticTarget(n);
    r.lnGroupOrder = lnGroupOrder(n);
    r.lnGroupOrderFixed = lnGroupOrderFixed(n);
    r.log10GroupOrder = log10GroupOrder(n);

    int finalLen = stages.totalFinal();
    r.withinUpper = (r.constructiveUpper <= 0) || (finalLen <= r.constructiveUpper);
    r.ratioToUpper = (r.constructiveUpper > 0)
        ? static_cast<double>(finalLen) / static_cast<double>(r.constructiveUpper)
        : 0.0;
    r.ratioToAsymptotic = (r.asymptoticTarget > 0.0)
        ? static_cast<double>(finalLen) / r.asymptoticTarget
        : 0.0;
    r.ratioToLower = (r.countingLower > 0)
        ? static_cast<double>(finalLen) / static_cast<double>(r.countingLower)
        : 0.0;
    r.sstm = stages.finalSstm > 0 ? stages.finalSstm : finalLen;
    r.obtm = stages.finalObtm;

    r.cascadeBudget = constructiveUpperCascade(n) ? cascadeStageBudget(n) : CascadeStageBudget{};
    r.overCenters = std::max(0, stages.centers - r.cascadeBudget.centers);
    r.overEdges = std::max(0, stages.edges - r.cascadeBudget.edges);
    r.overParity = std::max(0, stages.parity - r.cascadeBudget.parity);
    r.overKernel = std::max(0, stages.reduce3x3 - r.cascadeBudget.kernel);
    r.overTotalVsUcas = std::max(0, finalLen - r.constructiveUpperCascade);

    struct Cand { int over; const char* name; };
    const Cand cands[] = {
        {r.overCenters, "centers"},
        {r.overEdges, "edges"},
        {r.overParity, "parity"},
        {r.overKernel, "3x3"},
        {std::max(0, stages.reduced - r.cascadeBudget.setup), "reduced"},
    };
    int best = 0;
    r.fattestStage = "none";
    for (const auto& c : cands) {
        if (c.over > best) { best = c.over; r.fattestStage = c.name; }
    }
    if (best == 0) {
        const Cand raw[] = {
            {stages.centers, "centers"},
            {stages.edges, "edges"},
            {stages.parity, "parity"},
            {stages.reduced, "reduced"},
            {stages.reduce3x3, "3x3"},
        };
        int rawBest = -1;
        for (const auto& c : raw) {
            if (c.over > rawBest) { rawBest = c.over; r.fattestStage = c.name; }
        }
    }
    return r;
}

BoundReport BoundHarness::report(int n, const StageLengths& stages,
                                   const std::vector<Move>& finalSeq) {
    StageLengths s = stages;
    s.finalSstm = countSstm(finalSeq);
    s.finalObtm = countObtm(finalSeq);
    if (s.afterBatch == 0) s.afterBatch = s.finalSstm;
    BoundReport r = report(n, s);
    r.sstm = s.finalSstm;
    r.obtm = s.finalObtm;
    return r;
}

std::string BoundReport::toString() const {
    std::ostringstream oss;
    oss << "n=" << n
        << " centers=" << stages.centers << "(obtm=" << stages.centersObtm << ")"
        << " edges=" << stages.edges << "(obtm=" << stages.edgesObtm << ")"
        << " parity=" << stages.parity << "(obtm=" << stages.parityObtm << ")"
        << " reduced=" << stages.reduced << "(obtm=" << stages.reducedObtm << ")"
        << " 3x3=" << stages.reduce3x3 << "(obtm=" << stages.reduce3x3Obtm << ")"
        << " raw=" << stages.totalRaw()
        << " final=" << stages.totalFinal()
        << " sstm=" << sstm
        << " obtm=" << obtm
        << " L(n)=" << countingLower
        << " Lfix=" << countingLowerFixed
        << " U(n)=" << constructiveUpper
        << " Ucas=" << constructiveUpperCascade
        << " gapU-L=" << gapUpperMinusLower
        << " log10|G|=" << static_cast<int>(log10GroupOrder)
        << " gens=" << generators
        << " asym~" << static_cast<int>(asymptoticTarget)
        << " withinU=" << (withinUpper ? "yes" : "NO")
        << " final/L=" << ratioToLower
        << " final/U=" << ratioToUpper
        << " final/asym=" << ratioToAsymptotic
        << " UcasC=" << cascadeBudget.centers
        << " UcasE=" << cascadeBudget.edges
        << " UcasP=" << cascadeBudget.parity
        << " overC=" << overCenters
        << " overE=" << overEdges
        << " fattest=" << (fattestStage ? fattestStage : "?");
    if (communityObtmUpper > 0 && obtm > 0) {
        oss << " vsOBTM" << communityObtmUpper << "="
            << (obtm <= communityObtmUpper ? "under" : "over");
    }
    return oss.str();
}
