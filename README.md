# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
**God's algorithm target: ≤ 20 HTM moves for every 3×3** · nxn reduction

## 3×3 — God's Number path

```text
Multi-probe Kociemba  →  if len > 20: optimal IDA* (≤20)  →  CFOP fallback
```

- MoveTables + BFS pruning + coord-space phase-1  
- **GodsAlgorithm**: 12 pre-move probes, keep shortest, cleanup at God's Number  
- App **Solve** button calls this path  

Details: [docs/GODS_NUMBER_PATH.md](docs/GODS_NUMBER_PATH.md)

## nxn
Centers → edges → parity (even) → 3×3 stage

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
# Android Studio → Sync → Run → Scramble → Solve
```

## Status

- [x] GodsAlgorithm (multi-probe + ≤20 cleanup) wired to JNI
- [x] Dense MoveTables + BFS pruning + coord IDA*
- [x] Full nxn reduction + parity
- [ ] Perfect offline tables (hard guarantee of 20 on every scramble on-device)
- [ ] Production signed APK

---
*Android/BMW hacking style. Ship or die.*
