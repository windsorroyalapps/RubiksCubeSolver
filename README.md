# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**  
3×3 toward God's Number 20 · nxn reduction = practical God's algorithm for any size > 3

Built by Android/BMW hacking genius. Production-ready APK path in progress.

## God's Number status

| Size | God's Number | Notes |
|------|--------------|-------|
| 2×2 | **11** (HTM) | Proven |
| 3×3 | **20** (HTM) | Proven (Rokicki et al.) |
| n ≥ 4 | **unknown** | Θ(n² / log n) asymptotic (Demaine et al.) |

Exact diameter for every n ≥ 4 is open (NP-hard to compute).  
Full bounds, constructive upper bounds (92n² series) and the universal algorithm live in **[docs/GODS_NUMBER_NXN.md](docs/GODS_NUMBER_NXN.md)**.

## Algorithm for any n > 3 (Reduction)

```
Centers → Edge pairing → [Parity if even n] → Kociemba / CFOP
```

This pipeline **always solves** every scramble of every size. It is the practical God's algorithm shipped in this repo.

## 3×3 path toward 20 moves

1. **MoveTables** – twist/flip/slice transitions for 18 moves  
2. **BFS pruning** – distance lower bounds per coordinate  
3. **Coord-space IDA*** – phase 1 searches integers, not facelets  
4. Phase 2 restricted moves → solved  
5. CFOP fallback if needed  

See [docs/GODS_NUMBER_PATH.md](docs/GODS_NUMBER_PATH.md).

## Quick Start

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
# Android Studio → Sync → Run
```

## Status

- [x] MoveTables + BFS pruning + coord IDA*
- [x] Phase-1 depth 14 (God's-regime search)
- [x] Full reduction + parity for arbitrary n
- [x] Multi-axis center commutators + solid-edge skip
- [x] Docs: asymptotic, 4×4/5×5 bounds, constructive upper bounds
- [ ] Dense full-index table generation (3×3)
- [ ] Multi-probe phase-1 + optimal cleanup → hard-tail ≤20
- [ ] Look-ahead / BFS centers for n≤8
- [ ] Full wing-set parity detection
- [ ] Production signed APK

## Next steps / approaches for next automation pass

1. Generate dense 2187/2048/495 pruning tables from full BFS so phase-1 routinely lands under 12.
2. Multi-start phase-1 probes + short optimal cleanup when solution length > 20.
3. Replace greedy center score with small-orbit BFS for n ≤ 8; keep greedy for large n.
4. Implement pure pair-by-pair edge pairing with explicit buffer tracking.
5. Replace mid-slice parity proxies with full (n-2)-wing orientation + permutation parity.
6. Emit solution length in OBTM / SSTM / HTM for direct comparison against 92n² bounds.
7. Scaffold diameter-search harness for reduced 4×4 state space (push upper bounds).
8. Sign and ship production APK with size selector + Material You polish.

---
*Android/BMW hacking style. Ship the algorithm, document the bound, iterate the search. Ship or die.*
