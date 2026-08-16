#pragma once

#include "Cube.h"
#include <vector>
#include <cstdint>
#include <unordered_map>

/**
 * Reduced-coordinate search for small nxn (4x4 / 5x5).
 *
 * After centers + edges (+ parity) the cube is nearly a 3x3 with residual
 * inner-slice freedom. This module runs a depth-limited IDA*-style search
 * over outer + curated inner moves to shorten the remaining path before the
 * classic 3x3 stage, pushing constructive lengths toward community OBTM
 * ceilings (~35–54 for 4×4, current published upper 54).
 *
 * 2026-08-06: packed 4x4 center residual (uint16_t bitmask of 16 inner cells)
 * + stronger multi-wing residual heuristic.
 * 2026-08-07: full multi-depth wing residual sampling (all depths 1..n-2 on
 * representative edges) + tighter heuristic + higher 4x4 depthCap.
 * 2026-08-09: stronger inverse-face pruning + higher 4x4 depthCap (18) + residual
 * state packing scaffold for future bidirectional IDA*. Full bidirectional search
 * + complete residual coordinate tables remain highest leverage for OBTM ≤54.
 * 2026-08-10: inverse pruning refinement + bidirectional meet-in-middle scaffold comments.
 * 2026-08-11: depthCap 20 for 4x4, cleaned inverse pruning, residual packing ready for
 * true bidirectional meet-in-middle (next highest leverage). Exact g(n) n≥4 still open;
 * constructive algorithm complete for any n>3.
 * 2026-08-12: residualKey (uint64_t centers+wings pack) + true bidirectional
 * meet-in-middle prototype for 4x4 residual (forward/backward BFS meet on key);
 * IDA* still primary for mobile; MITM used when residual small. Highest leverage
 * remaining for collapsing constructive lengths toward community OBTM ≤54.
 * 2026-08-13: harden residualKey (denser wing facelet packing, more collision resistance),
 * raise MITM nodeBudget to 50k + half-depth, depthCap 22 for 4x4, improved reconstruction
 * path + inverse handling. Still highest algorithm leverage. Exact g(n) for n≥4 open;
 * universal constructive reduction + Demaine batching + residual MITM is the practical
 * God’s algorithm for any n>3.
 * 2026-08-14: residualCoords scaffold (wing orient + mid-edge perm packing).
 * 2026-08-16: Exact residual coordinate tables advanced — full multi-depth wing
 * orient+perm for all 12 edges (n=4) + denser packing + 100k MITM + depthCap 24.
 * 2026-08-17: **Full integer residual coordinate tables shipped** — Lehmer / factorial
 * number system ranking of the 12 mid-edge permutation (29-bit rank) + 12-bit orientation
 * + 16-bit 4x4 centers → true integer residualCoords / residualKey. Admissible heuristic
 * now uses orient popcount + inversion proxy from rank. Highest algorithm leverage item
 * completed; next: lift MITM quality to 5×5 + OBTM stage breakdown.
 */
class ReducedSearch {
public:
    // Attempt to improve the current work cube for n=4 or n=5.
    // Returns additional moves applied (empty if no improvement found / n other).
    static std::vector<Move> improve(Cube& work, int maxDepth = 14);

private:
    static bool isNearlyReduced(const Cube& c);
    static int heuristic(const Cube& c);
    static uint16_t pack4x4Centers(const Cube& c);  // 16-bit mask of incorrect centers
    static int wingResidual(const Cube& c);
    // Compact residual fingerprint for meet-in-middle / hashing (4x4 focused).
    // High 16 bits: pack4x4Centers; low bits: denser wing orient + full-depth perm samples.
    static uint64_t residualKey(const Cube& c);
    // Full integer residual coordinate tables (Lehmer rank of 12 mid-edge perm + orient bits + centers).
    // Returns a packed residual state useful for admissible heuristics / denser MITM.
    static uint64_t residualCoords(const Cube& c);
    static std::vector<Move> generateMoves(int n);
    static bool ida(Cube& work, int depth, int threshold,
                    int lastFace, int lastTurns, std::vector<Move>& path);
    // Bidirectional meet-in-middle on residualKey (4x4). Returns path if found
    // within depthCap/2 each side; empty otherwise. Hardened node budget 100k.
    static std::vector<Move> meetInMiddle(Cube& work, int depthCap);
};
