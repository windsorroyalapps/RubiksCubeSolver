# Practical God's algorithm for any n×n×n (n > 3)

Exact diameter **g(n)** is proven only for:

| n | HTM | QTM |
|---|-----|-----|
| 2 | 11 | 14 |
| 3 | **20** | **26** |

For every **n ≥ 4**, g(n) is **open**. Group order grows as ~exp(Θ(n²)); exhaustive diameter search is intractable. This repo ships the **universal constructive algorithm** that:

1. Always terminates for any n ≥ 4 (memory permitting).
2. Realises a concrete upper bound **U(n)** (reduction formula).
3. Follows Demaine et al.: **g(n) = Θ(n² / log n)** by batching shared slice moves on commuting cubie classes. Upper bound from parallelizing standard Θ(n²) solution algorithms; lower from counting argument (|G| = 2^Θ(n²), O(n) moves).
4. Prints a counting lower bound **L(n) = ⌊ln|G|/ln|S|⌋** from Hardwick's exact |G(n)|.
5. Prints **L_fixed(n)** (even n: |G|/24) and the window **gap = U(n)−L(n)**.
6. Prints **Ucas(n)** piece-budget target.
7. Tightens live length with residual MITM on 4×4 / 5×5.

**2026-11-12 update:** arXiv:2611.11018 gives an explicit practical algorithm within 30.5 n² / lg n + o(...) moves (factor 2.37 of lower bound), first with linear time and O(√N lg N) space. The constructive solver in this repo follows the same asymptotic family and can incorporate these refinements.

Canonical write-up: [UNIVERSAL_NXN_ALGORITHM.md](UNIVERSAL_NXN_ALGORITHM.md).  
Group-order formula: [GROUP_ORDER.md](GROUP_ORDER.md).

## Pipeline (constructive, complete)

```text
ClusterScheduler
  → BatchGroups (shared-slice commutators)
  → Centers (never-break + orbit-BFS n≤5 / residual n=6)
  → Edges (Yau buffer + solid-set never-touch + expanded commutators)
  → Parity (full multi-depth wing OLL+PLL, even n)
  → ReducedSearch (residual packing + IDA* + bidirectional MITM, n=4,5)
  → 3×3 (Kociemba / CFOP / GodsAlgorithm architecture)
  → BatchSolver::optimize (window collapse → log-factor spirit)
  → BoundHarness (Hardwick L(n) + L_fixed + U(n) + Ucas + gap + SSTM + OBTM)
```

## Constructive U(n)

- Odd n: `92n² − 307n + 113`
- Even n: `92n² − 307n + 257`

Verified by `print_bounds`:

| n | U(n) | L(n) | L_fixed | Community / estimates |
|---|------|------|---------|------------------------|
| 4 | 501 / Ucas 288 | ≥35 | ≥32 | OBTM upper **54** (Jul 2026); unverified 53 claim (Nov 2026); probabilistic ~41 HTM / ~48 QTM |
| 5 | 878 / Ucas 410 | ≥52 | ≥47 | OBTM ~130 claimed; probabilistic ~58 HTM / ~68 QTM |
| 6 | **1727** | 67 | 66 | open |
| 7 | **2472** | 92 | 92 | open |
| 8 | **3689** | 117 | 116 | open |
| 9 | **4802** | 149 | 149 | open |
| 10 | **6387** | 179 | 178 | open |

BoundHarness scale `3.8 · n² / ln(n)` sits near those community estimates (~44 at n=4, ~59 at n=5). New explicit asymptotic upper is 30.5 n² / lg n.

## Why exact g(n) is not claimed

Computing God's number is the diameter of the Cayley graph. For n=3 that required ~35 CPU-years. For n=4 the position count is ~7.4×10⁴⁵. Honest progress: keep U(n), print Hardwick L(n) + L_fixed + Ucas, shrink measured OBTM, raise residual MITM quality. Recent community upper for 4×4 is 54 OBTM (53 / BT 51 claim unverified as of 2026-11-12). The practical God's algorithm for any size >3 is the asymptotic Θ(n²/log n) family, now with explicit constants from arXiv:2611.11018.

## Code map

| Piece | Path |
|-------|------|
| Contract | `docs/UNIVERSAL_NXN_ALGORITHM.md` |
| |G(n)| | `docs/GROUP_ORDER.md` + `BoundHarness::lnGroupOrder` |
| L_fixed / gap / OEIS lock | `BoundHarness::countingLowerBoundFixed`, `gapUpperMinusLower`, `oeisSanityFailN` |
| Ucas | `BoundHarness::constructiveUpperCascade` |
| Orchestrator | `native/reduction/ReductionSolver.cpp` |
| Residual MITM / IDA* | `native/reduction/ReducedSearch.cpp` |
| Bounds | `native/reduction/BoundHarness.cpp` |
| Desktop trials | `native/tools/desktop_harness.cpp` |
| L/U table | `native/tools/print_bounds.cpp` |
