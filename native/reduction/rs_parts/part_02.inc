                    }
                    consider(std::move(nxt), std::move(p));
                }
            }
        }

        if (bestPath.empty() || bestC >= c0) return false;
        for (const Move& m : bestPath) {
            work.apply(m);
            out.push_back(m);
        }
        return true;
    };

    auto greedyCommutators = [&]() -> bool {
        const int c0 = absoluteCenterBad(work);
        const int e0 = StageCap::leftoverUnpairedWings(work);
        const int eAll0 = preserveFloating ? EdgePairing::leftoverUnpairedWings(work) : 999;
        if (c0 == 0) return false;
        bool found = false;
        int bestC = c0;
        std::vector<Move> best;
        for (int af : kFaces) {
            for (int bf : kFaces) {
                if (af == bf || (af ^ 1) == bf) continue;
                for (int ad = 0; ad < 2; ++ad) {
                    for (int bd = 0; bd < 2; ++bd) {
                        for (int at : {1, -1, 2}) {
                            for (int bt : {1, -1, 2}) {
                                Move mA{af, ad, at};
                                Move mB{bf, bd, bt};
                                std::vector<Move> seq = makeComm(mA, mB);
                                for (int rep = 0; rep < 2; ++rep) {
                                    Cube probe = work;
                                    for (const Move& m : seq) probe.apply(m);
                                    if (rep == 1) {
                                        for (const Move& m : seq) probe.apply(m);
                                    }
                                    const int c = absoluteCenterBad(probe);
                                    const int e = StageCap::leftoverUnpairedWings(probe);
                                    const int eAll = EdgePairing::leftoverUnpairedWings(probe);
                                    if (e <= e0 && (!preserveFloating || eAll <= eAll0) && c < bestC) {
                                        bestC = c;
                                        best = seq;
                                        if (rep == 1) {
                                            best.insert(best.end(), seq.begin(), seq.end());
                                        }
                                        found = true;
                                    }
                                }
                                for (int xf : kFaces) {
                                    for (int xt : {1, -1, 2}) {
                                        Move mX{xf, 0, xt};
                                        std::vector<Move> conj = {mX};
                                        conj.insert(conj.end(), seq.begin(), seq.end());
                                        conj.push_back(invertMove(mX));
                                        Cube probe = work;
                                        for (const Move& m : conj) probe.apply(m);
                                        const int c = absoluteCenterBad(probe);
                                        const int e = StageCap::leftoverUnpairedWings(probe);
                                        const int eAll = EdgePairing::leftoverUnpairedWings(probe);
                                        if (e <= e0 && (!preserveFloating || eAll <= eAll0) && c < bestC) {
                                            bestC = c;
                                            best = conj;
                                            found = true;
