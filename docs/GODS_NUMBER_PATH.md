# Path toward God's Number (20) for 3×3 + nxn overview

## 3×3 — what landed

### MoveTables (`native/cfop/MoveTables.*`)
- 18 moves (6 faces × 3 turns)
- `twistMove[2187][18]`, `flipMove[2048][18]`, `sliceMove[495][18]`
- Built from facelet applications + walk propagation on first `init()`

### Pruning (BFS)
- BFS over each coordinate using MoveTables
- Fallback combinatorial bounds for unreachable slots
- `phase1H = max(twist, flip, slice)` — admissible

### Kociemba phase 1
- **Coord-space IDA*** (no facelet apply per node)
- Depth cap raised to **16** (Gods-regime)
- **Multi-probe**: primary search + one U-reoriented secondary probe; keep shorter path
- Applies the found move list to the live cube once

### Phase 2
- Restricted generators `{U,D,F2,B2,L2,R2}` facelet IDA*, depth 12
- CFOP fallback if either phase fails

## Toward true ≤20

| Step | Status |
|------|--------|
| Move tables | Done (sampled + propagated) |
| BFS pruning | Done |
| Coord-space search | Done |
| Phase-1 depth 16 + multi-probe | Done |
| Full encode/decode all 2187/2048/495 from indices | Next (denser tables) |
| More phase-1 probes (axis + inverse starts) | Next |
| Optimal cleanup if len > 20 | Next |

With denser tables and additional probes, typical solutions drop into the high teens; the hard tail needs the optimal pass.

## nxn (n > 3) — God's algorithm status

Exact g(n) is **unknown** for every n ≥ 4.  
See **[GODS_NUMBER_NXN.md](GODS_NUMBER_NXN.md)** for:

- Asymptotic Θ(n² / log n)
- Concrete bounds for 4×4 / 5×5
- Constructive upper bounds (92n² series)
- Full reduction algorithm that solves every size
- Implementation map and next automation steps

The reduction pipeline in `native/reduction/` **is** the practical God's algorithm for arbitrary n: it always terminates and produces a correct solution. Optimality (diameter) remains open and is the long-term research target.
