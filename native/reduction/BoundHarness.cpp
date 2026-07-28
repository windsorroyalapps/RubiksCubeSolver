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
    // Shape only; constant 4 is a placeholder scale factor
    return 4.0 * (nn * nn) / std::log(nn);
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
        << " U(n)=" << constructiveUpper
        << " asym~" << static_cast<int>(asymptoticTarget)
        << " withinU=" << (withinUpper ? "yes" : "NO")
        << " final/U=" << ratioToUpper
        << " final/asym=" << ratioToAsymptotic;
    return oss.str();
}
