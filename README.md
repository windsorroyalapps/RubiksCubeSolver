# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
3×3 CFOP + Kociemba · nxn Reduction with centers, edges, and parity

Built by an Android/BMW hacking genius who ships clean APKs and never leaves a broken production build on the table.

## Features

### 3×3
- Facelet model + move engine
- CFOP with pattern-aware OLL/PLL
- Kociemba two-phase (coordinates + IDA*)
- **Pruning tables** for twist/flip heuristics

### n×n (4×4+)
- CenterSolver (commutators)
- EdgePairing (freeslice)
- **ParityHandler** – OLL + PLL parity algs for even-order cubes
- Reduction → Kociemba/CFOP

### App
- Material You UI, scramble / solve / reset / 5×5

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
cd RubiksCubeSolver
# Android Studio → Sync → Run
```

## Status

- [x] Android + NDK + Compose
- [x] Move engine + CFOP + Kociemba IDA*
- [x] CoordCube + pruning heuristics
- [x] CenterSolver + EdgePairing
- [x] Even-order OLL/PLL parity handlers
- [x] Full reduction pipeline
- [ ] Full offline-generated pruning DBs (speed)
- [ ] Production signed APK

---
*Android/BMW hacking style. Ship or die.*
