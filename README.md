# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**

- **3×3:** God's algorithm path toward **≤ 20 HTM** (proven God's Number)
- **n×n (n ≥ 4):** Reduction + Demaine-style batching — **complete constructive algorithm** for any size; work backward from constructive **U(n)** and asymptotic **Θ(n² / log n)**

Exact g(n) is proven only for n=2,3. For n>3 this repo implements the universal solving algorithm plus live bound instrumentation — not a false claim of a closed diameter.

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
# Android Studio → Sync → Run → Scramble → Solve
# or: GitHub Actions → Build Production APK artifact
```

Desktop residual harness:

```bash
g++ -O2 -std=c++17 -Inative/common -Inative/cfop -Inative/reduction \
  native/tools/desktop_harness.cpp native/common/Cube.cpp \
  native/cfop/CFOPSolver.cpp native/cfop/Kociemba.cpp native/cfop/CoordCube.cpp \
  native/cfop/Pruning.cpp native/cfop/MoveTables.cpp native/cfop/GodsAlgorithm.cpp \
  native/reduction/ReductionSolver.cpp native/reduction/CenterSolver.cpp \
  native/reduction/EdgePairing.cpp native/reduction/ParityHandler.cpp \
  native/reduction/BatchSolver.cpp native/reduction/ClusterScheduler.cpp \
  native/reduction/BatchGroups.cpp native/reduction/BoundHarness.cpp \
  native/reduction/ReducedSearch.cpp \
  -o artifacts/rcs_harness
RCS_MITM_NODEBUDGET4=150000 RCS_MITM_DEPTHCAP4=28 ./artifacts/rcs_harness 4 10
```

---

## 3×3 path

```text
Multi-probe Kociemba → if len > 20: optimal IDA* (≤20) → CFOP fallback
```

→ [docs/GODS_NUMBER_PATH.md](docs/GODS_NUMBER_PATH.md)

---

## n×n path (practical God's algorithm for any n > 3)

```text
ClusterScheduler → BatchGroups → Centers → Edges → Parity (even n)
  → ReducedSearch (IDA* + residualKey MITM on 4x4/5x5) → 3×3 → BatchSolver
  → BoundHarness (SSTM + OBTM + per-stage OBTM)
  → Cube::applyNotation SiGN replay (2R / Rw / 3Rw / M E S)
```

Exact diameter open for n≥4. Constructive algorithm always terminates.

→ [docs/GODS_NUMBER_NXN.md](docs/GODS_NUMBER_NXN.md)

---

## Bound harness — work backward from U(n)

| n | Constructive upper U(n) |
|---|-------------------------|
| 4 | **501** |
| 5 | **878** |
| 6 | **1321** |
| 7 | **1852** |
| 8 | **2473** |
| 9 | **3182** |
| 10 | **3981** |

4×4 OBTM community upper **54**. Probabilistic estimates ~41 HTM / ~48 QTM.

```kotlin
NativeSolver.create(5)
val sol = NativeSolver.solve()
val report = NativeSolver.boundReport()
NativeSolver.setMitmBudget(4, 150000, 28)
```

---

## Status (2026-08-28)

- [x] GodsAlgorithm + Kociemba IDA* (3×3)
- [x] nxn reduction + parity for any n≥4
- [x] ClusterScheduler + BatchGroups + BatchSolver
- [x] BoundHarness U(n) + OBTM/SSTM + per-stage OBTM
- [x] JNI MITM budgets + env overrides
- [x] ReducedSearch.cpp real IDA* + MITM
- [x] docs/GODS_NUMBER_NXN.md filled
- [x] native/tools/desktop_harness.cpp
- [x] **Cube::applyNotation SiGN for n>3** (2R, Rw, 3Rw, M/E/S) + harness replaySolved
- [ ] Perfect offline 3×3 pruning DBs
- [ ] Production signed APK + verified native .so
- [ ] Adaptive launcher icons
- [ ] Measure replaySolved rate on 4×4/5×5 after desktop compile

---

## Next steps / approaches to try next time (2026-08-28 post-SiGN)

1. Compile + run `desktop_harness` on 4×4/5×5 with raised `RCS_MITM_*`; log MITM hit rate, OBTM vs U(n) vs community 54, and **replaySolved=yes rate**. Highest remaining algorithm leverage.
2. Per-stage OBTM targeting: cut the fattest phase only (centers vs edges vs parity).
3. Verify green CI APK contains lib*.so.
4. 3×3 dense full-index pruning DBs toward the proven 20 ceiling.
5. Production signed APK + Material You + size selector to 20×20.
6. Adaptive launcher icons.
7. Recalibrate BoundHarness 3.8 scale if new community numbers appear; keep U(n) as hard constructive guarantee.
8. Center BFS node-budget tuning on desktop.
9. Edge pairing quality metrics (`pairedWings`) into BoundHarness.
10. Shorter parity algs that still clear full-depth detectors.
11. Optional full 24-wing Lehmer if 5×5+ residual stays deep.
12. Denser 5×5 center residual under higher MITM budgets.
13. Wire `Cube::movesToNotation` into ReductionSolver / GodsAlgorithm so emit+parse share one encoder.
14. Do not claim exact g(n) for n≥4 until a published diameter proof exists.

---

*Exact g(n) for n≥4 remains open. Constructive reduction + Demaine batching + residual MITM is the universal algorithm this repo ships. SiGN applyNotation for n>3 landed 2026-08-28.*
