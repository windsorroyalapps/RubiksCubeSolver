# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
3×3 CFOP + Kociemba · nxn Reduction (centers, edges, parity)

Built by an Android/BMW hacking genius who ships clean APKs and never leaves a broken production build on the table.

## Features

### 3×3
- Kociemba two-phase with **twist / flip / slice pruning tables**
- Admissible `max()` heuristic for faster IDA*
- CFOP fallback with pattern OLL/PLL

### n×n
- **Score-guided centers** (simulate commutators, pick best depth)
- **Skip-aware edge pairing** (don't re-pair finished dedges)
- **OLL + PLL parity** detection + standard algs for even order

See [docs/PRUNING_AND_PARITY.md](docs/PRUNING_AND_PARITY.md) for details.

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
cd RubiksCubeSolver
# Android Studio → Sync → Run
```

## Status

- [x] Pruning tables (twist/flip/slice) + admissible heuristic
- [x] Even-order OLL/PLL parity detection + algs
- [x] Tighter center score search + edge skip heuristics
- [x] Full reduction pipeline
- [ ] Offline full move-table BFS pruning DBs
- [ ] Production signed APK

---
*Android/BMW hacking style. Ship or die.*
