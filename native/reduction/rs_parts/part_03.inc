                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!found) return false;
        for (const Move& m : best) {
            work.apply(m);
            out.push_back(m);
        }
        return true;
    };

    for (int r = 0; r < rounds; ++r) {
        if (absoluteCenterBad(work) == 0) break;
        if (greedyCommutators()) continue;
        const int depth = (r < 3) ? std::max(bfsDepthEnv, 7) : std::max(bfsDepthEnv + 2, 9);
        const size_t cap = (r < 3)
            ? static_cast<size_t>(std::max(bfsNodesEnv, 80000))
            : static_cast<size_t>(std::max(bfsNodesEnv * 2, 200000));
        if (!bfsOnce(depth, cap)) break;
    }
    return out;
}

static Cube extractVirtual3x3(const Cube& big) {
    Cube s(3);
    const int n = big.size();
    // Prefer a solid wing slot; fall back to index 1.
    int wing = 1;
    for (int slot = 1; slot <= n - 2; ++slot) {
        bool ok = true;
        for (int e = 0; e < 12 && ok; ++e) {
            if (EdgePairing::pairedWings(big, e) < (n - 2)) { ok = false; break; }
        }
        if (ok) { wing = slot; break; }
    }
    for (int f = 0; f < 6; ++f) {
        s.set(f, 1, 1, static_cast<Color>(f));
        s.set(f, 0, 0, big.get(f, 0, 0));
        s.set(f, 0, 2, big.get(f, 0, n - 1));
        s.set(f, 2, 0, big.get(f, n - 1, 0));
        s.set(f, 2, 2, big.get(f, n - 1, n - 1));
        s.set(f, 0, 1, big.get(f, 0, wing));
        s.set(f, 2, 1, big.get(f, n - 1, wing));
        s.set(f, 1, 0, big.get(f, wing, 0));
        s.set(f, 1, 2, big.get(f, wing, n - 1));
    }
    return s;
}

static int faceMismatch(const Cube& c) {
    int bad = 0;
    const int n = c.size();
    for (int f = 0; f < 6; ++f)
        for (int r = 0; r < n; ++r)
            for (int col = 0; col < n; ++col)
                if (c.get(f, r, col) != static_cast<Color>(f)) ++bad;
    return bad;
}

static std::vector<Move> outerLayerIda(Cube& work, int maxDepth, int nodeCap) {
    if (work.isSolved()) return {};
    std::vector<Move> path, best;
    int nodes = 0;
    std::function<bool(int, int, int)> search = [&](int depth, int limit, int lastFace) -> bool {
        ++nodes;
        if (nodes > nodeCap) return false;
        if (work.isSolved()) { best = path; return true; }
        const int h = (faceMismatch(work) + 7) / 8;
        if (depth + h > limit) return false;
        if (depth >= limit) return false;
        for (int f = 0; f < 6; ++f) {
            if (f == lastFace) continue;
            for (int turns : {1, 2, -1}) {
                Move m{f, 0, turns};
                work.apply(m);
                path.push_back(m);
                if (search(depth + 1, limit, f)) return true;
                path.pop_back();
                work.apply(invertMove(m));
                if (nodes > nodeCap) return false;
            }
        }
        return false;
    };
    const int h0 = (faceMismatch(work) + 7) / 8;
    for (int depth = std::max(1, h0); depth <= maxDepth; ++depth) {
        path.clear();
        nodes = 0;
        if (search(0, depth, -1)) return best;
        if (nodes > nodeCap) break;
    }
    return {};
}

static std::vector<Move> nearSolvedFinish(Cube& work, int maxDepth, int nodeCap) {
    if (work.isSolved()) return {};
    const int startBad = faceMismatch(work);
