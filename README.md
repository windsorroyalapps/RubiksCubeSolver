# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
3×3 toward God's Number 20 · nxn reduction

## 3×3 path toward 20 moves

1. **MoveTables** – twist/flip/slice transitions for 18 moves  
2. **BFS pruning** – distance lower bounds per coordinate  
3. **Coord-space IDA*** – phase 1 searches integers, not facelets  
4. Phase 2 restricted moves → solved  
5. CFOP fallback if needed  

See [docs/GODS_NUMBER_PATH.md](docs/GODS_NUMBER_PATH.md).

## nxn
Centers → edges → parity (even) → Kociemba/CFOP

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
# Android Studio → Sync → Run
```

## Status

- [x] MoveTables + BFS pruning + coord IDA*
- [x] Phase-1 depth 14 (God's-regime search)
- [x] Full reduction + parity
- [ ] Dense full-index table generation
- [ ] Multi-probe phase-1 + optimal cleanup
- [ ] Production signed APK

---
*Android/BMW hacking style. Ship or die.*
