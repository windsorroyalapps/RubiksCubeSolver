        Cube small = extractVirtual3x3(work);
        if (faceMismatch(small) <= 48) {
            auto sol = Kociemba::solve(small);
            if (sol.empty()) sol = CFOPSolver::solve(small);
            if (!sol.empty()) {
                Cube probe = work;
                probe.apply(sol);
                if (probe.isSolved() || faceMismatch(probe) < faceMismatch(work)) {
                    work.apply(sol);
                    solution = std::move(sol);
                }
            }
        }
    }

    if (work.isSolved()) return solution;

    if (reducedEnough) {
        auto rest = outerLayerIda(work, 10, 200000);
        if (!rest.empty())
            solution.insert(solution.end(), rest.begin(), rest.end());
    }

    if (!work.isSolved()) {
        const int mm0 = faceMismatch(work);
        if (stageReady && mm0 <= 64) {
            auto fin = nearSolvedFinish(work, 14, 400000);
            if (!fin.empty())
                solution.insert(solution.end(), fin.begin(), fin.end());
        } else if (eStage <= 1 && cLeft <= 4 && mm0 <= 56) {
            auto fin = nearSolvedFinish(work, 12, 250000);
            if (!fin.empty())
                solution.insert(solution.end(), fin.begin(), fin.end());
        }
    }
    return solution;
}

std::vector<Move> ReductionSolver::solve(const Cube& cube) {
    g_lastLeftoverCenters = 0;
    g_lastLeftoverWings = 0;
    g_lastWorkSolved = false;
    if (cube.size() < 4) {
        g_lastBoundReport = {};
        return Kociemba::solve(cube);
    }

    Cube work = cube;
    std::vector<Move> solution;
    StageLengths stages;

    auto append = [&](const std::vector<Move>& moves, int* sstmCounter, int* obtmCounter) {
        solution.insert(solution.end(), moves.begin(), moves.end());
        if (sstmCounter) *sstmCounter += BoundHarness::count(moves);
        if (obtmCounter) *obtmCounter += BoundHarness::countObtm(moves);
    };

    append(solveCenters(work), &stages.centers, &stages.centersObtm);
    append(pairEdges(work), &stages.edges, &stages.edgesObtm);

    {
        // Drive centers down while holding StageCap leftoverE.
        auto repaired = repairCentersPreserveEdges(work, 14, false);
        if (!repaired.empty()) {
            append(repaired, &stages.centers, &stages.centersObtm);
        }
        // One floating-pair polish if StageCap is clean but eAll remains.
        if (StageCap::leftoverUnpairedWings(work) == 0 &&
            EdgePairing::leftoverUnpairedWings(work) > 0) {
            Cube pre = work;
            Cube probe = work;
            auto raw = EdgePairing::pairAll(probe);
            auto pair = StageCap::capThenRepair(
                raw, pre, StageCap::budgetEdges(work.size()), false);
            if (!pair.first.empty()) {
                Cube check = pre;
                check.apply(pair.first);
                if (StageCap::leftoverUnpairedWings(check) == 0 &&
                    EdgePairing::leftoverUnpairedWings(check) <
                        EdgePairing::leftoverUnpairedWings(pre)) {
                    work.apply(pair.first);
                    append(pair.first, &stages.edges, &stages.edgesObtm);
                }
            }
            if (absoluteCenterBad(work) > 0) {
                auto more = repairCentersPreserveEdges(work, 12, true);
                if (!more.empty())
                    append(more, &stages.centers, &stages.centersObtm);
            }
        }
        g_lastLeftoverCenters = absoluteCenterBad(work);
