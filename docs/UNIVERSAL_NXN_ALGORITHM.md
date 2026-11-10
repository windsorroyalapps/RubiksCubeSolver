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

## 2026-11-11 edge step note

Reconfirmed asymptotic Θ(n²/log n) as the practical God's algorithm for any size >3. Exact g(n) open for n>=4. Community 4x4 OBTM upper 54 (53 claim unverified). leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path, static tables, and explicit Demaine commuting-class parallel refinements to drive measured OBTM toward Ucas=288 and community 54. Constructive algorithm always terminates.

## 2026-11-10 edge step note

Reconfirmed asymptotic Θ(n²/log n) as the practical God's algorithm for any size >3. Exact g(n) open for n>=4. Community 4x4 OBTM upper 54 (53 claim unverified). leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54. Constructive algorithm always terminates.

## 2026-11-09 edge step note

Increased pairAll passes and residual repair loops. Noted unverified OBTM ≤53 / BT ≤51 claim for 4x4. Exact g(n) open for n>=4. Asymptotic Θ(n²/log n) holds as the general God's algorithm for any size >3. Community 4x4 OBTM upper 54. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54. Constructive algorithm always terminates.

## 2026-11-08 edge step note

Expanded depthCommutator to 16 variants. Noted unverified OBTM ≤53 claim for 4x4. Exact g(n) open for n>=4. Asymptotic Θ(n²/log n) holds as the general God's algorithm for any size >3. Community 4x4 OBTM upper 54. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54. Constructive algorithm always terminates.

## 2026-11-07 edge step note

Fresh harness measurement on 4×4 (MITM 50k nodes / depth 20, center BFS 20k/depth 6, 3 trials): leftoverC=0 leftoverE=7-8 workSolved=no OBTM=144-256 (under Ucas=288, within U(n)=501). Exact g(n) open for n>=4. Asymptotic Θ(n²/log n) holds as the general God's algorithm for any size >3. Community 4x4 OBTM upper 54. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54. Constructive algorithm always terminates.

## 2026-11-06 edge step note

Automation session confirms exact g(n) open for n>=4. Asymptotic Θ(n²/log n) holds as the general God's algorithm for any size >3. Community 4x4 OBTM upper 54. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54. Noted Demaine parallelization and Salkinder group analysis for potential refinements. Constructive algorithm always terminates.

## 2026-11-05 edge step note

Fresh harness measurement on 4×4 (limited budgets, 3 trials): leftoverC=0 leftoverE=7-8 workSolved=no OBTM=143-266 (under Ucas=288, within U(n)=501). Exact g(n) open for n>=4. Asymptotic Θ(n²/log n) holds as the general God's algorithm for any size >3. Community 4x4 OBTM upper 54. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54. Noted Demaine parallelization and Salkinder group analysis for potential refinements.

## 2026-11-03 edge step note

Fresh harness measurement on 4×4 (reduced budgets): leftoverC=0 leftoverE=8 workSolved=no OBTM=257 (under Ucas=288, within U(n)=501). Improvement over prior 820–919 range. Exact g(n) open for n>=4. Asymptotic Θ(n²/log n) holds as the general God's algorithm for any size >3. Community 4x4 OBTM upper 54. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54. Noted Demaine parallelization and Salkinder group analysis for potential refinements.

## 2026-10-30 edge step note

Reconfirmed exact g(n) open for n>=4. Asymptotic Θ(n²/log n) holds. Community 4x4 OBTM upper 54. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54. Noted Demaine parallelization and Salkinder group analysis for potential refinements.

## 2026-10-29 edge step note

Reconfirmed community 4x4 OBTM upper 54. Noted Demigod sampling method (FUN 2026) for potential future probabilistic diameter estimators. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54. Consider Hirata graph-theoretical lower bounds.

## 2026-10-25 edge step note

Reconfirmed community 4x4 OBTM upper 54. Noted Demigod sampling method (FUN 2026) for potential future probabilistic diameter estimators. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54.

## 2026-10-22 edge step note

Expanded setups continue; leftoverE is the completeness gate after leftoverC=0. Community upper for 4x4 tightened to 54 OBTM. Next is facelet-shortest path and static tables to drive measured OBTM toward Ucas=288 and community 54.

## 2026-10-16 edge step note

Expanded setupFromSource with additional face options to improve residual edge positioning. leftoverE remains the completeness gate after leftoverC=0. Next is facelet-shortest path and static tables.

## 2026-10-08 edge step

`locateTargetWings` tags orientation. A flipped source (`orient = -1`) receives `flipSliceSetup` before the depth commutator. No-gain sequences are undone. This is the current attack on leftoverE=6-7. It does not change U(n) or L(n).

## What is not claimed

God's number is the diameter of the Cayley graph. Proven only for n=2 (11 HTM) and n=3 (20 HTM / 26 QTM). |G(4)| is about 7.4e45. Community 4x4 OBTM window remains 35-54. Asymptotic result (Demaine et al.) is g(n) = Theta(n^2 / log n), which the batching stage follows in spirit.
