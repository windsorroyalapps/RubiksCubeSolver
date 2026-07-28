#include "BatchSolver.h"

#include <cmath>
#include <map>
#include <tuple>

static int normTurns(int t) {
    t %= 4;
    if (t < 0) t += 4;
    if (t == 3) return -1;
    return t; // 0,1,2,-1
}

std::vector<Move> BatchSolver::compress(const std::vector<Move>& raw) {
    std::vector<Move> out;
    for (const auto& m : raw) {
        int turns = normTurns(m.turns);
        if (turns == 0) continue;

        if (!out.empty()
            && out.back().face == m.face
            && out.back().depth == m.depth) {
            int sum = normTurns(out.back().turns + turns);
            if (sum == 0) out.pop_back();
            else out.back().turns = sum;
        } else {
            out.push_back(Move{m.face, m.depth, turns});
        }
    }
    return out;
}

std::vector<Move> BatchSolver::batchWindow(const std::vector<Move>& raw, int windowSize) {
    if (windowSize < 2 || raw.empty()) return raw;

    std::vector<Move> out;
    out.reserve(raw.size());

    for (size_t i = 0; i < raw.size(); ) {
        size_t end = std::min(i + static_cast<size_t>(windowSize), raw.size());

        // Within window: keep first occurrence of each (face, depth, turns)
        // so shared "same move" requests collapse to one (parallelism).
        std::map<std::tuple<int,int,int>, bool> seen;
        for (size_t j = i; j < end; ++j) {
            auto key = std::make_tuple(raw[j].face, raw[j].depth, normTurns(raw[j].turns));
            if (seen[key]) continue;
            seen[key] = true;
            out.push_back(Move{raw[j].face, raw[j].depth, normTurns(raw[j].turns)});
        }
        i = end;
    }
    return out;
}

std::vector<Move> BatchSolver::optimize(const std::vector<Move>& raw) {
    auto s = compress(raw);

    // Window sizes inspired by log-factor batching: try several scales
    // Larger window => more aggressive parallel collapse (more risk of
    // order sensitivity; we re-compress after).
    const int windows[] = {4, 8, 16, 32};
    for (int w : windows) {
        s = batchWindow(s, w);
        s = compress(s);
    }
    return s;
}

int BatchSolver::estimatedClusters(int n) {
    // Visible-ish work units ~ 6 faces * (n-2)^2 centers + 12*(n-2) wings + corners
    if (n < 2) return 0;
    int centers = 6 * (n - 2) * (n - 2);
    int wings = 12 * std::max(0, n - 2);
    int corners = 8;
    return centers + wings + corners; // Theta(n^2)
}

double BatchSolver::asymptoticTarget(int n) {
    if (n < 3) return 20.0;
    double nn = static_cast<double>(n);
    // Uncalibrated shape n^2 / ln(n); constant ~4 is a placeholder scale
    return 4.0 * (nn * nn) / std::log(nn);
}
