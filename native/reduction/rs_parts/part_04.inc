    if (startBad > 72) return {};

    static const int kFaces[6] = {U, D, F, B, L, R};
    std::vector<Move> out;
    const int eHold = StageCap::leftoverUnpairedWings(work);
    const int cHold = absoluteCenterBad(work);

    for (int round = 0; round < 64; ++round) {
        if (work.isSolved()) return out;
        const int bad0 = faceMismatch(work);
        bool improved = false;
        Move bestM{};
        int bestBad = bad0;
        for (int f : kFaces) {
            for (int d : {0, 1}) {
                for (int turns : {1, 2, -1}) {
                    Move m{f, d, turns};
                    work.apply(m);
                    const int bad = faceMismatch(work);
                    const int e = StageCap::leftoverUnpairedWings(work);
                    const int c = absoluteCenterBad(work);
                    work.apply(invertMove(m));
                    if (e <= eHold && c <= cHold && bad < bestBad) {
                        bestBad = bad;
                        bestM = m;
                        improved = true;
                    }
                }
            }
        }
        if (!improved) break;
        work.apply(bestM);
        out.push_back(bestM);
    }
    if (work.isSolved()) return out;

    std::vector<Move> path, best;
    int nodes = 0;
    std::function<bool(int, int, int, int)> search =
        [&](int depth, int limit, int lastFace, int lastDepth) -> bool {
        ++nodes;
        if (nodes > nodeCap) return false;
        if (work.isSolved()) { best = path; return true; }
        if (StageCap::leftoverUnpairedWings(work) > eHold) return false;
        if (absoluteCenterBad(work) > cHold) return false;
        const int h = (faceMismatch(work) + 7) / 8;
        if (depth + h > limit) return false;
        if (depth >= limit) return false;
        for (int f : kFaces) {
            for (int d : {0, 1}) {
                if (f == lastFace && d == lastDepth) continue;
                for (int turns : {1, 2, -1}) {
                    Move m{f, d, turns};
                    work.apply(m);
                    path.push_back(m);
                    if (search(depth + 1, limit, f, d)) return true;
                    path.pop_back();
                    work.apply(invertMove(m));
                    if (nodes > nodeCap) return false;
                }
            }
        }
        return false;
    };

    const int h0 = (faceMismatch(work) + 7) / 8;
    for (int depth = std::max(1, std::min(h0, maxDepth)); depth <= maxDepth; ++depth) {
        path.clear();
        nodes = 0;
        if (search(0, depth, -1, -1)) {
            out.insert(out.end(), best.begin(), best.end());
            return out;
        }
        if (nodes > nodeCap) break;
    }
    return out;
}

std::vector<Move> ReductionSolver::solveAs3x3(Cube& work) {
    if (work.isSolved()) return {};

    if (work.size() == 3) {
        auto sol = Kociemba::solve(work);
        if (sol.empty()) sol = CFOPSolver::solve(work);
        work.apply(sol);
        return sol;
    }

    std::vector<Move> solution;
    const int cLeft = absoluteCenterBad(work);
    const int eStage = StageCap::leftoverUnpairedWings(work);
    const int eAll = EdgePairing::leftoverUnpairedWings(work);
    // Virtual 3x3 requires ALL 12 dedges paired, not only StageCap's 4 strips.
    const bool reducedEnough = (cLeft == 0 && eAll == 0);
    const bool stageReady = (cLeft == 0 && eStage == 0);

    if (reducedEnough) {
