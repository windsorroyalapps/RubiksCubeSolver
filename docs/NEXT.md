# Next session log — 2026-09-07

## Done this session
- `StageCap::leftoverCenterCells` / `leftoverUnpairedWings` / `leftoverCommutators` / `capThenRepair`.
- ReductionSolver uses capThenRepair instead of silent clip; lastBoundReportString emits leftoverC/leftoverE.
- Contract: [STAGE_CAPS.md](STAGE_CAPS.md).
- Still **no exact integer g(n) for n≥4**.

## Do not claim
Exact integer God's number for any n≥4. |G(4)|≈7.4e45. Published window is still 35–54 OBTM.

## Try next
1. Compile `desktop_harness` **with StageCap.cpp** and log replaySolved + fattest + overC/overE + leftoverC/leftoverE on n=4 trials=10. Compare clip-only vs leftover-repair replaySolved.
2. If leftover commutators scramble already-good orbits, restrict A/B generators to the face that owns the leftover cell (per-cell targeting, not cycling faces).
3. If replaySolved is low, debug SiGN encode/decode only (single encoder).
4. Surface leftoverC/leftoverE + Ucas/L/Lfix + overC/overE in the Android UI.
5. 3×3 dense pruning DBs toward proven 20.
6. Verify green CI APK contains lib*.so.
7. Do not invent an integer g(4). Coset/IDA* diameter attack needs a cluster.
8. Keep Ucas honest: leftover repair can grow measured length past Ucas; if *correct* solutions exceed Ucas, raise the constant.
9. Sample-based demigod estimate once harness emits lengths on random 4×4 (avg × 2 is a cap, not a proof).
10. Center BFS node-budget tuning; Edge `pairedWings` metric into BoundHarness.
