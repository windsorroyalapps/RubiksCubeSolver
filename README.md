# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
From classic 3×3 CFOP all the way to theoretical 1000×1000+ reduction engine.

Built by an Android/BMW hacking genius who ships clean APKs and never leaves a broken production build on the table.

## Features

- **3×3**: Facelet model + full move engine + CFOP solver skeleton (last-layer algs live)
- **n×n**: ReductionSolver skeleton (centers → edges → 3×3) ready for expansion
- Material You UI with live scramble / solve / reset / 5×5 demo
- JNI + Kotlin wrapper fully wired

## Project Structure

```
native/
  common/     Cube.h / Cube.cpp          ← complete arbitrary-n engine
  cfop/       CFOPSolver.h / .cpp        ← Cross/F2L/OLL/PLL pipeline
  reduction/  ReductionSolver.h / .cpp   ← nxn reduction pipeline
app/          Compose UI + NativeSolver + JNI
```

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
cd RubiksCubeSolver
# Open in Android Studio → Sync → Run
```

## Status

- [x] Full Android + NDK + Compose project
- [x] Arbitrary-n move engine (Cube.cpp)
- [x] JNI + Kotlin bridge
- [x] Live demo UI (scramble / solve / reset / 5×5)
- [x] CFOPSolver skeleton with working last-layer algorithms
- [x] ReductionSolver skeleton calling CFOP for final stage
- [ ] Full piece-search Cross / F2L / complete OLL-PLL tables
- [ ] Real center & edge-pairing logic for big cubes
- [ ] First production signed APK

The app now has a real **Solve** button that calls into the native CFOP/Reduction engines.

---
*Android/BMW hacking style. Ship or die.*
