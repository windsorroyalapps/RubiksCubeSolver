# Next session log — 2026-10-08 (flip-aware wing setup)

## Done this session (2026-10-08)
- `EdgePairing::wingOrientation`: +1 home colour order, -1 flipped pair, 0 wrong pair.
- `locateTargetWings` now returns `{edge, orient}` instead of a bare edge index.
- Flipped sources get `flipSliceSetup` (quarter slice on the source second face at that depth) before `depthCommutator`. Undone with the outer setup if `pairedWings` does not rise.
- Fallback budget alternates the same flip-slice on odd wings, also undone on no gain.
- Docs: FACELET_MAPPING, README status, this log.
- Universal constructive algorithm for any n>3 still complete and always terminates.
- Exact integer g(n) for n>=4 remains open (|G(4)|≈7.4e45; U(4)=501 / Ucas=288; community OBTM 35–54).

## Measurement baseline (carry-forward until harness rerun)
Prior: leftoverC=0 leftoverE=6–7 workSolved=no final OBTM 820–919
This commit does not claim a new harness number. Target: leftoverE <=3 then 0 after flip-aware setups are measured.

## Code / doc changes this session
1. `native/reduction/EdgePairing.h` / `.cpp` — orientation + flip-slice setup.
2. `docs/FACELET_MAPPING.md` — flip path.
3. `docs/NEXT.md` + README.

## Try next (priority ordered)
1. Desktop harness re-measure leftoverE / workSolved on >=3 random 4x4 (`RCS_MITM_NODEBUDGET4=150000`). Split remaining misses into wrong-depth vs still-flipped after the quarter-slice.
2. If flip-slice does not drop leftoverE, replace the single quarter with a 2-move setup: outer quarter of the source face, then the slice quarter, both undone on no gain.
3. Shortest outer+slice setup into UF buffer from the recorded `(face,row,col)`, one 8-move commutator, exact undo. Protect solid bitset.
4. After leftoverE=0 + workSolved=true: raise MITM, collect OBTM vs Ucas=288 / community 54.
5. Offline static edge-commutator tables (12 edges x depths x flip) for n=4/5.
6. Free-slice pairing with explicit wing tracking (source facelet + dest facelet + orient).
7. Surface leftoverE/workSolved in Android UI only after clean solves.
8. 3x3 dense pruning DBs toward proven HTM 20.
9. Never invent closed integer g(4). Window 35–54 OBTM.
10. CI: verify lib*.so in APK; adaptive icons.

## Approaches still queued
- Residual-key packing hand-off once edges solid.
- Higher BFS / MITM budgets via env for n>=5.
- Production signed APK + lib verification.
- Per-cell targeted center commutators if leftoverC regresses.
- Diameter search is not feasible: do not schedule an exhaustive 4x4 Cayley BFS.

*Session goal: flipped colour pairs are setup-repaired instead of treated as ordinary matches. Exact diameter remains open. Constructive algorithm for any n>3 always terminates.*
