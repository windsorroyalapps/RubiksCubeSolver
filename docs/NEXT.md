# Next session log — 2026-10-16 (progress toward residual edge completeness)

## Done this session (2026-10-16)
- Continued universal constructive algorithm for any n>3.
- Expanded `setupFromSource` in EdgePairing with additional face options (L/R) to improve source positioning into the UF buffer.
- Confirmed leftoverC=0 path solid; leftoverE remains the completeness gate.
- Updated status: constructive U(n) locked, L(n) counting, asymptotic Θ(n²/log n) holds.
- Exact integer g(n) for n≥4 remains open — do not invent closed diameters. |G(4)|≈7.4×10^45 makes exhaustive search impossible.

## Measurement baseline (carry-forward)
Prior: leftoverC=0 leftoverE=6–7 workSolved=no final OBTM 820–919
Target remains leftoverE=0 then workSolved=true on random 4×4 before raising MITM.

## Code / doc changes this session
1. native/reduction/EdgePairing.cpp — expanded setupFromSource faces.
2. native/reduction/EdgePairing.h — comment update for 2026-10-16.
3. docs/NEXT.md — this log.
4. README.md — status date and next-steps section refreshed.

## Try next (priority ordered)
1. Desktop harness re-measure leftoverE / workSolved on ≥3 random 4×4 with RCS_MITM_NODEBUDGET4=150000 after expanded setups. Classify remaining unpaired into depth vs orientation vs setup-miss.
2. Implement shortest outer+slice path from recorded (face,row,col) into UF buffer with exact undo and solid-bitset protection. Prefer facelet coordinates. Static lookup for n=4.
3. Offline static edge-commutator tables (12 edges × depths × flip) for n=4/5 so pairOne becomes a lookup.
4. Once leftoverE=0 and workSolved=true on ≥3 random 4×4: raise MITM budgets, collect measured OBTM vs Ucas=288 / community 54.
5. Free-slice pairing with explicit wing tracking (source + dest facelet + orient).
6. Surface leftoverE/workSolved in Android UI only after clean solves.
7. 3×3 dense pruning DBs toward proven HTM 20.
8. Never invent closed integer g(4). Community window 35–54 OBTM. Demaine et al. asymptotic remains the diameter result.
9. CI: verify lib*.so; adaptive icons; production signed APK.

## Approaches still queued
- Residual-key packing once edges solid.
- Higher BFS/MITM budgets for n≥5 via env.
- Per-cell targeted center commutators if leftoverC regresses.
- Diameter search not feasible — do not schedule exhaustive Cayley BFS on 4×4.

*Session goal: keep the constructive algorithm complete and terminating for any n>3 while driving leftoverE toward 0. Exact God's number open for n≥4.*
