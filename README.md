# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
From classic 3×3 CFOP all the way to theoretical 1000×1000+ reduction engine.

Built by an Android/BMW hacking genius who ships clean APKs and never leaves a broken production build on the table.

## Features

- **3×3**
  - Full facelet model + arbitrary-depth move engine
  - Beginner CFOP pipeline (Cross → corners → middle → LL)
  - **Pattern-aware 2-look OLL** (dot / L / line detection)
  - **Pattern-aware corner OLL** (Sune / Pi / Headlights)
  - PLL tables (A-perm, Ua/Ub, H, T, Y, J …)
  - **Kociemba two-phase skeleton** (falls back to CFOP until pruning tables land)
- **n×n**: ReductionSolver skeleton
- Material You UI + live Solve button

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
cd RubiksCubeSolver
# Android Studio → Sync → Run
```

## Status

- [x] Android + NDK + Compose
- [x] Arbitrary-n move engine + edge/corner helpers
- [x] Full beginner CFOP path
- [x] Pattern recognition for yellow cross + corner OLL
- [x] OLL / PLL algorithm tables
- [x] Kociemba two-phase skeleton (wired as preferred solver)
- [ ] Full pruning tables + IDA* for Kociemba
- [ ] Complete 57 OLL + 21 PLL recognition
- [ ] Real big-cube centers & edge pairing
- [ ] Production signed APK

---
*Android/BMW hacking style. Ship or die.*
