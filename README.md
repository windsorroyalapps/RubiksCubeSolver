# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
From classic 3×3 CFOP all the way to theoretical 1000×1000+ reduction engine.

Built by an Android/BMW hacking genius who ships clean APKs and never leaves a broken production build on the table.

## Features

- **3×3**: Full CFOP (Cross → F2L → OLL → PLL) + Kociemba two-phase optimal solver
- **4×4 → n×n**: Reduction method (Centers → Edge Pairing → 3×3 stage)
- Asymptotically optimal **O(n² / log n)** path for huge cubes (Demaine et al. 2011)
- Material You UI + offline-first
- Continuous APK production pipeline ready

## Repo Structure

```
app/                  # Android app (Kotlin + Jetpack Compose)
native/               # C++ solver engine (JNI)
  cfop/               # 3x3 CFOP + Kociemba
  reduction/          # nxn centers + edge pairing
  common/             # Cube model, moves, notation
docs/                 # Algorithm notes & God's Number references
.gitignore
LICENSE
```

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
cd RubiksCubeSolver
# Open in Android Studio → Build → Generate Signed Bundle / APK
```

## Status

- [x] Repo created & scaffolded
- [ ] Native C++ cube model
- [ ] CFOP implementation
- [ ] Reduction pipeline
- [ ] First production APK drop

We edit until the APK is completed and shippable. No half-finished artifacts.

---
*Android/BMW hacking style. Ship or die.*
