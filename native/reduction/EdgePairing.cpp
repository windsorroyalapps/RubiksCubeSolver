#include "EdgePairing.h"

#include <algorithm>
#include <cstdint>
#include <queue>
#include <set>
#include <utility>

static constexpr int kStageEdges[4] = {0, 2, 4, 6}; // UF UB DF DB

static void edgeFaces(int edgeIndex, int& f1, int& f2) {
    static const int map[12][2] = {
        {U, F}, {U, R}, {U, B}, {U, L},
        {D, F}, {D, R}, {D, B}, {D, L},
        {F, R}, {F, L}, {B, R}, {B, L}
    };
    f1 = map[edgeIndex][0];
    f2 = map[edgeIndex][1];
}

static Move inverse(const Move& m) {
    return Move{m.face, m.depth, m.turns == 2 ? 2 : -m.turns};
}

static bool sameAxisOpposite(int f1, int f2) {
    return (f1 ^ 1) == f2;
}

static bool wingColors(const Cube& work, int edgeIndex, int slot, Color& a, Color& b) {
    const int n = work.size();
    if (slot < 1 || slot > n - 2) return false;
    int f1, f2;
    edgeFaces(edgeIndex, f1, f2);
    const int o = slot;

    if ((f1 == U && f2 == F) || (f1 == F && f2 == U)) {
        a = work.get(U, n - 1, o); b = work.get(F, 0, o);
    } else if ((f1 == U && f2 == R) || (f1 == R && f2 == U)) {
        a = work.get(U, o, n - 1); b = work.get(R, 0, o);
    } else if ((f1 == U && f2 == B) || (f1 == B && f2 == U)) {
        a = work.get(U, 0, o); b = work.get(B, 0, o);
    } else if ((f1 == U && f2 == L) || (f1 == L && f2 == U)) {
        a = work.get(U, o, 0); b = work.get(L, 0, o);
    } else if ((f1 == D && f2 == F) || (f1 == F && f2 == D)) {
        a = work.get(D, 0, o); b = work.get(F, n - 1, o);
    } else if ((f1 == D && f2 == R) || (f1 == R && f2 == D)) {
        a = work.get(D, o, n - 1); b = work.get(R, n - 1, o);
    } else if ((f1 == D && f2 == B) || (f1 == B && f2 == D)) {
        a = work.get(D, n - 1, o); b = work.get(B, n - 1, o);
    } else if ((f1 == D && f2 == L) || (f1 == L && f2 == D)) {
        a = work.get(D, o, 0); b = work.get(L, n - 1, o);
    } else if ((f1 == F && f2 == R) || (f1 == R && f2 == F)) {
        a = work.get(F, o, n - 1); b = work.get(R, o, 0);
    } else if ((f1 == F && f2 == L) || (f1 == L && f2 == F)) {
        a = work.get(F, o, 0); b = work.get(L, o, n - 1);
    } else if ((f1 == B && f2 == R) || (f1 == R && f2 == B)) {
        a = work.get(B, o, 0); b = work.get(R, o, n - 1);
    } else {
        a = work.get(B, o, n - 1); b = work.get(L, o, 0);
    }
    return true;
}

int EdgePairing::pairedWings(const Cube& work, int edgeIndex) {
    const int n = work.size();
    if (n < 4) return 0;
    int f1, f2;
    edgeFaces(edgeIndex, f1, f2);
    const Color c1 = static_cast<Color>(f1);
    const Color c2 = static_cast<Color>(f2);
    int paired = 0;
    for (int slot = 1; slot <= n - 2; ++slot) {
        Color a = Color::U, b = Color::U;
        if (!wingColors(work, edgeIndex, slot, a, b)) continue;
        if (a == c1 && b == c2) ++paired;
    }
    return paired;
}

int EdgePairing::leftoverUnpairedWings(const Cube& work) {
    const int n = work.size();
    if (n < 4) return 0;
    const int need = n - 2;
    int leftover = 0;
    for (int e = 0; e < 12; ++e) {
        const int p = pairedWings(work, e);
        if (p < need) leftover += (need - p);
    }
    return leftover;
}

static int stageCapLeftover(const Cube& work) {
    const int n = work.size();
    if (n < 4) return 0;
    int bad = 0;
    auto strip = [&](int fa, int ra, int fb, int rb) {
        for (int i = 1; i < n - 1; ++i) {
            const Color a = work.get(fa, ra, i);
            const Color b = work.get(fb, rb, i);
            if (a != static_cast<Color>(fa) || b != static_cast<Color>(fb)) ++bad;
        }
    };
    strip(U, n - 1, F, 0);
    strip(U, 0, B, 0);
    strip(D, 0, F, n - 1);
    strip(D, n - 1, B, n - 1);
    return bad;
}

