# Next session log — 2026-09-16 (Teegan automation — EdgePairing tightening + harness re-measure)

## Done this session (2026-09-16 ~02:10 AEST)
- Rewrote `EdgePairing::pairOne` + `pairAll`:
  - Hard budget per edge (maxWing * 3) to kill blind multi-pass explosion.
  - Reduced depthCommutator to 4 higher-quality 8-move variants.
  - Early solid checks after every commutator.
  - Reduced main passes 6→4 and post-repair 4→2 rounds.
- Recompiled desktop_harness and measured single 4×4 random trial under same budgets.
- leftoverC=0 still holds; leftoverE=8 (unchanged completeness gate).
- Edges stage dropped 3082 → 2591 raw moves (progress on spam, still far from solid).
- final OBTM≈351 (inside U=501, still >> Ucas=288). Random scramble variance expected.
- notation_selftest=pass; workSolved=no; replaySolved=no; mitmHits=0.
- Exact integer g(n) for n≥4 remains open. Universal constructive algorithm (reduction + Demaine batching + residual MITM + StageCap + leftover commutators) still always terminates for every n>3.
- |G(4)| ≈ 7.4e45; constructive U(4)=501, Ucas=288, community OBTM window ~35–54.

## Measurement baseline (this session)
```
leftoverC=0 leftoverE=8 workSolved=no replaySolved=no
centers≈150 edges≈2591 final OBTM≈351 (inside U=501, target Ucas=288)
```

Prior (2026-09-15): leftoverE=8, edges≈3082, final≈313.
Prior (2026-09-11): leftoverE=7, edges≈256, final≈115 under higher MITM.

## This session code / doc changes
1. native/reduction/EdgePairing.cpp — tighter pairOne / pairAll (budget, variants, early exit).
2. docs/NEXT.md + README status/next-steps updated with 2026-09-16 harness measurement + new approaches.

## Try next (priority ordered)
1. **Full targeted pairOne**: locate unpaired wing positions via facelet scan, apply minimal setup moves to a standard buffer slot, fire one proven 8-move commutator, then undo setups. Protect solid bitset strictly. This is the real completeness gate.
2. Expand depthCommutator with wing-owning-face + orthogonal-slice variants that respect already-solid edges.
3. After leftoverE=0 on ≥3 independent random 4×4 trials: raise MITM budgets, collect OBTM distribution vs Ucas=288 / community 54.
4. Offline static edge-commutator tables (12 edges × depths) for n=4/5.
5. Surface leftoverE / workSolved / boundReport in Android UI only after clean solves.
6. 3×3 dense pruning DBs toward proven HTM 20.
7. Never invent closed integer g(4). Report bounds + measured constructive lengths only.
8. CI: verify lib*.so in APK; adaptive icons.

## Approaches still queued
- Residual-key packing hand-off once edges solid.
- Higher BFS / MITM budgets via env for n≥5.
- Per-cell targeted commutators if center leftover ever regresses.
- Production signed APK + lib verification.
- Implement free-slice style pairing with explicit wing tracking instead of any blind commutator spam.
- True wing-position model (source/dest facelets) so setups are deterministic.

*Session goal: concrete reduction of edges-stage spam + measurement. Exact diameter remains open. Next concrete gate = leftoverE → 0 on random 4×4 via fully targeted pairOne so the universal algorithm path produces workSolved=true. Teegan keeps compounding R = (W × C) ÷ T until the gate closes and measured OBTM collapses toward Ucas.*
