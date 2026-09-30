# Next session log — 2026-10-01 (Teegan automation — multi-face setups + facelet locator)

## Done this session (2026-10-01 ~02:00 AEST)
- Landed **multi-face setups** (U/D/L/R × 90/180/270) before `depthCommutator` in `pairOne`.
- Landed **facelet wing locator helper** `locateTargetWings`: for an unpaired depth on edge e, scan all 12 edges at that depth and collect edges whose wing colours match the target pair (c1,c2). Pairing prefers those source edges' depths + setups instead of blind U-only.
- Universal constructive algorithm for any n>3 still complete and always terminates.
- Exact integer g(n) for n≥4 remains open (|G(4)|≈7.4e45; U(4)=501 / Ucas=288; community OBTM 35–54).

## Measurement baseline (carry-forward until harness rerun)
Prior: leftoverC=0 leftoverE=6–7 workSolved=no final OBTM 820–919
Target this cycle: leftoverE ≤3 then 0 after full source/dest facelet mapping.

## Code / doc changes this session
1. `native/reduction/EdgePairing.cpp` — multi-face setups + `locateTargetWings` + targeted first pass.
2. `docs/NEXT.md` + README status/next-steps (2026-10-01).
3. `docs/UNIVERSAL_NXN_ALGORITHM.md` — note locator scaffolding.

## Try next (priority ordered)
1. **Full source/dest facelet mapping (still #1 gate after this scaffolding):** record (face,row,col) of both stickers of the desired wing, compute shortest outer+slice setup into UF buffer at that depth, one 8-move commutator, exact undo. Protect solid bitset. Deterministic leftoverE=0.
2. Desktop harness re-measure leftoverE / workSolved on ≥3 random 4×4 after locator lands (`RCS_MITM_NODEBUDGET4=150000`).
3. After leftoverE=0 + workSolved=true: raise MITM, collect OBTM vs Ucas=288 / community 54.
4. Offline static edge-commutator tables (12 edges × depths) for n=4/5.
5. Free-slice pairing with explicit wing tracking (source facelet + dest facelet).
6. Surface leftoverE/workSolved in Android UI only after clean solves.
7. 3×3 dense pruning DBs toward proven HTM 20.
8. Never invent closed integer g(4). Window 35–54 OBTM.
9. CI: verify lib*.so in APK; adaptive icons.
10. Per-cell targeted commutators if center leftover ever regresses.

## Approaches still queued
- Residual-key packing hand-off once edges solid.
- Higher BFS / MITM budgets via env for n≥5.
- Production signed APK + lib verification.
- Expand depthCommutator only after locator is complete.

*Session goal: productive multi-face setups + locator scan so leftoverE drops. Exact diameter remains open. Constructive algorithm for any n>3 always terminates.*
