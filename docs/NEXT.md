# Next session log — 2026-10-09 (2-move flip setup)

## Done this session (2026-10-09)
- `twoMoveFlipSetup(sourceEdge, depth, variant)`: outer quarter of the source first face, then a slice quarter on the source second face at that depth.
- Used only after the existing single `flipSliceSetup` pass does not raise `pairedWings`. Both moves undone exactly on no gain.
- Fallback budget: wings with `wing % 4 == 3` try the 2-move setup instead of the single slice, also undone on no gain.
- Docs: FACELET_MAPPING, README status, this log.
- Universal constructive algorithm for any n>3 still complete and always terminates.
- Exact integer g(n) for n>=4 remains open (|G(4)|≈7.4e45; U(4)=501 / Ucas=288; community OBTM window 35–54). Not claimed closed.

## Measurement baseline (carry-forward until harness rerun)
Prior: leftoverC=0 leftoverE=6–7 workSolved=no final OBTM 820–919
This commit does not claim a new harness number. Target: leftoverE <=3 then 0 after the 2-move setup is measured.

## Code / doc changes this session
1. `native/reduction/EdgePairing.h` / `.cpp` — 2-move flip setup after single quarter fails.
2. `docs/FACELET_MAPPING.md` — two-move path.
3. `docs/NEXT.md` + README.

## Try next (priority ordered)
1. Desktop harness re-measure leftoverE / workSolved on >=3 random 4x4 (`RCS_MITM_NODEBUDGET4=150000`). Split remaining misses into wrong-depth vs still-flipped after single-slice vs after 2-move setup.
2. Shortest outer+slice path from recorded `(face,row,col)` into the UF buffer, one commutator, exact undo. Protect solid bitset. Prefer facelet coordinates over the edge-index heuristic in `setupFromSource`.
3. Offline static edge-commutator tables (12 edges x depths x flip) for n=4/5 so pairOne is a lookup, not a search.
4. After leftoverE=0 and workSolved=true on >=3 random 4x4: raise MITM, collect OBTM vs Ucas=288 / community 54.
5. Free-slice pairing with explicit wing tracking (source facelet + dest facelet + orient).
6. Surface leftoverE/workSolved in Android UI only after clean solves.
7. 3x3 dense pruning DBs toward proven HTM 20.
8. Never invent closed integer g(4). Window 35–54 OBTM. Demaine et al.: g(n)=Theta(n^2/log n) for n x n x n. Exact integers only n=2 and n=3.
9. CI: verify lib*.so in APK; adaptive icons.

## Approaches still queued
- Residual-key packing hand-off once edges solid.
- Higher BFS / MITM budgets via env for n>=5.
- Production signed APK + lib verification.
- Per-cell targeted center commutators if leftoverC regresses.
- Diameter search is not feasible: do not schedule an exhaustive 4x4 Cayley BFS.

*Session goal: flipped pairs get a second, two-move setup when the single quarter does not gain. Exact diameter remains open. Constructive algorithm for any n>3 always terminates.*
