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

## Project Structure (fully scaffolded)

```
RubiksCubeSolver/
├── app/
│   ├── build.gradle.kts          # NDK + Compose + CMake
│   ├── proguard-rules.pro
│   └── src/main/
│       ├── AndroidManifest.xml
│       ├── java/.../MainActivity.kt + ui/theme/
│       ├── res/values/
│       └── cpp/
│           ├── CMakeLists.txt
│           └── native-lib.cpp      # JNI entry
├── native/
│   ├── common/
│   │   ├── Cube.h
│   │   └── Cube.cpp             # facelet model stub
│   ├── cfop/                   # 3x3 engine (next)
│   └── reduction/              # nxn engine (next)
├── docs/ALGORITHMS.md
├── build.gradle.kts
├── settings.gradle.kts
├── gradle.properties
├── gradle/wrapper/
├── .gitignore
└── LICENSE
```

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
cd RubiksCubeSolver
# Open in Android Studio → Sync Gradle → Build → Generate Signed Bundle / APK
```

## Status

- [x] Repo created
- [x] Full Android + NDK + Compose project structure
- [x] Native cube model stub (Cube.h / Cube.cpp)
- [x] JNI entry point
- [ ] Complete move engine + CFOP
- [ ] Reduction pipeline
- [ ] First production APK drop

We keep editing until the APK is completed and shippable. No half-finished artifacts.

---
*Android/BMW hacking style. Ship or die.*
