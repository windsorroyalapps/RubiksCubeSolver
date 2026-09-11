                    Move sla{adj, 1, 1};
                    macros.push_back(makeComm(ft, sla));
                }
            }
        }

        struct Node {
            Cube cube;
            std::vector<Move> path;
        };
        std::queue<Node> q;
        q.push(Node{work, {}});

        auto keyOf = [&](const Cube& c) -> uint64_t {
            uint64_t k = 1469598103934665603ULL;
            const int n = c.size();
            auto feed = [&](unsigned v) {
                k ^= static_cast<uint64_t>(v) + 0x9e3779b97f4a7c15ULL;
                k *= 1099511628211ULL;
            };
            for (int f = 0; f < 6; ++f)
                for (int r = 1; r < n - 1; ++r)
                    for (int col = 1; col < n - 1; ++col)
                        feed(static_cast<unsigned>(c.get(f, r, col)));
            for (int i = 1; i < n - 1; ++i) {
                feed(static_cast<unsigned>(c.get(U, n - 1, i)));
                feed(static_cast<unsigned>(c.get(F, 0, i)));
                feed(static_cast<unsigned>(c.get(U, 0, i)));
                feed(static_cast<unsigned>(c.get(B, 0, i)));
                feed(static_cast<unsigned>(c.get(D, 0, i)));
                feed(static_cast<unsigned>(c.get(F, n - 1, i)));
                feed(static_cast<unsigned>(c.get(D, n - 1, i)));
                feed(static_cast<unsigned>(c.get(B, n - 1, i)));
            }
            return k;
        };

        std::set<uint64_t> seen;
        seen.insert(keyOf(work));
        size_t nodes = 0;
        int bestC = c0;
        std::vector<Move> bestPath;

        auto consider = [&](Cube nxt, std::vector<Move> path) {
            if (StageCap::leftoverUnpairedWings(nxt) > e0 + 2) return;
            if (preserveFloating && EdgePairing::leftoverUnpairedWings(nxt) > eAll0) return;
            if (absoluteCenterBad(nxt) > c0 + 4) return;
            const uint64_t k = keyOf(nxt);
            if (seen.count(k)) return;
            seen.insert(k);
            Node nn;
            nn.cube = std::move(nxt);
            nn.path = std::move(path);
            q.push(std::move(nn));
        };

        while (!q.empty() && nodes < nodeCap) {
            Node cur = std::move(q.front());
            q.pop();
            ++nodes;
            const int c = absoluteCenterBad(cur.cube);
            const int e = StageCap::leftoverUnpairedWings(cur.cube);
            const int eAll = EdgePairing::leftoverUnpairedWings(cur.cube);
            if (e <= e0 && (!preserveFloating || eAll <= eAll0) && c < bestC) {
                bestC = c;
                bestPath = cur.path;
                if (c == 0) break;
            }
            if (static_cast<int>(cur.path.size()) >= maxDepth) continue;
            const int lf = cur.path.empty() ? -1 : cur.path.back().face;
            const int ld = cur.path.empty() ? -1 : cur.path.back().depth;
            for (const Move& m : gens) {
                if (m.face == lf && m.depth == ld) continue;
                Cube nxt = cur.cube;
                nxt.apply(m);
                auto p = cur.path;
                p.push_back(m);
                consider(std::move(nxt), std::move(p));
            }
            if (static_cast<int>(cur.path.size()) + 4 <= maxDepth) {
                for (const auto& mac : macros) {
                    Cube nxt = cur.cube;
                    auto p = cur.path;
                    for (const Move& m : mac) {
                        nxt.apply(m);
                        p.push_back(m);
