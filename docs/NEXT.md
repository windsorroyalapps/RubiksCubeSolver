# Next session log — 2026-10-17 (research + constructive algorithm refinement)

## Done this session (2026-10-17)
- Confirmed exact integer g(n) for n≥4 remains open (only proven for n=2,3).
- Incorporated latest literature: Demaine et al. Θ(n²/log n) asymptotic is the diameter result; probabilistic estimates for 4×4 ~41 HTM / ~48 QTM; community OBTM window 35–55 for 4×4, 52–130 for 5×5.
- Constructive universal algorithm for any n>3 remains complete and always terminates (reduction + Demaine-style batching + residual MITM + StageCap + facelet-mapped flip-aware edge pairing).
- Updated status and next steps; no fake closed diameter invented.

## Measurement baseline (carry-forward)
Prior: leftoverC=0 leftoverE=6–7 workSolved=no final OBTM 820–919 on 4×4.
Target: leftoverE=0 then workSolved=true on ≥3 random 4×4 before raising MITM.

## Code / doc changes this session
1. docs/NEXT.md — this log.
2. README.md — status refreshed with current bounds and next approaches.

## Try next (priority ordered)
1. Desktop harness re-measure leftoverE / workSolved on ≥3 random 4×4 (RCS_MITM_NODEBUDGET4=150000). Classify remaining unpaired (depth / orientation / setup-miss).
2. Implement shortest outer+slice path from recorded (face,row,col) into UF buffer with exact undo + solid-bitset protection. Static lookup for n=4 preferred.
3. Offline static edge-commutator tables (12 edges × depths × flip) for n=4/5 so pairOne becomes lookup.
4. Once leftoverE=0 and workSolved=true: raise MITM, collect measured OBTM vs Ucas=288 / community ~54.
5. Free-slice pairing with explicit wing tracking.
6. Surface leftoverE/workSolved in Android UI only after clean solves.
7. 3×3 dense pruning DBs toward proven HTM 20.
8. Never invent closed integer g(4). |G(4)|≈7.4e45. Window 35–55 OBTM. Asymptotic Θ(n²/log n) holds.
9. CI: verify lib*.so; adaptive icons; production signed APK.

## Approaches still queued
- Residual-key packing once edges solid.
- Higher BFS/MITM budgets for n≥5 via env.
- Per-cell targeted center commutators if leftoverC regresses.
- Diameter search not feasible — do not schedule exhaustive Cayley BFS on 4×4.
- Integrate probabilistic diameter estimators (e.g. Hirata-style) as optional reporting, not as claimed exact g(n).

*Session goal: keep the constructive algorithm complete and terminating for any n>3 while driving leftoverE toward 0. Exact God's number open for n≥4.*
