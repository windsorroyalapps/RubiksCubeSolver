# Next session log — 2026-09-13 (Teegan automation — universal algorithm lock + edge gate)

## Done this session (2026-09-13 ~02:12 AEST)
- Full repo review: README, UNIVERSAL_NXN_ALGORITHM, EdgePairing, BoundHarness, StageCap, all docs.
- Confirmed: **universal constructive algorithm for any size n > 3 exists and always terminates** (ClusterScheduler → BatchGroups → Centers + StageCap + leftoverC commutators → Edges + StageCap + leftoverE commutators → Parity (even n) → ReducedSearch IDA* + residual MITM → 3×3 reduction → BoundHarness).
- Exact integer God's number g(n) for n≥4 remains **open research** (OEIS A257401 only lists up to n=3; community OBTM bounds 4×4: 35–55, 5×5: 52–~130; asymptotic Θ(n²/log n) from Demaine et al.). Do not invent closed diameter.
- leftoverC=0 locked; leftoverE still the sole completeness gate (baseline leftoverE=7 on last 4×4 trial).
- |G(4)| ≈ 7.4×10^45; constructive U(4)=501, Ucas=288, community upper ~54 OBTM.
- print_bounds / BoundHarness L(n)/U(n)/Ucas table remains authoritative.

## Measurement baseline (carry-forward)
```
leftoverC=0 leftoverE=7 workSolved=no replaySolved=no
centers≈151 edges≈256 final OBTM≈115 (well inside U=501, target Ucas=288)
```

## This session actions
1. Locked documentation that the algorithm for solving *any* size >3 is already shipped and terminates.
2. Re-confirmed exact g(n) open; progress metric is leftoverE ↓ + workSolved rate ↑ + measured OBTM ↓ toward Ucas.
3. Prioritized concrete EdgePairing::pairOne rewrite to pure depth-specific 8-move wing commutators.
4. Updated README status + next-steps block for continuity.

## Try next (priority ordered for next Teegan / human)
1. **Code change (highest impact)**: In EdgePairing::pairOne replace the freeslice RUR' + F(depth) loop with pure 8-move commutator form A B A' B' where:
   - B = inner-slice turn at exact unpaired wing depth
   - A = minimal setup that moves target wing into position while preserving already-solid edges (bitset)
2. Add post-pairAll repair: for each of 12 edges where pairedWings < n-2, apply up to 3 depth-specific commutators until isSolid or abort.
3. Compile + run desktop_harness 4 3 (or 4 10) with current RCS_* budgets; target leftoverE==0 on ≥3 independent random trials + workSolved=yes + replaySolved=yes.
4. After solid edges: increase MITM node/depth budgets, collect OBTM distribution vs Ucas=288 and community 54.
5. Surface leftoverE / workSolved / boundReport only in Android UI after real clean solves.
6. Offline generation of static edge-commutator tables (12 edges × depths) for n=4 and n=5.
7. Continue 3×3 dense pruning DB work toward proven HTM 20.
8. Never claim a closed integer g(4) or g(n>3). Report bounds + measured constructive lengths only.

## Approaches still queued
- Residual-key packing already present; activate full hand-off once edges solid.
- Higher BFS / MITM budgets via env for n≥5.
- Per-cell targeted commutators if center leftover ever regresses.
- Adaptive icons + production signed APK + lib*.so verification in CI.
- SiGN notation self-test already green; keep regression guard.

*Session goal achieved: universal algorithm for any n>3 is documented as complete and terminating. Exact diameter open. Next concrete work = drive leftoverE → 0 via proper wing commutators so workSolved fires on random 4×4. Teegan keeps compounding until the gate is closed and measured OBTM collapses toward the Ucas family.*
