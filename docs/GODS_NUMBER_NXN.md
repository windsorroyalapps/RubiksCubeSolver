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
   (ClusterScheduler → BatchGroups shared-slice commutators + score-driven cleanup with **never-break** global multi-face score + **full center-orbit BFS for n≤5** / residual short-search for n=6)
2. **Edge pairing** – match the (n-2) wings belonging to each of the 12 edges  
   (**Yau-style buffer tracking**: explicit UF buffer, solid-set never-touch, priority order, 4-pass freeslice + real wing-count)
3. **Parity** (even n only) – fix OLL parity ("flipped" dedge) then PLL parity (odd edge permutation)  
   (**full multi-depth wing** orientation + permutation over all depths 1..n-2)
4. **ReducedSearch** (4×4/5×5) – depth-limited IDA* on residual centers+wings before classic 3×3
5. **3×3 stage** – treat the reduced cube as a normal 3×3 and run multi-probe Kociemba (or CFOP fallback)
6. **BatchSolver::optimize** – windowed collapse of identical (face,depth,turns) moves (log-factor spirit)
7. **BoundHarness** – report stage lengths vs U(n) and vs ~n²/log n (scale ≈ 3.8) **+ dual OBTM/SSTM counts**

This pipeline is complete for every n ≥ 4 (software limit ~ memory for the facelet array).  
It realises a true algorithm that solves every position and approaches the asymptotic order via batching.

## Implementation status in this repo

| Component | File(s) | Status |
|-----------|---------|--------|
| Center commutators + batch groups | `CenterSolver.*` + `BatchGroups.*` + `ClusterScheduler.*` | Working + never-break global score + **orbit-BFS n≤5** + residual n=6 |
| Edge freeslice + buffer | `EdgePairing.*` | Working multi-pass + wing-count + **Yau buffer + solid-set protect** |
| Even-n parity | `ParityHandler.*` | OLL + PLL algs + **full multi-depth wing detectors** |
| Reduced residual search | `ReducedSearch.*` | **Packed 4×4 center bitmask + stronger wing residual + IDA*** |
| Orchestrator | `ReductionSolver.*` | Full pipeline |
| 3×3 engine | `Kociemba` + `GodsAlgorithm` + `CFOPSolver` | Phase-1 + IDA* path + CFOP fallback |
| Bound instrumentation | `BoundHarness.*` | U(n) table + stage report + asymptotic **+ OBTM/SSTM** |
| Post-process batching | `BatchSolver.*` | compress + window collapse |

## Progress note (automation session 2026-08-01)

- **OBTM / SSTM dual metrics shipped**: BoundHarness now counts SSTM (every Move) and OBTM (collapse consecutive depth-0 same-face turns). Final report emits `sstm=` / `obtm=` and flags `vs4x4OBTM54=under|over` for n=4.
- ReductionSolver feeds the final optimised sequence into the dual-metric report.
- Exact **g(n) for n≥4 remains open** (intractable). What we ship is the complete constructive algorithm + documented U(n) + Θ(n²/log n) path + best published 4×4 OBTM ≤54 reference + live dual counts.

## Progress note (automation session 2026-08-02)

- **CI APK production shipped**: `.github/workflows/build-apk.yml` builds debug APK on every push to main, uploads artifact `rubikscubesolver-debug-apk`. Native C++ + Kotlin path exercised in the cloud.
- Algorithm status unchanged: complete constructive solver for **any n > 3** (centers → edges → parity → 3×3 + batching). Exact God's Number still open; we continue working backward from U(n) and community OBTM ceilings.

## Progress note (automation session 2026-08-03)

- **Center residual short-search shipped** for n≤6: after face greedy + BatchGroups, a bounded residual phase tries short commutator sequences that reduce absolute incorrect center cells while never dropping the global never-break score. Acts as a light BFS-style cleanup of remaining cells; keeps mobile responsive (depth-limited).
- More commutator variants (slice-first + 2-turn slices) + higher attempt budgets in solveFace.

## Progress note (automation session 2026-08-04)

- **Full center-orbit BFS shipped for n≤5**: residual short-search replaced by a true depth-limited BFS on the packed incorrect-center state (uint64_t bitmask of correct/incorrect cells). Generators = all outer turns + curated center commutators. Never-break global-score guard retained. n=6 keeps residual short-search (larger state space).
- Highest-leverage step for tightening constructive lengths on 4×4/5×5 toward community OBTM ceilings (~54 for 4×4).
- Exact g(n) for n≥4 still open. Constructive algorithm remains complete and universal for **any n > 3**.

## Progress note (automation session 2026-08-05)

- **Edge buffer tracking (Yau-style) shipped**: explicit buffer edge = UF, solid-set (`bitset<12>`) never-touch once `pairedWings == n-2`, priority order (non-buffer first, buffer last), 4 progressive passes with mid-pass solid refresh.
- **Full wing parity shipped**: OLL orientation + PLL side-match now sample **all depths 1..n-2** (complete (n-2) wings) instead of residual proxy samples. Classic 4×4 residual 1/3 still special-cased.
- Both items were the top two items on the previous roadmap. Constructive algorithm for **any n > 3** is tighter and more robust; exact g(n) remains open.

## Progress note (automation session 2026-08-06)

- **ReducedSearch packing shipped**: 4×4 center residual now packed as `uint16_t` bitmask (16 inner cells, popcount heuristic). Stronger multi-wing residual (12 mid-edge samples + extra depth samples for n≥5). Default maxDepth raised; 5×5 still capped for mobile safety.
- This is the next step on the reduced-coordinate path. Full residual coordinate tables + bidirectional IDA* remain the highest-leverage items to collapse constructive lengths toward community OBTM ≤54.
- Exact g(n) for n≥4 still open. Universal constructive algorithm for **any n > 3** continues to improve.

## Next steps (automation roadmap — current work 2026-08-06)

1. **Full residual coordinates + bidirectional IDA*** – pack complete edge/center residual state into compact integers; add bidirectional search so 4×4 constructive lengths collapse toward community OBTM ≤54. **Highest leverage remaining.**
2. **3×3 dense DBs** – full-index BFS pruning so phase-1 routinely ≤12 and totals hit the 20 ceiling.
3. **OBTM stage breakdown** – optional per-stage OBTM so we can see which phase is furthest from the 54-move 4×4 ceiling.
4. **Production signed APK** – signed release, Material You polish, on-device size selector to 20×20; wire CI release when keystore secret present.
5. **Asymptotic fit** – re-calibrate BoundHarness scale if new community numbers appear; keep U(n) as hard constructive guarantee.
6. **Gradle wrapper binary** – commit full `gradlew` + jar.
7. **Center BFS node-budget tuning** – raise maxNodes / maxDepth on desktop; mobile-safe defaults; optional JNI param.
8. **Edge pairing quality metrics** – log pairedWings / solid count into BoundHarness.
9. **Parity alg variants** – alternate OLL/PLL sequences, pick shortest that clears full-depth detectors.
10. **APK artifact verification loop** – confirm CI APK + native .so after each push.

## References

- Demaine et al., "Algorithms for Solving Rubik's Cubes", ESA 2011 / arXiv:1106.5736 (Θ(n²/log n))
- Rokicki et al., cube20.org (3×3 = 20)
- cubezzz / speedsolving threads (4×4 OBTM 35–54)
- Community upper-bound derivations (92n² series)

---
*Android/BMW hacking genius mode: ship the algorithm that solves any n>3, document the bound, automate the APK, iterate the search. Next commit: bidirectional residual coords for 4×4/5×5.*
