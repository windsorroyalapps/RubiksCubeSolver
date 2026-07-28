# God's Number & Algorithm for n×n×n (n > 3)

## Exact values (known)

| n | Metric | God's Number | Status |
|---|--------|--------------|--------|
| 2 | HTM | **11** | Proven |
| 2 | QTM | **14** | Proven |
| 3 | HTM | **20** | Proven (Rokicki et al. 2010) |
| 3 | QTM | **26** | Proven |
| ≥4 | any | **unknown** | Open |

Exact diameter for n≥4 is computationally intractable (Demaine et al.: computing the diameter is NP-hard).

## Known bounds for small n > 3

### 4×4×4 (Rubik's Revenge)
- **Outer Block Turn Metric (OBTM)**: 35 ≤ g(4) ≤ 55 (later tightened to ~54)
- **Single Slice Turn Metric (SSTM)**: 32 ≤ g(4) ≤ 53
- **Block Turn Metric (BTM)**: 29 ≤ g(4) ≤ 53
- Conjectures in community: ~41 HTM / ~48 QTM

### 5×5×5
- OBTM upper bound claims around 130 (community computer searches)
- Large gap remains between lower and upper bounds

### Higher n
No tight constants; only asymptotic and constructive upper bounds.

## Asymptotic God's Number (Demaine, Demaine, Eisenstat, Lubiw, Winslow 2011)

```
g(n) = Θ(n² / log n)
```

- Lower bound comes from information-theoretic / group-order arguments.
- Upper bound obtained by parallelising classic reduction algorithms so that many independent pieces are solved simultaneously, shaving a log factor.

This is the true God's number order for any fixed metric (HTM / QTM / etc.) as n → ∞.

## Practical constructive upper bounds

From community derivations (reduction + 3×3 stage):

- **Odd n**: ≤ 92n² − 307n + 113 moves
- **Even n**: ≤ 92n² − 307n + 257 moves

These are far from optimal but are explicit, implementable algorithms that always terminate.

## The algorithm that works for ANY n > 3 (this repo)

**Reduction method** is the universal constructive algorithm:

1. **Centers** – gather all center facelets of each colour into solid (n-2)×(n-2) blocks  
   (commutators + inner-slice turns; order U/D then belt)
2. **Edge pairing** – match the (n-2) wings belonging to each of the 12 edges  
   (freeslice / Yau-style cycles)
3. **Parity** (even n only) – fix OLL parity ("flipped" dedge) then PLL parity (odd edge permutation)
4. **3×3 stage** – treat the reduced cube as a normal 3×3 and run Kociemba two-phase (or CFOP fallback)

This pipeline is complete for every n ≥ 4 (software limit ~ memory for the facelet array).  
It does **not** achieve the asymptotic optimal diameter, but it is a true algorithm that solves every position.

## Implementation status in this repo

| Component | File(s) | Status |
|-----------|---------|--------|
| Center commutators | `CenterSolver.*` | Working heuristic (score-driven) |
| Edge freeslice | `EdgePairing.*` | Working multi-pass |
| Even-n parity | `ParityHandler.*` | OLL + PLL algs + detectors |
| Orchestrator | `ReductionSolver.*` | Full pipeline |
| 3×3 engine | `Kociemba` + `CFOPSolver` | Phase-1 depth 14 + CFOP fallback |

## Next steps (automation roadmap)

1. **Tighter center heuristics** – replace pure score greedy with BFS on center orbits for n≤8; look-ahead commutator selection.
2. **Better edge pairing** – implement pure pair-by-pair + buffer tracking so that already-paired wings are never broken; Yau-style cross reduction for speed on large n.
3. **Parity detection robustness** – full wing orientation / permutation parity from the complete set of (n-2) wings instead of the mid-slice proxy.
4. **Move count accounting** – emit exact outer-block / single-slice counts so we can compare against the 92n² formulas.
5. **God's-number search scaffolding** – for small n (4,5) add IDA* / bidirectional search over reduced coordinates once centers+edges are fixed, to push the upper bounds lower.
6. **Production APK** – signed release build, Material You UI polish, size selector up to 20×20 on-device (higher offline).
7. **Dense pruning tables for 3×3** – finish the full-index BFS tables so phase-1 routinely lands ≤12 and total solutions approach the 20 ceiling.

## References

- Demaine et al., "Algorithms for Solving Rubik's Cubes", 2011 (Θ(n²/log n))
- Rokicki et al., cube20.org (3×3 = 20)
- Speedsolving wiki / cubezzz discussions (4×4 bounds)
- Community upper-bound derivations (92n² series)

---
*Android/BMW hacking genius mode: ship the algorithm, document the bound, iterate the search.*
