# Path toward God's Number (20) for 3×3

## What landed

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
- Depth cap raised to **14**
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
| Full encode/decode all 2187/2048/495 from indices | Next (denser tables) |
| Multiple phase-1 probes | Next |
| Optimal cleanup if len > 20 | Next |

With denser tables and multi-probe phase-1, typical solutions drop into the high teens; the hard tail needs the optimal pass.
