        g_lastLeftoverWings = StageCap::leftoverUnpairedWings(work);
    }

    if (work.size() % 2 == 0) {
        const int cLeft = absoluteCenterBad(work);
        const int eLeft = StageCap::leftoverUnpairedWings(work);
        const int eAll = EdgePairing::leftoverUnpairedWings(work);
        if (cLeft == 0 && eLeft == 0 && eAll == 0) {
            append(ParityHandler::fix(work), &stages.parity, &stages.parityObtm);
        }
    }

    if ((work.size() == 4 || work.size() == 5) &&
        StageCap::leftoverUnpairedWings(work) <= 2) {
        Cube probe = work;
        auto improved = ReducedSearch::improve(probe);
        if (!improved.empty()) {
            const int eBefore = StageCap::leftoverUnpairedWings(work);
            const int cBefore = absoluteCenterBad(work);
            if (StageCap::leftoverUnpairedWings(probe) <= eBefore &&
                (absoluteCenterBad(probe) < cBefore || probe.isSolved())) {
                work.apply(improved);
                append(improved, &stages.reduced, &stages.reducedObtm);
                g_lastLeftoverCenters = absoluteCenterBad(work);
                g_lastLeftoverWings = StageCap::leftoverUnpairedWings(work);
            }
        }
    }

    if (StageCap::leftoverUnpairedWings(work) == 0 && absoluteCenterBad(work) > 0) {
        auto more = repairCentersPreserveEdges(work, 16, true);
        if (!more.empty()) {
            append(more, &stages.centers, &stages.centersObtm);
            g_lastLeftoverCenters = absoluteCenterBad(work);
            g_lastLeftoverWings = StageCap::leftoverUnpairedWings(work);
        }
    }

    append(solveAs3x3(work), &stages.reduce3x3, &stages.reduce3x3Obtm);

    // Last-ditch finish when centers + stage cross are home.
    if (!work.isSolved() && absoluteCenterBad(work) == 0 &&
        StageCap::leftoverUnpairedWings(work) == 0) {
        auto fin = nearSolvedFinish(work, 14, 400000);
        if (!fin.empty()) {
            append(fin, &stages.reduce3x3, &stages.reduce3x3Obtm);
        }
    }

    auto optimized = BatchSolver::optimize(solution);
    if (work.isSolved()) {
        Cube verify = cube;
        verify.apply(optimized);
        if (!verify.isSolved()) {
            optimized = BatchSolver::compress(solution);
            verify = cube;
            verify.apply(optimized);
            if (!verify.isSolved()) optimized = solution;
        }
    }
    solution = std::move(optimized);
    stages.afterBatch = BoundHarness::count(solution);

    g_lastWorkSolved = work.isSolved();
    g_lastLeftoverCenters = absoluteCenterBad(work);
    g_lastLeftoverWings = StageCap::leftoverUnpairedWings(work);
    g_lastBoundReport = BoundHarness::report(cube.size(), stages, solution);
    return solution;
}

std::string ReductionSolver::solveToNotation(const Cube& cube) {
    return Cube::movesToNotation(solve(cube));
}

std::string ReductionSolver::lastBoundReportString() {
    std::string s = g_lastBoundReport.toString();
    s += " leftoverC=";
    s += std::to_string(g_lastLeftoverCenters);
    s += " leftoverE=";
    s += std::to_string(g_lastLeftoverWings);
    s += " workSolved=";
    s += (g_lastWorkSolved ? "yes" : "no");
    return s;
}
