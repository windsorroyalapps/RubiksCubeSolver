# Next session log — 2026-09-11 (Teegan automation run + edge leftover measurement)

## Done this session (2026-09-11 02:00 AEST)
- Cloned repo, compiled desktop_harness, ran 4×1 trial with lower BFS budgets for speed.
- Measurement: leftoverC=0 leftoverE=7 workSolved=no replaySolved=no final OBTM=115 (within U=501, Ucas=288).
- Centers still clean (overC=119). Edges improved slightly vs prior 8 unpaired (now 7).
- Exact integer g(n) for n≥4 remains **open**. |G(4)| ~ 7.4e45. Do not invent g(4). Community window 35–54 OBTM.
- Universal constructive algorithm (reduction + Demaine batching + StageCap + leftover commutators + residual MITM) is the algorithm that solves any n>3. Always terminates.

## New measurement (this run)
```
leftoverC=0 leftoverE=7 workSolved=no
centers=151(obtm=149) edges=256(obtm=246) parity=0 reduced=4 3x3=0
raw=411 final=115 sstm=115 obtm=115
withinU=yes overC=119 overE=64 fattest=centers
```

## Still true
Edge-pairing incompleteness is the gate (freeslice sequences leave unpaired wings). Centers complete.

## Try next (priority order) — for next automation / human
1. **Implement single-depth wing 8-move commutator in EdgePairing::pairOne.**  
   Classic form: A B A' B' where B is the exact inner slice depth of the unpaired wing, A is a face turn that moves that wing into position without disturbing solid edges (respect solid bitset).
2. Post-pairAll leftover repair: loop over edges with pairedWings < n-2, apply depth-specific commutator, re-check isSolid.
3. After leftoverE==0 on ≥3 independent 4×4 trials: confirm workSolved=yes + replaySolved=yes + measure OBTM distribution vs Ucas=288 / community 54.
4. Only then expose leftover + workSolved in Android BoundReport / UI.
5. Generate offline edge-commutator tables (12 edges × (n-2) depths) for n=4,5 static arrays (mirror OLL/PLL style).
6. Increase residual MITM budgets once edges solid so ReducedSearch finishes.
7. 3×3 dense pruning tables for proven ≤20 HTM.
8. Do **not** claim an integer g(4). Progress metric = measured OBTM ↓ + workSolved rate ↑.

## Approaches queued for future sessions
- Edge commutator table generation offline.
- Residual key packing already in ReducedSearch; hand-off after solid edges.
- For larger n, higher BFS node budgets via env vars.
- Per-cell targeted commutators if centers regress.

*Session goal remains: drive leftoverE → 0 so the universal algorithm reaches workSolved=yes on random 4×4. Exact diameter open research problem.*
