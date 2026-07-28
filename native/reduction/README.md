# Reduction Engine (nxn) — Practical God's Algorithm

Reduces any n×n×n (n ≥ 4) to a 3×3, then solves with Kociemba/CFOP.
This is the constructive algorithm that works for **every** size; exact diameter remains open.

## Pipeline

1. **CenterSolver** – gather center pieces per face using multi-axis commutators + slice moves
2. **EdgePairing** – freeslice-style wing pairing into 12 solid dedges (skips already-paired)
3. **ParityHandler** (even n) – OLL parity then PLL parity
4. **3×3 stage** – Kociemba (with CFOP fallback) on the reduced cube

## Files

| File | Role |
|------|------|
| `CenterSolver.*` | Multi-axis center commutators for arbitrary n |
| `EdgePairing.*` | Freeslice edge pairing with solid-edge skip |
| `ParityHandler.*` | Even-n OLL/PLL parity detect + fix |
| `ReductionSolver.*` | Orchestrator |

## Notes

- Works for 4×4, 5×5, … up to memory limits on device
- Design target includes theoretical 1000×1000 (software-only)
- Exact God's number g(n) = Θ(n² / log n); see `docs/GODS_NUMBER_NXN.md`

## Next approaches

- Look-ahead / small BFS for centers on n ≤ 8
- Pure pair-by-pair + buffer tracking for edges
- Full wing-set parity instead of mid-slice proxy
- Move-count emission in OBTM / SSTM for bound comparison
