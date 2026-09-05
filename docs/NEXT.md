# Next session log — 2026-09-06

## Done this session
- Added `native/reduction/StageCap.{h,cpp}`: 8-move commutator family + `capToBudget`.
- ReductionSolver clips CenterSolver / EdgePairing output to Ucas C / E (`8(n-2)²` / `96(n-2)`).
- CMake links StageCap.cpp.
- Contract: [STAGE_CAPS.md](STAGE_CAPS.md).
- README / this log updated. Still **no exact integer g(n) for n≥4**.

## Do not claim
Exact integer God's number for any n≥4. |G(4)|≈7.4e45. Published window is still 35–54 OBTM.

## Try next
1. Compile `desktop_harness` **with StageCap.cpp** and log replaySolved + fattest + overC/overE on n=4 trials=10. Cap can make replaySolved drop if the tail was load-bearing — that is the signal to replace the clip with a correct leftover-cell commutator, not to raise the budget silently.
2. When clip fires, emit leftover unsolved center cells / unpaired wings and apply `eightMoveCommutator` per leftover (never-break).
3. If replaySolved is low, debug SiGN encode/decode only (single encoder).
4. Surface Ucas/L/Lfix + overC/overE in the Android UI.
5. 3×3 dense pruning DBs toward proven 20.
6. Verify green CI APK contains lib*.so.
7. Do not invent an integer g(4). Coset/IDA* diameter attack needs a cluster.
8. Keep Ucas honest: if measured solutions exceed Ucas after a *correct* commutator rewrite, raise the constant.
9. Sample-based demigod estimate once harness emits lengths on random 4×4 (avg × 2 is a cap, not a proof).
