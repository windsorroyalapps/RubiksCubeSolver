# Demaine bounds → BatchSolver in this repo

## Theory (Demaine et al. 2011)

**Lower bound**  
\( |\text{configs}| = 2^{\Theta(n^2)} \), branching \( O(n) \)  
\( \Rightarrow \) diameter \( \Omega(n^2 / \log n) \).

**Upper bound**  
Naive: \( O(1) \) moves per cluster \( \times \Theta(n^2) \) clusters \( = O(n^2) \).  
Parallel: one slice move updates many cubies; batch shared needs  
\( \Rightarrow O(n^2 / \log n) \).

## Implementation: `BatchSolver`

| Step | What |
|------|------|
| `compress` | Merge consecutive same (face, depth) turns; drop 0-mod-4 |
| `batchWindow` | In each window of size W, keep **first** of each identical move (collapse duplicates = shared work) |
| `optimize` | compress → windows 4,8,16,32 → compress again |

Wired at the end of `ReductionSolver::solve` so nxn solutions are post-processed.

Helpers:
- `estimatedClusters(n)` ≈ \( \Theta(n^2) \) work units
- `asymptoticTarget(n)` ≈ \( 4 \, n^2 / \ln n \) (shape only)

## Honest note

Window batching is a **practical approximation** of Demaine parallelism, not a full proof-carrying construction. It shortens repeated identical slice requests that naive center/edge loops emit. True \( O(n^2/\log n) \) needs careful scheduling of cluster sequences so batching never breaks correctness; we still rely on reduction stages being mostly correct before optimize.

## Pipeline

```text
Centers → Edges → Parity (even) → 3x3 → BatchSolver::optimize
```
