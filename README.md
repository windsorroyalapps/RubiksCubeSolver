# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
3×3 CFOP + Kociemba two-phase · nxn Reduction (centers → edges → 3×3)

Built by an Android/BMW hacking genius who ships clean APKs and never leaves a broken production build on the table.

## Features

### 3×3
- Facelet model + full move engine
- Beginner CFOP with pattern-aware OLL/PLL
- **Kociemba two-phase** (twist/flip/slice coordinates + IDA*)
- Automatic CFOP fallback

### n×n (4×4+)
- **CenterSolver** – commutator-based center gathering
- **EdgePairing** – freeslice wing pairing
- **ReductionSolver** – full pipeline then Kociemba/CFOP

### App
- Material You UI
- Scramble / Solve / Reset / Create 5×5
- Native engine via JNI

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
cd RubiksCubeSolver
# Android Studio → Sync → Run
```

## Status

- [x] Android + NDK + Compose
- [x] Arbitrary-n move engine
- [x] CFOP + pattern OLL/PLL
- [x] Kociemba coordinates + IDA*
- [x] CenterSolver (nxn)
- [x] EdgePairing (nxn)
- [x] Reduction pipeline wired end-to-end
- [ ] Pruning tables for faster Kociemba
- [ ] Even-order parity handlers
- [ ] Production signed APK

---
*Android/BMW hacking style. Ship or die.*