static int absoluteCenterBad(const Cube& work) {
    const int n = work.size();
    int bad = 0;
    for (int f = 0; f < 6; ++f) {
        const Color want = static_cast<Color>(f);
        for (int r = 1; r < n - 1; ++r)
            for (int c = 1; c < n - 1; ++c)
                if (work.get(f, r, c) != want) ++bad;
    }
    return bad;
}

static int totalPairedAll(const Cube& work) {
    int s = 0;
    for (int e = 0; e < 12; ++e) s += EdgePairing::pairedWings(work, e);
    return s;
}

static int stageSolidCount(const Cube& work) {
    int s = 0;
    for (int e : kStageEdges)
        if (EdgePairing::isSolid(work, e)) ++s;
    return s;
}

static bool breaksStageSolid(const Cube& before, const Cube& after) {
    for (int e : kStageEdges) {
        if (EdgePairing::isSolid(before, e) && !EdgePairing::isSolid(after, e))
            return true;
    }
    return false;
}

static void appendCommutator(int aFace, int aDepth, int aTurns,
                             int bFace, int bDepth, int bTurns,
                             std::vector<std::vector<Move>>& out) {
    const Move mA{aFace, aDepth, aTurns};
    const Move mB{bFace, bDepth, bTurns};
    out.push_back({mA, mB, inverse(mA), inverse(mB)});
    out.push_back({mB, mA, inverse(mB), inverse(mA)});
}

static void collectCandidates(int n, std::vector<std::vector<Move>>& out) {
    static const int kFaces[6] = {U, D, F, B, L, R};
    const int maxSlice = std::max(1, n / 2);

    for (int sliceFace : kFaces) {
        for (int depth = 1; depth < maxSlice; ++depth) {
            for (int outerFace : kFaces) {
                if (outerFace == sliceFace || sameAxisOpposite(outerFace, sliceFace)) continue;
                for (int st : {1, -1, 2})
                    for (int ot : {1, -1, 2})
                        appendCommutator(outerFace, 0, ot, sliceFace, depth, st, out);
            }
        }
    }

    for (int aFace : kFaces) {
        for (int bFace : kFaces) {
            if (aFace == bFace || sameAxisOpposite(aFace, bFace)) continue;
            for (int da = 1; da < maxSlice; ++da)
                for (int db = 1; db < maxSlice; ++db)
                    for (int ta : {1, -1, 2})
                        for (int tb : {1, -1, 2})
                            appendCommutator(aFace, da, ta, bFace, db, tb, out);
        }
    }

    for (int sliceFace : kFaces) {
        for (int depth = 1; depth < maxSlice; ++depth) {
            for (int outerFace : kFaces) {
                if (outerFace == sliceFace || sameAxisOpposite(outerFace, sliceFace)) continue;
                for (int ot : {1, -1, 2}) {
                    appendCommutator(outerFace, 0, ot, sliceFace, depth, 2, out);
                    Move mO{outerFace, 0, ot};
                    Move mS{sliceFace, depth, 2};
                    out.push_back({mS, mO, mS, inverse(mO), mS, mO, mS, inverse(mO)});
                }
            }
        }
    }

    // Classic 4x4 pure wing 3-cycle (often center-preserving):
    // r2 B2 U2 l U2 r' U2 r U2 F2 r F2 l' B2 r2
    if (n == 4) {
        out.push_back({
            Move{R, 1, 2}, Move{B, 0, 2}, Move{U, 0, 2}, Move{L, 1, 1},
            Move{U, 0, 2}, Move{R, 1, -1}, Move{U, 0, 2}, Move{R, 1, 1},
            Move{U, 0, 2}, Move{F, 0, 2}, Move{R, 1, 1}, Move{F, 0, 2},
            Move{L, 1, -1}, Move{B, 0, 2}, Move{R, 1, 2}
        });
        // Conjugates by outer faces
        const auto base = out.back();
        for (int xf : kFaces) {
            for (int xt : {1, -1, 2}) {
                Move mX{xf, 0, xt};
                std::vector<Move> seq = {mX};
                seq.insert(seq.end(), base.begin(), base.end());
                seq.push_back(inverse(mX));
                out.push_back(std::move(seq));
            }
        }
    }

    for (int face : kFaces) {
        for (int depth = 1; depth < maxSlice; ++depth) {
            for (int turns : {1, -1, 2}) {
                for (int other : kFaces) {
                    if (other == face || sameAxisOpposite(other, face)) continue;
                    for (int ot : {1, -1, 2}) {
                        Move w0{face, 0, turns};
                        Move w1{face, depth, turns};
                        Move mO{other, 0, ot};
                        out.push_back({w0, w1, mO, inverse(w0), inverse(w1), inverse(mO)});
                        out.push_back({mO, w0, w1, inverse(mO), inverse(w0), inverse(w1)});
                    }
                }
            }
        }
    }

    const size_t baseLimit = std::min(out.size(), size_t{96});
    for (size_t i = 0; i < baseLimit; ++i) {
        if (out[i].size() != 4) continue;
        const auto comm = out[i];
        for (int xf : kFaces) {
            for (int xt : {1, -1, 2}) {
                Move mX{xf, 0, xt};
                std::vector<Move> seq = {mX};
                seq.insert(seq.end(), comm.begin(), comm.end());
                seq.push_back(inverse(mX));
                out.push_back(std::move(seq));
            }
        }
    }

    for (int face : kFaces) {
        for (int depth = 0; depth < maxSlice; ++depth) {
            for (int turns : {1, -1, 2})
                out.push_back({Move{face, depth, turns}});
        }
    }
}

