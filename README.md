# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
From classic 3×3 CFOP all the way to theoretical 1000×1000+ reduction engine.

Built by an Android/BMW hacking genius who ships clean APKs and never leaves a broken production build on the table.

## Features

- **3×3**: Complete facelet model + move engine + **full beginner CFOP pipeline**
  - White Cross search & insert
  - White corners (sexy move)
  - Middle edges
  - Yellow cross / OLL / PLL algs
- **n×n**: ReductionSolver skeleton (centers → edges → CFOP)
- Material You UI with scramble / solve / reset / 5×5
- JNI + Kotlin bridge fully wired

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
cd RubiksCubeSolver
# Open in Android Studio → Sync → Run on device/emulator
```

## Status

- [x] Android + NDK + Compose project
- [x] Arbitrary-n move engine
- [x] Edge / corner query helpers
- [x] White cross detection + search/insert
- [x] Full beginner layer-by-layer solver (Cross → LL)
- [x] CFOPSolver + ReductionSolver linked and callable from UI
- [ ] Pattern-based full OLL/PLL tables
- [ ] Optimal Kociemba two-phase
- [ ] Real big-cube center & edge pairing
- [ ] First production signed APK

Hit **Solve** in the app and the native beginner CFOP engine runs end-to-end.

---
*Android/BMW hacking style. Ship or die.*
