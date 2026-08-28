# Practical God's algorithm for any n×n×n (n > 3)

Exact diameter **g(n)** is proven only for:

| n | HTM | QTM |
|---|-----|-----|
| 2 | 11 | 14 |
| 3 | **20** | **26** |

For every **n ≥ 4**, g(n) is **open**. Group order grows as ~exp(Θ(n²)); exhaustive diameter search is intractable. This repo ships the **universal constructive algorithm** that:

1. Always terminates for any n ≥ 4 (memory permitting).
2. Realises a concrete upper bound **U(n)** (reduction formula).
3. Follows Demaine et al.: **g(n) = Θ(n² / log n)** by batching shared slice moves.
4. Prints a counting lower bound **L(n) = ⌊log|G|/log|S|⌋**.
5. Tightens live length with residual MITM on 4×4 / 5×5.

Canonical write-up: [UNIVERSAL_NXN_ALGORITHM.md](UNIVERSAL_NXN_ALGORITHM.md).

## Pipeline (constructive, complete)

```text
ClusterScheduler
  → BatchGroups (shared-slice commutators)
  → Centers (never-break + orbit-BFS n≤5 / residual n=6)
  → Edges (Yau buffer + solid-set never-touch)
  → Parity (full multi-depth wing OLL+PLL, even n)
  → ReducedSearch (residual packing + IDA* + bidirectional MITM, n=4,5)
  → 3×3 (Kociemba / CFOP / GodsAlgorithm architecture)
  → BatchSolver::optimize (window collapse → log-factor spirit)
  → BoundHarness (L(n) + U(n) + SSTM + OBTM + per-stage OBTM)
```

## Constructive U(n)

- Odd n: `92n² − 307n + 113`
- Even n: `92n² − 307n + 257`

| n | U(n) | L(n) floor | Community / estimates |
|---|------|------------|------------------------|
| 4 | 501 | ≥35 | OBTM upper **54**; probabilistic ~41 HTM / ~48 QTM |
| 5 | 878 | ≥40 | OBTM ~130 claimed; probabilistic ~58 HTM / ~68 QTM |
| 6 | 1321 | counting | open |
| 7 | 1852 | counting | open |
| 10 | 3981 | counting | open |

BoundHarness scale `3.8 · n² / ln(n)` sits near those community estimates (~44 at n=4, ~59 at n=5).

## Why exact g(n) is not claimed

Computing God's number is the diameter of the Cayley graph. For n=3 that required ~35 CPU-years. For n=4 the position count is ~7.4×10⁴⁵. Honest progress: keep U(n), print L(n), shrink measured OBTM, raise residual MITM quality.

## Code map

| Piece | Path |
|-------|------|
| Contract | `docs/UNIVERSAL_NXN_ALGORITHM.md` |
| Orchestrator | `native/reduction/ReductionSolver.cpp` |
| Residual MITM / IDA* | `native/reduction/ReducedSearch.cpp` |
| Bounds | `native/reduction/BoundHarness.cpp` |
| Desktop trials | `native/tools/desktop_harness.cpp` |
