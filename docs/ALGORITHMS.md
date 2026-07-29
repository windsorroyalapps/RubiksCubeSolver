# Solving Algorithms

## 3×3 — CFOP (Fridrich) + Kociemba

1. **Cross** — White (or any color) cross on bottom, edges aligned with centers.
2. **F2L** — Pair + insert 4 corner-edge pairs (41 basic cases).
3. **OLL** — Orient last layer (57 algs full / ~10 for 2-look).
4. **PLL** — Permute last layer (21 algs full / 2-look).

**God's Number (HTM): 20.** Proven.

Kociemba two-phase (coord IDA*) is the primary engine; CFOP is the reliable fallback.

## 4×4+ — Reduction Method (universal for any n)

1. **Solve Centers**  
   Group all center pieces of each face into solid blocks.  
   Odd n: fixed centers. Even n: choose colour scheme.  
   *This repo:* ClusterScheduler → BatchGroups shared-slice commutators + score cleanup.

2. **Edge Pairing**  
   Match the (n-2) wing pieces that belong to each of the 12 edges.  
   Techniques: Freeslice, Yau, Hoya, pure pair-by-pair.

3. **Parity (even n)**  
   OLL parity ("flipped" dedge) then PLL parity (odd permutation of edges).

4. **3×3 Stage**  
   Treat reduced cube as normal 3×3 with Kociemba/CFOP.

5. **Batch optimize**  
   Windowed collapse of identical (face, depth, turns) moves — Demaine log-factor spirit.

Same pipeline works for any n up to device memory (design target 1000×1000 software-only).

## Theoretical Bound (Demaine et al. 2011)

God's Number for n×n×n is **Θ(n² / log n)**.  
Upper bound obtained by parallelising classic Θ(n²) reduction algorithms.

Exact values only known for n = 2 and n = 3. For n ≥ 4 the diameter remains open; only bounds exist (see [GODS_NUMBER_NXN.md](GODS_NUMBER_NXN.md)).  
Best published 4×4 OBTM upper: **54**.

## Implementation Notes (this repo)

- `native/cfop/` → 3×3 CFOP + Kociemba two-phase + GodsAlgorithm
- `native/reduction/` → centers + edge pairing + parity + batching + BoundHarness for arbitrary n
- `native/common/` → cube state, move notation, facelet model

## Next approaches (current work on this repo)

- Dense full-index pruning tables for 3×3 (close the gap to 20)
- Look-ahead / BFS center solving for small-to-medium n
- Full wing-parity detection (not mid-slice proxy)
- Emit move counts in multiple metrics (OBTM/SSTM) for bound comparison against 54
- Scaffolding for diameter search on reduced 4×4 / 5×5 state spaces
- Production signed APK + Material You polish
