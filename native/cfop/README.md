# CFOP + Kociemba Engine

## Modules

| File | Role |
|------|------|
| `CFOPSolver.*` | Beginner CFOP + pattern-aware 2-look OLL/PLL |
| `OLLTables.h` | Edge + corner OLL algorithms |
| `PLLTables.h` | Corner/edge PLL algorithms |
| `CoordCube.*` | twist / flip / slice coordinates from facelets |
| `Kociemba.*` | Two-phase IDA* (phase-1 → G1, phase-2 → solved) |

## Solve path (3x3)

1. **Kociemba phase 1** – IDA* on (twist, flip, slice) until G1  
2. **Kociemba phase 2** – IDA* with restricted move set to solved  
3. On failure / depth limit → **CFOP beginner** fallback  

## Next
- Precomputed move tables + pruning tables for real God's-algorithm performance
- Full 57 OLL / 21 PLL case IDs