// allowCenterDamage: if true, accept absC increase only when leftoverE strictly drops.
static bool improveStageCap(Cube& work, std::vector<Move>& out,
                            const std::vector<std::vector<Move>>& candidates,
                            bool allowCenterDamage) {
    const int scBefore = stageCapLeftover(work);
    if (scBefore == 0) return false;
    const int solidBefore = stageSolidCount(work);
    const int pairedBefore = totalPairedAll(work);
    const int cBefore = absoluteCenterBad(work);

    bool found = false;
    int bestSc = scBefore;
    int bestC = cBefore + (allowCenterDamage ? 64 : 0);
    int bestSolid = solidBefore;
    int bestPaired = pairedBefore;
    int bestLen = 999;
    std::vector<Move> bestSeq;

    for (const auto& seq : candidates) {
        if (seq.empty()) continue;
        Cube probe = work;
        for (const Move& m : seq) probe.apply(m);
        if (breaksStageSolid(work, probe)) continue;
        const int c = absoluteCenterBad(probe);
        const int sc = stageCapLeftover(probe);
        if (sc > scBefore) continue;
        if (!allowCenterDamage) {
            if (c > cBefore) continue;
        } else {
            // Must strictly improve leftoverE to justify center dirt.
            if (c > cBefore && sc >= scBefore) continue;
            if (c > cBefore + 8) continue; // hard cap on damage per step
        }
        const int sol = stageSolidCount(probe);
        const int paired = totalPairedAll(probe);
        const int len = static_cast<int>(seq.size());
        if (sc == scBefore && c >= cBefore && sol <= solidBefore && paired <= pairedBefore)
            continue;

        const bool better =
            !found ||
            sc < bestSc ||
            (sc == bestSc && c < bestC) ||
            (sc == bestSc && c == bestC && sol > bestSolid) ||
            (sc == bestSc && c == bestC && sol == bestSolid && paired > bestPaired) ||
            (sc == bestSc && c == bestC && sol == bestSolid && paired == bestPaired && len < bestLen);
        if (better) {
            found = true;
            bestSc = sc;
            bestC = c;
            bestSolid = sol;
            bestPaired = paired;
            bestLen = len;
            bestSeq = seq;
        }
    }
    if (!found) return false;
    for (const Move& m : bestSeq) {
        work.apply(m);
        out.push_back(m);
    }
    return true;
}

