# Reduction Engine (nxn)

Reduces any n×n×n (n ≥ 4) to a 3×3, then solves with Kociemba/CFOP.

## Pipeline

1. **CenterSolver** – gather center pieces per face using commutators + slice moves
2. **EdgePairing** – freeslice-style wing pairing into 12 solid dedges
3. **3×3 stage** – Kociemba (with CFOP fallback) on the reduced cube

## Files

| File | Role |
|------|------|
| `CenterSolver.*` | Center commutators for arbitrary n |
| `EdgePairing.*` | Freeslice edge pairing |
| `ReductionSolver.*` | Orchestrator |

## Notes

- Works for 4×4, 5×5, … up to memory limits on device
- Design target includes theoretical 1000×1000 (software-only)
- Parity cases on even-order cubes handled in a later pass
