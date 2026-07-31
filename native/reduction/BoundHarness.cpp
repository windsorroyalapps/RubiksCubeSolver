#include "BoundHarness.h"

#include <cmath>
#include <sstream>

int BoundHarness::constructiveUpper(int n) {
    if (n < 2) return 0;
    if (n == 2) return 11;  // pocket cube God's number (HTM)
    if (n == 3) return 20;  // proven God's number

    // Community constructive formula (reduction-style guarantee)
    int nn = n * n;
    if (n % 2 == 0)
        return 92 * nn - 307 * n + 257;
    return 92 * nn - 307 * n + 113;
}

double BoundHarness::asymptoticTarget(int n) {
    if (n < 3) return static_cast<double>(constructiveUpper(n));
    double nn = static_cast<double>(n);
    // Demaine Theta(n^2 / log n). Scale ~3.8 fitted so that
    // community 4x4 (~40-48) and 5x5 (~55-70) estimates sit near the curve.
    // (n^2 / ln(n) * 3.8 ≈ 44 for n=4, ≈59 for n=5.)
    return 3.8 * (nn * nn) / std::log(nn);
}

int BoundHarness::countObtm(const std::vector<Move>& moves) {
    if (moves.empty()) return 0;
    int count = 0;
    for (size_t i = 0; i < moves.size(); ++i) {
        // Collapse consecutive outer-face (depth==0) turns on the same face
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
    r.asymptoticTarget = asymptoticTarget(n);

    int finalLen = stages.totalFinal();
    r.withinUpper = (r.constructiveUpper <= 0) || (finalLen <= r.constructiveUpper);
    r.ratioToUpper = (r.constructiveUpper > 0)
        ? static_cast<double>(finalLen) / static_cast<double>(r.constructiveUpper)
        : 0.0;
    r.ratioToAsymptotic = (r.asymptoticTarget > 0.0)
        ? static_cast<double>(finalLen) / r.asymptoticTarget
        : 0.0;
    r.sstm = stages.finalSstm > 0 ? stages.finalSstm : finalLen;
    r.obtm = stages.finalObtm;  // may be 0 if not filled
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
        << " centers=" << stages.centers
        << " edges=" << stages.edges
        << " parity=" << stages.parity
        << " 3x3=" << stages.reduce3x3
        << " raw=" << stages.totalRaw()
        << " final=" << stages.totalFinal()
        << " sstm=" << sstm
        << " obtm=" << obtm
        << " U(n)=" << constructiveUpper
        << " asym~" << static_cast<int>(asymptoticTarget)
        << " withinU=" << (withinUpper ? "yes" : "NO")
        << " final/U=" << ratioToUpper
        << " final/asym=" << ratioToAsymptotic;
    if (n == 4 && obtm > 0) {
        oss << " vs4x4OBTM54=" << (obtm <= 54 ? "under" : "over");
    }
    return oss.str();
}
