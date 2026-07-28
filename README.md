# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
3×3 CFOP + Kociemba two-phase, extensible to nxn reduction.

Built by an Android/BMW hacking genius who ships clean APKs and never leaves a broken production build on the table.

## Features

- **3×3**
  - Facelet model + full move engine
  - Beginner CFOP (Cross → LL) with **pattern-aware OLL/PLL**
  - **Kociemba two-phase** with real coordinates:
    - `twist` / `flip` / `slice` extracted from facelets
    - Phase-1 IDA* → G1 subgroup
    - Phase-2 IDA* → solved (restricted moves)
    - Automatic CFOP fallback if depth limit hit
- **n×n**: ReductionSolver skeleton
- Material You UI + Solve button wired to native engine

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
cd RubiksCubeSolver
# Android Studio → Sync Gradle → Run
```

## Status

- [x] Android + NDK + Compose project
- [x] Arbitrary-n move engine
- [x] Full beginner CFOP + pattern OLL/PLL
- [x] CoordCube (twist / flip / slice)
- [x] Kociemba phase-1 & phase-2 IDA* search
- [x] JNI solve path: Kociemba → CFOP fallback
- [ ] Precomputed pruning / move tables (speed)
- [ ] Full 57 OLL + 21 PLL IDs
- [ ] Big-cube centers & edge pairing
- [ ] Production signed APK

---
*Android/BMW hacking style. Ship or die.*
