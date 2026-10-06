# Next session log — 2026-10-07 (Teegan automation — source/dest facelet map)

## Done this session (2026-10-07)
- Landed `WingFacelet` + `EdgePairing::wingFacelets` so both stickers of a wing are `(face,row,col)`, matching the colour scan.
- `pairOne` now uses `locateTargetWings` for real: source edge picks the outer setup face, then `depthCommutator` at that depth, undo if `pairedWings` does not rise.
- Removed the trailing no-gain `U` that could unpair already-solid wings.
- Docs: `docs/FACELET_MAPPING.md`, README status, universal algorithm note.
- Universal constructive algorithm for any n>3 still complete and always terminates.
- Exact integer g(n) for n>=4 remains open (|G(4)|≈7.4e45; U(4)=501 / Ucas=288; community OBTM 35–54).

## Measurement baseline (carry-forward until harness rerun)
Prior: leftoverC=0 leftoverE=6–7 workSolved=no final OBTM 820–919
This commit does not claim a new harness number. Target: leftoverE <=3 then 0 after this mapping is measured.

## Code / doc changes this session
1. `native/reduction/EdgePairing.h` / `.cpp` — facelet map + source-driven setup.
2. `docs/FACELET_MAPPING.md` (new).
3. `docs/NEXT.md` + README + `docs/UNIVERSAL_NXN_ALGORITHM.md`.

## Try next (priority ordered)
1. Desktop harness re-measure leftoverE / workSolved on >=3 random 4x4 (`RCS_MITM_NODEBUDGET4=150000`). If leftoverE still >0, log which dest edges miss a source at the unpaired depth (flip vs wrong depth).
2. Treat orientation: a colour-matched wing may be flipped. Add a flip-aware setup (slice quarter before commutator) instead of only the direct colour match.
3. Shortest outer+slice setup into UF buffer from the recorded `(face,row,col)`, one 8-move commutator, exact undo. Protect solid bitset.
4. After leftoverE=0 + workSolved=true: raise MITM, collect OBTM vs Ucas=288 / community 54.
5. Offline static edge-commutator tables (12 edges x depths x flip) for n=4/5.
6. Free-slice pairing with explicit wing tracking (source facelet + dest facelet).
7. Surface leftoverE/workSolved in Android UI only after clean solves.
8. 3x3 dense pruning DBs toward proven HTM 20.
9. Never invent closed integer g(4). Window 35–54 OBTM.
10. CI: verify lib*.so in APK; adaptive icons.

## Approaches still queued
- Residual-key packing hand-off once edges solid.
- Higher BFS / MITM budgets via env for n>=5.
- Production signed APK + lib verification.
- Per-cell targeted center commutators if leftoverC regresses.

*Session goal: facelet coordinates drive the commutator instead of a discarded locator scan. Exact diameter remains open. Constructive algorithm for any n>3 always terminates.*
