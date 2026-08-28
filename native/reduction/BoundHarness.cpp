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

int BoundHarness::generatorCount(int n) {
    if (n < 2) return 0;
    const int depths = std::max(1, n / 2);
    return 6 * depths * 3;  // faces × slice depths × {90,180,270}
}

int BoundHarness::countingLowerBound(int n) {
    if (n < 2) return 0;
    if (n == 2) return 11;
    if (n == 3) return 20;

    const int gens = generatorCount(n);
    // Conservative |G| >= 2^{1.5 n^2} from independent-ish cubie coloring.
    // Diameter >= log(|G|) / log(|S|).
    const double logG = 1.5 * static_cast<double>(n) * static_cast<double>(n) * std::log(2.0);
    const double lb = logG / std::log(static_cast<double>(std::max(2, gens)));
    int fromCount = static_cast<int>(std::floor(lb));

    // Lift to published community lowers where they exist.
    if (n == 4) return std::max(fromCount, 35);  // OBTM lower ~35
    if (n == 5) return std::max(fromCount, 40);
    return std::max(fromCount, 1);
}

int BoundHarness::communityObtmUpper(int n) {
    if (n == 3) return 20;
    if (n == 4) return 54;   // cubezzz / speedsolving 2015-era tightened bound
    if (n == 5) return 130;  // claimed OBTM-style upper, not a diameter proof
    return 0;
}

double BoundHarness::asymptoticTarget(int n) {
    if (n < 3) return static_cast<double>(constructiveUpper(n));
    double nn = static_cast<double>(n);
    return 3.8 * (nn * nn) / std::log(nn);
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
    r.countingLower = countingLowerBound(n);
    r.communityObtmUpper = communityObtmUpper(n);
    r.generators = generatorCount(n);
    r.asymptoticTarget = asymptoticTarget(n);

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
        << " U(n)=" << constructiveUpper
        << " gens=" << generators
        << " asym~" << static_cast<int>(asymptoticTarget)
        << " withinU=" << (withinUpper ? "yes" : "NO")
        << " final/L=" << ratioToLower
        << " final/U=" << ratioToUpper
        << " final/asym=" << ratioToAsymptotic;
    if (communityObtmUpper > 0 && obtm > 0) {
        oss << " vsOBTM" << communityObtmUpper << "="
            << (obtm <= communityObtmUpper ? "under" : "over");
    }
    return oss.str();
}
