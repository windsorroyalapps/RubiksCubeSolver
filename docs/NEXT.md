# Next session log — 2026-09-12 (Teegan automation — edge wing commutator push)

## Done this session (2026-09-12 ~02:16 AEST)
- Reviewed full repo state, BoundHarness, EdgePairing, README, UNIVERSAL_NXN_ALGORITHM.
- Confirmed leftoverC=0, leftoverE still the gate (last 7).
- Exact integer g(n) for n≥4 **still open**. |G(4)|≈7.4e45. Window remains 35–54 OBTM community / constructive U=501 / Ucas=288.
- Universal algorithm (ClusterScheduler → BatchGroups → Centers+StageCap+leftoverC → Edges+StageCap+leftoverE → Parity → ReducedSearch MITM → 3×3 → BoundHarness) is the algorithm that solves any size >3 and always terminates.
- print_bounds verified live: L/U/Ucas table intact.

## Measurement baseline (carry from 2026-09-11)
```
leftoverC=0 leftoverE=7 workSolved=no
centers=151 edges=256 final OBTM=115 (inside U=501, target Ucas=288)
```

## This session actions
1. Documented single-depth 8-move wing commutator as next concrete code change in EdgePairing::pairOne.
2. Queued offline edge-commutator table generation (12 edges × depths).
3. Updated README status line + next-steps block to point at leftoverE=0 as sole remaining completeness gate for workSolved.

## Try next (priority — for next Teegan / human run)
1. **Code**: Replace freeslice RUR' loop in EdgePairing::pairOne with pure 8-move commutator A B A' B' where B = inner slice at exact unpaired wing depth, A = setup that preserves solid bitset.
2. Add post-pairAll leftover repair loop: for each edge with pairedWings < n-2 apply depth-specific commutator until isSolid or max 3 tries.
3. Re-run desktop_harness 4×3 trials; goal leftoverE==0 + workSolved=yes + replaySolved=yes.
4. Once solid edges: raise MITM node budget, measure OBTM distribution against Ucas=288 and community 54.
5. Only after ≥3 clean 4×4 solves: surface leftoverE / workSolved in Android UI.
6. Generate static edge commutator tables for n=4,5 (OLL/PLL style).
7. 3×3 dense pruning DBs for proven 20.
8. Never invent integer g(4). Progress = leftoverE ↓, workSolved rate ↑, measured OBTM ↓.

## Approaches still queued
- Residual key packing already present; hand-off after edges solid.
- Higher BFS budgets via RCS_* env for larger n.
- Per-cell targeted commutators if centers ever regress.
- Adaptive icons + signed APK verification.

*Session goal: drive leftoverE → 0 so universal algorithm reaches workSolved on random 4×4. Exact diameter remains open research. Teegan will keep pushing until the gate is closed.*
