# Next session log — 2026-09-05

## Done this session
- Split Ucas(n) into per-stage budgets (`CascadeStageBudget`).
- BoundReport now prints UcasC/UcasE/UcasP, overC/overE, fattest stage.
- JNI + Kotlin: `constructiveUpperCascade`, `countingLower`, `countingLowerFixed`.
- print_bounds prints stage-budget rows for n=4..10.
- desktop_harness header prints L / Lfix / Ucas split.
- New contract: [STAGE_BUDGETS.md](STAGE_BUDGETS.md).

## Do not claim
Exact integer God's number for any n≥4. |G(4)|≈7.4e45. Published window is still 35–54 OBTM.

## Try next
1. Compile `desktop_harness` and log replaySolved + fattest stage on n=4 trials=10.
2. Cap CenterSolver / EdgePairing when overC or overE > 0 (swap to length-capped commutator).
3. If replaySolved is low, debug SiGN encode/decode only (single encoder).
4. Surface Ucas/L/Lfix in the Android UI, not just JNI.
5. 3×3 dense pruning DBs toward proven 20.
6. Do not invent an integer g(4). Coset/IDA* diameter attack needs a cluster.
