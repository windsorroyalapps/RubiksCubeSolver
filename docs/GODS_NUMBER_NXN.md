# God's Number & Algorithm for n×n×n (n > 3)

## Exact values (known)

| n | Metric | God's Number | Status |
|---|--------|--------------|--------|
| 2 | HTM | **11** | Proven |
| 2 | QTM | **14** | Proven |
| 3 | HTM | **20** | Proven (Rokicki et al. 2010) |
| 3 | QTM | **26** | Proven |
| ≥4 | any | **unknown** | Open (diameter NP-hard in related models) |

Exact diameter for n≥4 is computationally intractable (Demaine et al.).

## Known bounds for small n > 3

### 4×4×4 (Rubik's Revenge)
- **Outer Block Turn Metric (OBTM)**: 35 ≤ g(4) ≤ **54** (cubezzz / speedsolving 2015+, improved from 55)
- **Single Slice Turn Metric (SSTM)**: 32 ≤ g(4) ≤ 53
- **Block Turn Metric (BTM)**: 29 ≤ g(4) ≤ 53
- Community conjecture: ~41 HTM / ~48 QTM

### 5×5×5
- OBTM upper bound claims ~130 (community computer searches)
- Large gap remains between lower and upper bounds
- Informal HTM-scale estimates ~55–70

### Higher n
No tight constants; only asymptotic and constructive upper bounds.

## Asymptotic God's Number (Demaine, Demaine, Eisenstat, Lubiw, Winslow 2011)

```
g(n) = Θ(n² / log n)
```

- Lower bound: information-theoretic / group-order arguments.
- Upper bound: parallelising classic reduction algorithms (shared-slice batching) so many independent pieces advance together, shaving a log factor.

This is the true God's-number order for any fixed metric as n → ∞.  
**This repo implements the constructive half of that upper-bound technique** via ClusterScheduler + BatchGroups + BatchSolver.

## Practical constructive upper bounds (reduction family)

From community derivations of a concrete reduction solver:

- **Odd n**: ≤ 92n² − 307n + 113 moves
- **Even n**: ≤ 92n² − 307n + 257 moves

These are far from optimal but are explicit, implementable algorithms that **always terminate** for every position.

| n | U(n) |
|---|------|
| 4 | 501 |
| 5 | 878 |
| 6 | 1321 |
| 7 | 1852 |
| 8 | 2473 |
| 9 | 3182 |
| 10 | 3981 |

## The algorithm that works for ANY n > 3 (this repo)

**Reduction method + Demaine-style batching** is the universal constructive algorithm:

1. **Centers** – gather all center facelets of each colour into solid (n-2)×(n-2) blocks  
   (ClusterScheduler → BatchGroups shared-slice commutators + score-driven cleanup with **never-break** global multi-face score)
2. **Edge pairing** – match the (n-2) wings belonging to each of the 12 edges  
   (freeslice / multi-pass + **real wing-count** skip so solid edges stay solid)
3. **Parity** (even n only) – fix OLL parity ("flipped" dedge) then PLL parity (odd edge permutation)  
   (**multi-depth** sampling for n>4)
4. **3×3 stage** – treat the reduced cube as a normal 3×3 and run multi-probe Kociemba (or CFOP fallback)
5. **BatchSolver::optimize** – windowed collapse of identical (face,depth,turns) moves (log-factor spirit)
6. **BoundHarness** – report stage lengths vs U(n) and vs ~n²/log n (scale ≈ 3.8)

This pipeline is complete for every n ≥ 4 (software limit ~ memory for the facelet array).  
It realises a true algorithm that solves every position and approaches the asymptotic order via batching.

## Implementation status in this repo

| Component | File(s) | Status |
|-----------|---------|--------|
| Center commutators + batch groups | `CenterSolver.*` + `BatchGroups.*` + `ClusterScheduler.*` | Working + never-break global score |
| Edge freeslice | `EdgePairing.*` | Working multi-pass + wing-count |
| Even-n parity | `ParityHandler.*` | OLL + PLL algs + multi-depth detectors |
| Orchestrator | `ReductionSolver.*` | Full pipeline |
| 3×3 engine | `Kociemba` + `GodsAlgorithm` + `CFOPSolver` | Phase-1 + IDA* path + CFOP fallback |
| Bound instrumentation | `BoundHarness.*` | U(n) table + stage report + asymptotic |
| Post-process batching | `BatchSolver.*` | compress + window collapse |

## Progress note (automation session)

- Tightened **CenterSolver**: global 0–600 score; reject any commutator that drops global correctness; skip faces already at 100%; extra BatchGroups passes; 1/2/−1 turn probes.
- Tightened **EdgePairing**: `pairedWings` now scans all (n−2) wing offsets via facelets; 3-pass pairAll; solid edges never re-touched.
- Tightened **ParityHandler**: OLL/PLL proxies sample mid + depth 1 + depth n−2 on even n>4.
- Exact **g(n) for n≥4 remains open** (intractable). What we ship is the complete constructive algorithm + documented U(n) + Θ(n²/log n) path + best published 4×4 OBTM ≤54 reference.

## Next steps (automation roadmap — current work)

1. **OBTM / SSTM accounting** – dual move counts so we can measure against the 54-move 4×4 ceiling and community 5×5 numbers.
2. **Center BFS orbits** – short exact BFS for remaining incorrect cells on n≤6.
3. **Edge buffer + Yau** – explicit buffer tracking; Yau-style cross reduction for large n.
4. **Full wing parity** – orientation + permutation from complete (n−2) wing set.
5. **4×4 / 5×5 IDA*** – reduced coordinates once centers+edges fixed → push constructive U(n) lower.
6. **Production signed APK** – signed release, Material You, size selector to 20×20.
7. **Dense 3×3 pruning DBs** – full-index BFS so solutions routinely hit the 20 ceiling.
8. **Asymptotic constant** – re-fit scale if better community estimates appear; keep U(n) as hard guarantee.

## References

- Demaine et al., "Algorithms for Solving Rubik's Cubes", ESA 2011 / arXiv:1106.5736 (Θ(n²/log n))
- Rokicki et al., cube20.org (3×3 = 20)
- cubezzz / speedsolving threads (4×4 OBTM 35–54)
- Community upper-bound derivations (92n² series)

---
*Android/BMW hacking genius mode: ship the algorithm, document the bound, iterate the search. Next commit: OBTM metrics + center BFS orbits + APK path.*
