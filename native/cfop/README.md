# CFOP + Kociemba Engine

## Modules

| File | Role |
|------|------|
| `CFOPSolver.*` | Beginner CFOP + pattern-aware 2-look OLL/PLL |
| `OLLTables.h` | Edge + corner OLL algorithms |
| `PLLTables.h` | Corner/edge PLL algorithms |
| `CoordCube.*` | twist / flip / slice coordinates from facelets |
| `Kociemba.*` | Two-phase IDA* (phase-1 → G1, phase-2 → solved) |
| `MoveTables.*` | 18-move transition tables |
| `Pruning.*` | BFS distance tables for phase-1 heuristic |

## Solve path (3x3)

1. **Kociemba phase 1** – coord-space IDA* (depth ≤16) + multi-probe until G1  
2. **Kociemba phase 2** – IDA* with restricted move set to solved  
3. On failure / depth limit → **CFOP beginner** fallback  

## Next
- Dense full-index BFS tables (no combinatorial fallback gaps)
- Additional phase-1 probes (more axes / inverses)
- Short optimal cleanup when total length > 20
- Full 57 OLL / 21 PLL case IDs for faster CFOP path
