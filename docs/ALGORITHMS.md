# Solving Algorithms

## 3×3 — CFOP (Fridrich)

1. **Cross** — White (or any color) cross on bottom, edges aligned with centers.
2. **F2L** — Pair + insert 4 corner-edge pairs (41 basic cases).
3. **OLL** — Orient last layer (57 algs full / ~10 for 2-look).
4. **PLL** — Permute last layer (21 algs full / 2-look).

God's Number: **20**.

## 4×4+ — Reduction Method

1. **Solve Centers**  
   Group all center pieces of each face into solid blocks.  
   Odd n: fixed centers. Even n: choose color scheme.

2. **Edge Pairing**  
   Match the (n-2) wing pieces that belong to each of the 12 edges.  
   Techniques: Freeslice, Yau, Hoya, pure pair-by-pair.

3. **3×3 Stage**  
   Treat reduced cube as normal 3×3 with CFOP.  
   Handle parity cases on even-order cubes (OLL parity, PLL parity).

Same pipeline works for any n up to 1000+ (and beyond).

## Theoretical Bound (Demaine et al. 2011)

God's Number for n×n×n is **Θ(n² / log n)**.  
Upper bound obtained by parallelizing classic Θ(n²) reduction algorithms.

## Implementation Notes (this repo)

- `native/cfop/` → 3×3 CFOP + Kociemba two-phase
- `native/reduction/` → centers + edge pairing for arbitrary n
- `native/common/` → cube state, move notation, facelet model
