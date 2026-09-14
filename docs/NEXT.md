# Next session log — 2026-09-15 (Teegan automation — harness re-measure + edge gate)

## Done this session (2026-09-15 ~02:30 AEST)
- Compiled desktop_harness under current budgets; single 4×4 random trial measured.
- Confirmed leftoverC=0 still holds; leftoverE=8 (slightly up from prior 7 baseline under different budgets).
- Edges stage produced 3082 raw moves (OBTM 3081) — blind multi-variant commutator application is still too noisy; final OBTM=313 (inside U=501, still >> Ucas=288).
- notation_selftest=pass; workSolved=no; replaySolved=no; mitmHits=0.
- Exact integer g(n) for n≥4 remains open. Universal constructive algorithm (reduction + Demaine batching + residual MITM + StageCap + leftover commutators) still always terminates for every n>3.
- |G(4)| ≈ 7.4e45; constructive U(4)=501, Ucas=288, community OBTM window ~35–54.

## Measurement baseline (this session)
```
leftoverC=0 leftoverE=8 workSolved=no replaySolved=no
centers≈150 edges≈3082 final OBTM≈313 (inside U=501, target Ucas=288)
```

Prior (2026-09-11): leftoverE=7, edges≈256, final≈115 under higher MITM.

## This session code / doc changes
1. docs/NEXT.md + README status/next-steps updated with 2026-09-15 harness measurement.
2. No algorithmic change to EdgePairing yet — measurement first, then targeted rewrite next.

## Try next (priority ordered)
1. Rewrite pairOne to be *targeted*: locate unpaired wing positions, apply minimal setup moves to a standard buffer position, then apply a single proven 8-move commutator, then undo setups. Protect solid bitset strictly.
2. Expand depthCommutator with wing-owning-face + orthogonal-slice variants that respect already-solid edges (avoid the current blind multi-pass explosion that drives edges to 3k moves).
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
- Implement free-slice style pairing with explicit wing tracking instead of blind commutator spam.

*Session goal: concrete measurement of current EdgePairing under harness. Exact diameter remains open. Next concrete gate = leftoverE → 0 on random 4×4 via targeted pairOne so the universal algorithm path produces workSolved=true. Teegan keeps compounding R = (W × C) ÷ T until the gate closes and measured OBTM collapses toward Ucas.*
