# Universal algorithm for any n x n x n, n > 3

Contract. This is the constructive solver the repo ships. It is not a proof of God's number.

## Status

- Always terminates for any n >= 4 (memory permitting).
- Realises constructive upper bound U(n): odd `92n^2 - 307n + 113`, even `92n^2 - 307n + 257`.
- Prints counting lower L(n) = floor(ln|G|/ln|S|) from Hardwick, lifted to community 35 (n=4) and 52 (n=5).
- Prints L_fixed, gap, Ucas, OBTM, leftoverC, leftoverE.
- Exact integer g(n) for n >= 4 is open. Do not publish a closed diameter.
- Community 4x4 OBTM upper now **54** (Jul 2026 computer-assisted phase bound). Unverified 53 / BT 51 claim (Nov 2026 manuscript) noted but not adopted.
- Asymptotic diameter g(n) = Θ(n² / log n) (Demaine et al., arXiv:1106.5736). Upper from parallelizing O(n²) folk algorithms via shared slice moves on O(n²/log n) commuting classes; lower from counting.
- **2026-11-12:** arXiv:2611.11018 provides explicit practical algorithm with 30.5 n² / lg n + o(n²/lg n) moves, within 2.37+o(1) of lower bound, linear time, O(√N lg N) space. First practical O(n²/lg n) solver. This repo's constructive path is in the same family and can adopt the tighter constants and parallel techniques.

## Pipeline

```text
ClusterScheduler
  -> BatchGroups
  -> Centers (never-break + orbit-BFS n<=5)
  -> StageCap(C) + leftover center commutators
  -> Edges (facelet map + flip-aware slice setup + expanded depth commutators + solid-set)
  -> StageCap(E) + leftover edge commutators
  -> Parity (even n)
  -> ReducedSearch (IDA* + residual MITM, n=4,5)
  -> 3x3 (Kociemba / CFOP / GodsAlgorithm architecture)
  -> BatchSolver::optimize
  -> BoundHarness
```

## 2026-11-12 edge step note

Reconfirmed asymptotic Θ(n²/log n) as the practical God's algorithm for any size >3. Exact g(n) open for n>=4. Community 4x4 OBTM upper 54 (53 claim unverified). Noted new explicit constant 30.5 n²/lg n from arXiv:2611.11018. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path, static tables, and explicit Demaine commuting-class parallel refinements to drive measured OBTM toward Ucas=288 and community 54. Constructive algorithm always terminates.

## 2026-11-11 edge step note

Reconfirmed asymptotic Θ(n²/log n) as the practical God's algorithm for any size >3. Exact g(n) open for n>=4. Community 4x4 OBTM upper 54 (53 claim unverified). leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path, static tables, and explicit Demaine commuting-class parallel refinements to drive measured OBTM toward Ucas=288 and community 54. Constructive algorithm always terminates.

## What is not claimed

God's number is the diameter of the Cayley graph. Proven only for n=2 (11 HTM) and n=3 (20 HTM / 26 QTM). |G(4)| is about 7.4e45. Community 4x4 OBTM window remains 35-54. Asymptotic result (Demaine et al.) is g(n) = Theta(n^2 / log n), which the batching stage follows in spirit. Explicit practical upper now available via arXiv:2611.11018.
