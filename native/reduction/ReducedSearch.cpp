#include "ReducedSearch.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <queue>
#include <unordered_map>
#include <utility>

namespace {

size_t g_budget4 = 100000;
size_t g_budget5 = 50000;
int g_cap4 = 24;
int g_cap5 = 18;
int g_mitmHits = 0;
int g_nodes = 0;
bool g_envLoaded = false;

int envInt(const char* name, int fallback) {
    const char* v = std::getenv(name);
    if (!v || !*v) return fallback;
    return std::atoi(v);
}

size_t envSize(const char* name, size_t fallback) {
    const char* v = std::getenv(name);
    if (!v || !*v) return fallback;
    long long x = std::atoll(v);
    if (x <= 0) return fallback;
    return static_cast<size_t>(x);
}

int innerMismatch(const Cube& cube) {
    const int n = cube.size();
    int bad = 0;
    for (int f = 0; f < 6; ++f) {
        const Color target = cube.get(f, n / 2, n / 2);
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) {
                const bool corner = (r == 0 || r == n - 1) && (c == 0 || c == n - 1);
                if (corner) continue;
                if (cube.get(f, r, c) != target) ++bad;
            }
        }
    }
    return bad;
}

uint64_t mix64(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

uint64_t packKey(const Cube& cube) {
    const int n = cube.size();
    uint64_t h = 1469598103934665603ULL;
    auto feed = [&](int v) {
        h ^= static_cast<uint64_t>(v) + 0x9e3779b97f4a7c15ULL;
        h *= 1099511628211ULL;
    };
    for (int f = 0; f < 6; ++f) {
        for (int r = 1; r < n - 1; ++r) {
            for (int c = 1; c < n - 1; ++c) {
                feed(static_cast<int>(cube.get(f, r, c)));
            }
        }
        for (int d = 1; d < n - 1; ++d) {
            feed(static_cast<int>(cube.get(f, 0, d)));
            feed(static_cast<int>(cube.get(f, n - 1, d)));
            feed(static_cast<int>(cube.get(f, d, 0)));
            feed(static_cast<int>(cube.get(f, d, n - 1)));
        }
    }
    return mix64(h);
}

struct GenMove { Move m; };

std::vector<GenMove> generators(int n) {
    std::vector<GenMove> out;
    const int maxDepth = std::max(1, n / 2);
    const int turns[3] = {1, 2, -1};
    for (int f = 0; f < 6; ++f) {
        for (int d = 0; d < maxDepth; ++d) {
            for (int t : turns) out.push_back(GenMove{Move{f, d, t}});
        }
    }
    return out;
}

Move invert(const Move& m) {
    int t = m.turns;
    if (t == 2) return Move{m.face, m.depth, 2};
    return Move{m.face, m.depth, -t};
}

bool sameAxis(const Move& a, const Move& b) {
    return a.face == b.face && a.depth == b.depth;
}

struct Node {
    Cube cube;
    std::vector<Move> path;
};

}  // namespace

void ReducedSearch::loadEnvOverrides() {
    if (g_envLoaded) return;
    g_envLoaded = true;
    g_budget4 = envSize("RCS_MITM_NODEBUDGET4", static_cast<int>(g_budget4));
    g_budget5 = envSize("RCS_MITM_NODEBUDGET5", static_cast<int>(g_budget5));
    g_cap4 = envInt("RCS_MITM_DEPTHCAP4", g_cap4);
    g_cap5 = envInt("RCS_MITM_DEPTHCAP5", g_cap5);
}

void ReducedSearch::setNodeBudget(int n, size_t budget) {
    loadEnvOverrides();
    if (budget == 0) return;
    if (n == 4) g_budget4 = budget;
    else if (n == 5) g_budget5 = budget;
}

void ReducedSearch::setDepthCap(int n, int cap) {
    loadEnvOverrides();
    if (cap <= 0) return;
    if (n == 4) g_cap4 = cap;
    else if (n == 5) g_cap5 = cap;
}

size_t ReducedSearch::getNodeBudget(int n) {
    loadEnvOverrides();
    if (n == 4) return g_budget4;
    if (n == 5) return g_budget5;
    return 0;
}

int ReducedSearch::getDepthCap(int n) {
    loadEnvOverrides();
    if (n == 4) return g_cap4;
    if (n == 5) return g_cap5;
    return 0;
}

uint64_t ReducedSearch::residualKey(const Cube& cube) {
    return packKey(cube);
}

int ReducedSearch::residualHeuristic(const Cube& cube) {
    return (innerMismatch(cube) + 7) / 8;
}

int ReducedSearch::lastMitmHits() { return g_mitmHits; }
int ReducedSearch::lastNodesExplored() { return g_nodes; }

