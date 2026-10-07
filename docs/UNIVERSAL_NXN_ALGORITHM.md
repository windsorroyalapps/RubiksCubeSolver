# Universal algorithm for any n x n x n, n > 3

Contract. This is the constructive solver the repo ships. It is not a proof of God's number.

## Status

- Always terminates for any n >= 4 (memory permitting).
- Realises constructive upper bound U(n): odd `92n^2 - 307n + 113`, even `92n^2 - 307n + 257`.
- Prints counting lower L(n) = floor(ln|G|/ln|S|) from Hardwick, lifted to community 35 (n=4) and 52 (n=5).
- Prints L_fixed, gap, Ucas, OBTM, leftoverC, leftoverE.
- Exact integer g(n) for n >= 4 is open. Do not publish a closed diameter.

## Pipeline

```text
ClusterScheduler
  -> BatchGroups
  -> Centers (never-break + orbit-BFS n<=5)
  -> StageCap(C) + leftover center commutators
  -> Edges (facelet map + flip-aware slice setup + solid-set)
  -> StageCap(E) + leftover edge commutators
  -> Parity (even n)
  -> ReducedSearch (IDA* + residual MITM, n=4,5)
  -> 3x3 (Kociemba / CFOP / GodsAlgorithm architecture)
  -> BatchSolver::optimize
  -> BoundHarness
```

## 2026-10-08 edge step

`locateTargetWings` tags orientation. A flipped source (`orient = -1`) receives `flipSliceSetup` before the depth commutator. No-gain sequences are undone. This is the current attack on leftoverE=6-7. It does not change U(n) or L(n).

## What is not claimed

God's number is the diameter of the Cayley graph. Proven only for n=2 (11 HTM) and n=3 (20 HTM / 26 QTM). |G(4)| is about 7.4e45. Community 4x4 OBTM window remains 35-54. Asymptotic result (Demaine et al.) is g(n) = Theta(n^2 / log n), which the batching stage follows in spirit.
