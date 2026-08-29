# Universal algorithm for any n×n×n with n > 3

This is the algorithm this repo treats as **complete** for every size greater than 3.

It is **not** a closed-form exact God's number. Exact diameter g(n) is proven only for n=2 (11 HTM) and n=3 (20 HTM / 26 QTM). For n≥4 the Cayley-graph diameter is open; |G(4)| ≈ 7.4×10⁴⁵.

What *is* settled:

1. **Termination.** Reduction always finishes for any n≥4 (memory permitting).
2. **Constructive upper bound** U(n) — every reachable state solves in ≤ U(n) moves of this algorithm family.
3. **Asymptotic God's number** g(n) = Θ(n² / log n) (Demaine et al. 2011).
4. **Counting lower bound** L(n) = ⌊ ln|G| / ln|S| ⌋ with Hardwick's exact |G(n)| and |S| = 6·⌊n/2⌋·3.

## Algorithm (constructive God's-algorithm family)

Input: scrambled n×n×n, n≥4.

```
1. ClusterScheduler      partition cubies into commuting slice classes
2. BatchGroups           shared-slice commutators (Demaine parallel factor)
3. CenterSolver          never-break orbits; BFS n≤5; residual n≥6
4. EdgePairing           Yau buffer + solid-set never-touch wings
5. ParityHandler         even n only: wing OLL + PLL parity at all depths
6. ReducedSearch         n∈{4,5}: packed residual IDA* + bidirectional MITM
7. 3×3 kernel            Kociemba → GodsAlgorithm ≤20 → CFOP fallback
8. BatchSolver.optimize  window collapse → log-factor compression
9. BoundHarness          emit Hardwick L(n), U(n), SSTM, OBTM, per-stage OBTM
10. Cube::applyNotation  SiGN replay (2R / Rw / 3Rw / M E S)
```

Output: SiGN move string + BoundReport.

## Bounds the harness prints

| symbol | meaning |
|--------|---------|
| L(n) | Hardwick counting + community lower |
| U(n) | constructive reduction formula |
| log10\|G\| | Hardwick group order |
| asym | 3.8 · n² / ln n |
| OBTM≤54 | published 4×4 outer-block upper (not ours) |

U(n):
- odd n:  92n² − 307n + 113
- even n: 92n² − 307n + 257

## Honest statement for the README claim

> "God's number and algorithm for any size > 3"

- **Algorithm:** yes — this pipeline, implemented under `native/reduction/`.
- **Asymptotic number:** yes — Θ(n² / log n).
- **Exact integer g(n):** no, and this repo will not invent one. Closing g(4) is a multi-CPU-year research program, not a phone solver.

Progress metric for the next session: `print_bounds` OEIS check + measured OBTM / U(n) / L(n) on desktop_harness + replaySolved rate.