std::vector<Move> ReducedSearch::improve(Cube& work) {
    loadEnvOverrides();
    g_mitmHits = 0;
    g_nodes = 0;

    const int n = work.size();
    if (n != 4 && n != 5) return {};

    const int startScore = innerMismatch(work);
    if (startScore == 0) return {};

    const size_t budget = (n == 4) ? g_budget4 : g_budget5;
    const int depthCap = (n == 4) ? g_cap4 : g_cap5;
    const auto gens = generators(n);

    std::vector<Move> bestPath;
    int bestScore = startScore;

    auto idaSearch = [&](int limit) -> bool {
        std::function<bool(Cube&, int, int, const Move*)> dfs;
        dfs = [&](Cube& c, int g, int bound, const Move* prev) -> bool {
            ++g_nodes;
            if (static_cast<size_t>(g_nodes) > budget) return false;
            const int h = residualHeuristic(c);
            if (g + h > bound) return false;
            const int sc = innerMismatch(c);
            if (sc < bestScore) bestScore = sc;
            if (sc == 0) return true;
            if (g >= bound) return false;
            for (const auto& gm : gens) {
                if (prev && sameAxis(*prev, gm.m)) continue;
                c.apply(gm.m);
                if (dfs(c, g + 1, bound, &gm.m)) {
                    bestPath.insert(bestPath.begin(), gm.m);
                    return true;
                }
                c.apply(invert(gm.m));
                if (static_cast<size_t>(g_nodes) > budget) return false;
            }
            return false;
        };
        Cube clone = work;
        bestPath.clear();
        return dfs(clone, 0, limit, nullptr);
    };

    const int h0 = residualHeuristic(work);
    for (int bound = std::max(1, h0); bound <= std::min(depthCap, 8); ++bound) {
        if (static_cast<size_t>(g_nodes) > budget) break;
        if (idaSearch(bound)) break;
        if (bestScore == 0) break;
    }

    {
        std::unordered_map<uint64_t, std::vector<Move>> fwd;
        std::queue<Node> q;
        q.push(Node{work, {}});
        fwd.emplace(residualKey(work), std::vector<Move>{});
        size_t used = 0;
        const int half = std::max(2, std::min(depthCap / 2, n == 4 ? 6 : 4));
        while (!q.empty() && used < budget / 2) {
            Node cur = std::move(q.front());
            q.pop();
            if (static_cast<int>(cur.path.size()) >= half) continue;
            const Move* prev = cur.path.empty() ? nullptr : &cur.path.back();
            for (const auto& gm : gens) {
                if (prev && sameAxis(*prev, gm.m)) continue;
                Cube nxt = cur.cube;
                nxt.apply(gm.m);
                auto path = cur.path;
                path.push_back(gm.m);
                uint64_t key = residualKey(nxt);
                if (fwd.find(key) != fwd.end()) continue;
                fwd.emplace(key, path);
                ++used;
                ++g_nodes;
                const int sc = innerMismatch(nxt);
                if (sc < bestScore) {
                    bestScore = sc;
                    bestPath = path;
                }
                q.push(Node{std::move(nxt), std::move(path)});
                if (used >= budget / 2) break;
            }
        }

        Cube goal(n);
        std::unordered_map<uint64_t, std::vector<Move>> back;
        std::queue<Node> bq;
        bq.push(Node{goal, {}});
        back.emplace(residualKey(goal), std::vector<Move>{});
        size_t usedB = 0;
        while (!bq.empty() && usedB < budget / 2) {
            Node cur = std::move(bq.front());
            bq.pop();
            auto it = fwd.find(residualKey(cur.cube));
            if (it != fwd.end()) {
                ++g_mitmHits;
                std::vector<Move> meetPath = it->second;
                for (auto rit = cur.path.rbegin(); rit != cur.path.rend(); ++rit) {
                    meetPath.push_back(invert(*rit));
                }
                Cube test = work;
                test.apply(meetPath);
                const int sc = innerMismatch(test);
                if (sc < bestScore) {
                    bestScore = sc;
                    bestPath = meetPath;
                }
                if (bestScore == 0) break;
            }
            if (static_cast<int>(cur.path.size()) >= half) continue;
            const Move* prev = cur.path.empty() ? nullptr : &cur.path.back();
            for (const auto& gm : gens) {
                if (prev && sameAxis(*prev, gm.m)) continue;
                Cube nxt = cur.cube;
                nxt.apply(gm.m);
                auto path = cur.path;
                path.push_back(gm.m);
                uint64_t key = residualKey(nxt);
                if (back.find(key) != back.end()) continue;
                back.emplace(key, path);
                ++usedB;
                ++g_nodes;
                bq.push(Node{std::move(nxt), std::move(path)});
                if (usedB >= budget / 2) break;
            }
        }
    }

    if (bestPath.empty() || bestScore >= startScore) return {};
    work.apply(bestPath);
    return bestPath;
}
