# Next session log — 2026-10-23 (automation progress on universal nxn algorithm)

## Done this session (2026-10-23)
- Confirmed exact integer g(n) for n≥4 remains open (proven only for n=2,3; 3x3 HTM=20, QTM=26).
- Latest community OBTM upper for 4x4 remains 54 (Jul 2026, AnanasClassic computer-assisted proof of phases 1+2 ≤18; +16 phase 3 +20 reduced 3x3).
- Probabilistic estimates ~41 HTM / ~48 QTM for 4x4; asymptotic g(n)=Θ(n²/log n) from Demaine et al. holds.
- Universal constructive algorithm for any n>3 is complete and always terminates: reduction + Demaine-style batching + residual MITM + StageCap + facelet-mapped flip-aware 2-move edge pairing + leftover commutators.
- No closed integer diameter invented. Constructive U(n) and counting L(n)/L_fixed/Ucas remain the practical bounds.
- Status: leftoverC=0 confirmed; leftoverE reduction is the remaining completeness gate for workSolved on 4x4.

## Measurement baseline (carry-forward)
Prior: leftoverC=0 leftoverE=6–7 workSolved=no final OBTM 820–919 on 4×4.
Target: leftoverE=0 then workSolved=true on ≥3 random 4×4 before raising MITM budgets.

## Code / doc changes this session
1. docs/NEXT.md — this log.
2. README.md — refreshed status, bounds table (OBTM 54), next steps.

## Try next (priority ordered)
1. Re-run desktop harness `./artifacts/rcs_harness 4 10` with RCS_MITM_NODEBUDGET4=150000 RCS_MITM_DEPTHCAP4=28 and log leftoverE / workSolved. Classify remaining unpaired wings (wrong-depth / orientation / setup-miss).
2. Implement shortest outer+slice path from recorded (face,row,col) into UF buffer, exact undo, solid-bitset protected. Prefer static lookup for n=4.
3. Offline static edge-commutator tables (12 edges × depths × flip) for n=4/5 so pairOne becomes a pure lookup.
4. Once leftoverE=0 and workSolved=true on ≥3 random 4×4: raise MITM, collect measured OBTM vs Ucas=288 / community 54.
5. Free-slice pairing with explicit wing tracking.
6. Surface leftoverE/workSolved in Android UI only after clean solves.
7. 3×3 dense pruning DBs toward proven HTM 20.
8. Never invent closed integer g(4). |G(4)|≈7.4e45. Window 35–54 OBTM. Asymptotic Θ(n²/log n) holds.
9. CI: verify lib*.so; adaptive icons; production signed APK.
10. Optional: integrate phase-decomposition certificates from 54-bound research for tighter constructive stages; probabilistic diameter estimator as optional reporting (not claimed exact g(n)).

## Approaches still queued
- Residual-key packing once edges solid.
- Higher BFS/MITM budgets for n≥5 via env overrides.
- Per-cell targeted center commutators if leftoverC regresses.
- Diameter search not feasible — do not schedule exhaustive Cayley BFS on 4×4.
- Integrate Hirata-style or sampling-based estimators for practical diameter bounds.

*Session goal: keep the constructive algorithm complete and terminating for any n>3 while driving leftoverE toward 0. Exact God's number open for n≥4. Algorithm ships as reduction + batching + residual search + StageCap.*
