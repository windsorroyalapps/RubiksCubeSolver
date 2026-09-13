# Next session log — 2026-09-14 (Teegan automation — edge commutator rewrite)

## Done this session (2026-09-14 ~02:20 AEST)
- Full review of EdgePairing.cpp / .h, CenterSolver commutator pattern, UNIVERSAL_NXN_ALGORITHM, BoundHarness, StageCap.
- **Rewrote EdgePairing::pairOne** to pure depth-specific 8-move wing commutator family (6 variants of A B A' B' style).
- Expanded pairAll to 6 passes + explicit post-pairAll leftover repair loop (up to 4 repair rounds of depth-targeted commutators).
- Goal remains: drive leftoverE → 0 on random 4×4 so workSolved / replaySolved fire.
- Exact integer g(n) for n≥4 still open research. Universal constructive algorithm already terminates for every n>3.
- |G(4)| ≈ 7.4e45; constructive U(4)=501, Ucas=288, community OBTM window ~35–54.

## Measurement baseline (carry-forward from 2026-09-11)
```
leftoverC=0 leftoverE=7 workSolved=no replaySolved=no
centers≈151 edges≈256 final OBTM≈115 (inside U=501, target Ucas=288)
```

## This session code changes
1. native/reduction/EdgePairing.cpp — depthCommutator() + multi-variant pairOne + post-repair.
2. docs/NEXT.md + README status/next-steps updated for continuity.

## Try next (priority ordered)
1. Compile + run desktop_harness 4 5 (or 4 10) under current RCS_* budgets; measure leftoverE on ≥3 independent random trials. Target leftoverE==0 + workSolved=yes + replaySolved=yes.
2. If leftoverE still >0: expand depthCommutator table with more wing-specific setups (owning face + orthogonal slice) and protect solid bitset more aggressively inside the repair loop.
3. Once leftoverE=0 reliably: increase MITM node/depth, collect OBTM distribution vs Ucas=288 / community 54.
4. Offline static edge-commutator tables (12 edges × depths) for n=4/5 to replace runtime generation.
5. Surface leftoverE / workSolved / boundReport in Android UI only after clean solves.
6. 3×3 dense pruning DBs toward proven HTM 20.
7. Never invent closed integer g(4). Report bounds + measured constructive lengths only.
8. CI: verify lib*.so in APK; adaptive icons.

## Approaches still queued
- Residual-key packing hand-off once edges solid.
- Higher BFS / MITM budgets via env for n≥5.
- Per-cell targeted commutators if center leftover ever regresses.
- Production signed APK + lib verification.

*Session goal: concrete EdgePairing rewrite shipped. Exact diameter remains open. Next concrete gate = leftoverE → 0 on random 4×4 so the universal algorithm path produces workSolved=true. Teegan keeps compounding R = (W × C) ÷ T until the gate closes and measured OBTM collapses toward Ucas.*