static bool bfsStageCap(Cube& work, std::vector<Move>& out, int maxDepth, size_t nodeCap,
                        bool allowCenterDamage) {
    const int sc0 = stageCapLeftover(work);
    if (sc0 == 0) return false;
    const int c0 = absoluteCenterBad(work);

    static const int kFaces[6] = {U, D, F, B, L, R};
    const int n = work.size();
    const int maxSlice = std::max(1, n / 2);
    std::vector<Move> gens;
    for (int f : kFaces)
        for (int t : {1, -1, 2})
            gens.push_back(Move{f, 0, t});
    for (int f : kFaces)
        for (int t : {1, -1, 2})
            for (int d = 1; d < maxSlice; ++d)
                gens.push_back(Move{f, d, t});

    struct Node {
        Cube cube;
        std::vector<Move> path;
    };
    std::queue<Node> q;
    q.push({work, {}});

    auto keyOf = [&](const Cube& c) -> uint64_t {
        uint64_t k = 1469598103934665603ULL;
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
    int bestSc = sc0;
    int bestC = c0;
    std::vector<Move> bestPath;
    const int midCap = allowCenterDamage ? (c0 + 6) : (c0 == 0 ? 0 : c0 + 2);

    while (!q.empty() && nodes < nodeCap) {
        Node cur = std::move(q.front());
        q.pop();
        ++nodes;
        const int sc = stageCapLeftover(cur.cube);
        const int c = absoluteCenterBad(cur.cube);
        const bool endOk = allowCenterDamage ? (sc < sc0 || c <= c0) : (c <= c0);
        if (endOk && (sc < bestSc || (sc == bestSc && c < bestC))) {
            bestSc = sc;
            bestC = c;
            bestPath = cur.path;
            if (sc == 0 && c <= c0) break;
        }
        if (static_cast<int>(cur.path.size()) >= maxDepth) continue;
        const int lastF = cur.path.empty() ? -1 : cur.path.back().face;
        const int lastD = cur.path.empty() ? -1 : cur.path.back().depth;
        for (const Move& m : gens) {
            if (m.face == lastF && m.depth == lastD) continue;
            Cube nxt = cur.cube;
            nxt.apply(m);
            if (breaksStageSolid(cur.cube, nxt) && stageCapLeftover(nxt) >= sc) continue;
            const int cn = absoluteCenterBad(nxt);
            if (cn > midCap) continue;
            if (stageCapLeftover(nxt) > sc0 + 3) continue;
            const uint64_t k = keyOf(nxt);
            if (seen.count(k)) continue;
            seen.insert(k);
            Node nn;
            nn.cube = std::move(nxt);
            nn.path = cur.path;
            nn.path.push_back(m);
            q.push(std::move(nn));
        }
    }

    if (bestPath.empty() || bestSc >= sc0) return false;
    Cube verify = work;
    for (const Move& m : bestPath) verify.apply(m);
    if (!allowCenterDamage && absoluteCenterBad(verify) > c0) return false;
    if (allowCenterDamage && absoluteCenterBad(verify) > c0 && stageCapLeftover(verify) >= sc0)
        return false;
    for (const Move& m : bestPath) {
        work.apply(m);
        out.push_back(m);
    }
    return true;
}

static std::vector<Move> solveStageCapEdges(Cube& work, bool allowCenterDamage) {
    std::vector<Move> moves;
    std::vector<std::vector<Move>> candidates;
    candidates.reserve(8000);
    collectCandidates(work.size(), candidates);
    std::stable_sort(candidates.begin(), candidates.end(),
                     [](const std::vector<Move>& a, const std::vector<Move>& b) {
                         return a.size() < b.size();
                     });

    for (int round = 0; round < 200; ++round) {
        if (stageCapLeftover(work) == 0) break;
        if (improveStageCap(work, moves, candidates, allowCenterDamage)) continue;
        const int sc = stageCapLeftover(work);
        const int depth = sc <= 2 ? 6 : 5;
        const size_t cap = sc <= 2 ? 150000 : 80000;
        if (bfsStageCap(work, moves, depth, cap, allowCenterDamage)) continue;
        break;
    }
    return moves;
}

static bool improveEdge(Cube& work, int edgeIndex, std::vector<Move>& out,
                        const std::vector<std::vector<Move>>& candidates,
                        bool protectStage, bool allowCenterDamage) {
    if (EdgePairing::isSolid(work, edgeIndex)) return false;
    const int before = EdgePairing::pairedWings(work, edgeIndex);
    const int scBefore = stageCapLeftover(work);
    const int cBefore = absoluteCenterBad(work);

    bool found = false;
    int bestDelta = 0;
    int bestSc = scBefore;
    int bestC = cBefore + (allowCenterDamage ? 64 : 0);
    int bestLen = 999;
    std::vector<Move> bestSeq;

    for (const auto& seq : candidates) {
        Cube probe = work;
        for (const Move& m : seq) probe.apply(m);
        if (protectStage && breaksStageSolid(work, probe)) continue;
        const int c = absoluteCenterBad(probe);
        const int sc = stageCapLeftover(probe);
        if (sc > scBefore) continue;
        if (!allowCenterDamage) {
            if (c > cBefore) continue;
        } else if (c > cBefore) {
            if (c > cBefore + 8) continue;
            // require progress on this edge
        }
        const int delta = EdgePairing::pairedWings(probe, edgeIndex) - before;
        if (delta <= 0) continue;
        const int len = static_cast<int>(seq.size());
        const bool better =
            !found || delta > bestDelta ||
            (delta == bestDelta && c < bestC) ||
            (delta == bestDelta && c == bestC && sc < bestSc) ||
            (delta == bestDelta && c == bestC && sc == bestSc && len < bestLen);
        if (better) {
            found = true;
            bestDelta = delta;
            bestSc = sc;
            bestC = c;
            bestLen = len;
            bestSeq = seq;
        }
    }
    if (!found) return false;
    for (const Move& m : bestSeq) {
        work.apply(m);
        out.push_back(m);
    }
    return true;
}

static std::vector<Move> pairRemaining(Cube& work, bool allowCenterDamage) {
    std::vector<Move> moves;
    std::vector<std::vector<Move>> candidates;
    collectCandidates(work.size(), candidates);
    std::stable_sort(candidates.begin(), candidates.end(),
                     [](const std::vector<Move>& a, const std::vector<Move>& b) {
                         return a.size() < b.size();
                     });

    static const int order[8] = {1, 3, 5, 7, 9, 10, 11, 8};

    for (int pass = 0; pass < 6; ++pass) {
        bool any = false;
        for (int e : order) {
            for (int attempt = 0; attempt < 32; ++attempt) {
                if (!improveEdge(work, e, moves, candidates, true, allowCenterDamage)) break;
                any = true;
                if (EdgePairing::isSolid(work, e)) break;
            }
        }
        if (!any) break;
        if (EdgePairing::leftoverUnpairedWings(work) == 0) break;
    }

    if (stageCapLeftover(work) > 0) {
        auto fix = solveStageCapEdges(work, allowCenterDamage);
        moves.insert(moves.end(), fix.begin(), fix.end());
    }
    return moves;
}

// Greedy center restore that must not worsen StageCap leftoverE.
static std::vector<Move> restoreCentersHoldEdges(Cube& work) {
    std::vector<Move> out;
    static const int kFaces[6] = {U, D, F, B, L, R};
    for (int round = 0; round < 48; ++round) {
        const int c0 = absoluteCenterBad(work);
        const int e0 = stageCapLeftover(work);
        if (c0 == 0) break;
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
                                std::vector<Move> seq = {mA, mB, inverse(mA), inverse(mB)};
                                Cube probe = work;
                                for (const Move& m : seq) probe.apply(m);
                                const int c = absoluteCenterBad(probe);
                                const int e = stageCapLeftover(probe);
                                if (e <= e0 && c < bestC) {
                                    bestC = c;
                                    best = seq;
                                    found = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!found) break;
        for (const Move& m : best) {
            work.apply(m);
            out.push_back(m);
        }
    }
    return out;
}

std::vector<Move> EdgePairing::pairOne(Cube& work, int edgeIndex,
                                       const std::bitset<12>& /*solid*/) {
    std::vector<Move> moves;
    if (work.size() < 4) return moves;
    std::vector<std::vector<Move>> candidates;
    collectCandidates(work.size(), candidates);
    for (int attempt = 0; attempt < 48; ++attempt) {
        if (!improveEdge(work, edgeIndex, moves, candidates, true, false)) break;
        if (isSolid(work, edgeIndex)) break;
    }
    return moves;
}

std::vector<Move> EdgePairing::pairAll(Cube& work) {
    std::vector<Move> solution;
    if (work.size() < 4) return solution;

    // Phase 1: center-safe pairing.
    auto stage = solveStageCapEdges(work, false);
    solution.insert(solution.end(), stage.begin(), stage.end());
    auto rest = pairRemaining(work, false);
    solution.insert(solution.end(), rest.begin(), rest.end());

    // Phase 2: if leftoverE remains, allow limited center damage to finish edges.
    if (stageCapLeftover(work) > 0) {
        auto stage2 = solveStageCapEdges(work, true);
        solution.insert(solution.end(), stage2.begin(), stage2.end());
        auto rest2 = pairRemaining(work, true);
        solution.insert(solution.end(), rest2.begin(), rest2.end());
        if (stageCapLeftover(work) > 0) {
            auto polish = solveStageCapEdges(work, true);
            solution.insert(solution.end(), polish.begin(), polish.end());
        }
    }

    // Phase 3: restore absolute centers without worsening leftoverE.
    if (absoluteCenterBad(work) > 0) {
        auto restore = restoreCentersHoldEdges(work);
        solution.insert(solution.end(), restore.begin(), restore.end());
    }
    return solution;
}
