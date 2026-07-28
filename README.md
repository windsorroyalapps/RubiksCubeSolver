# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
From classic 3×3 CFOP all the way to theoretical 1000×1000+ reduction engine.

Built by an Android/BMW hacking genius who ships clean APKs and never leaves a broken production build on the table.

## Features

- **3×3**: Full facelet model + move engine + Singmaster notation
- **Arbitrary n**: Same engine works for 4×4, 5×5 … up to device memory limits (design target 1000×1000+)
- Material You UI + offline-first native engine
- Live demo: scramble / solve / reset / create larger cubes from the UI

## Project Structure

```
RubiksCubeSolver/
├── app/
│   ├── build.gradle.kts
│   └── src/main/
│       ├── java/.../
│       │   ├── MainActivity.kt      ← live demo UI
│       │   └── NativeSolver.kt      ← Kotlin ↔ C++ bridge
│       └── cpp/
│           ├── CMakeLists.txt
│           └── native-lib.cpp
├── native/
│   ├── common/
│   │   ├── Cube.h
│   │   └── Cube.cpp               ← full arbitrary-n move engine
│   ├── cfop/                     ← next: CFOP + Kociemba
│   └── reduction/                ← next: centers + edge pairing
├── docs/ALGORITHMS.md
└── … Gradle + license files
```

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
cd RubiksCubeSolver
# Open in Android Studio → Sync Gradle → Run on device/emulator
# or Build → Generate Signed Bundle / APK
```

## Status

- [x] Repo + full Android/NDK/Compose structure
- [x] Complete arbitrary-n facelet model + move engine (`Cube.cpp`)
- [x] JNI bridge + Kotlin wrapper (`NativeSolver`)
- [x] Live demo UI (scramble / solve / reset / 5×5)
- [ ] CFOP algorithm tables + Kociemba
- [ ] Reduction pipeline (centers + edge pairing)
- [ ] First production signed APK

Core engine is complete and callable from the app.  
Next we finish the actual solving algorithms and ship the production APK.

---
*Android/BMW hacking style. Ship or die.*
