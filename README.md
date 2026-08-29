# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**

- **3×3:** God's algorithm path toward **≤ 20 HTM** (proven God's Number)
- **n×n (n ≥ 4):** Universal constructive algorithm — reduction + Demaine batching + residual MITM. Always terminates. Exact integer g(n) is **open**; asymptotic **g(n)=Θ(n²/log n)**.

Exact g(n) is proven only for n=2,3. This repo implements the algorithm for every larger size plus live L(n)/U(n)/OBTM instrumentation — not a fake closed diameter.

Canonical contract: [docs/UNIVERSAL_NXN_ALGORITHM.md](docs/UNIVERSAL_NXN_ALGORITHM.md)

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

Bounds table only (no full solver):

```bash
g++ -O2 -std=c++17 -Inative/common -Inative/reduction \
  native/tools/print_bounds.cpp native/reduction/BoundHarness.cpp \
  -o artifacts/print_bounds
./artifacts/print_bounds 20
```

---

## 3×3 path

```text
Multi-probe Kociemba → if len > 20: optimal IDA* (≤20) → CFOP fallback
```

→ [docs/GODS_NUMBER_PATH.md](docs/GODS_NUMBER_PATH.md)

---

## n×n path (algorithm for any n > 3)

```text
ClusterScheduler → BatchGroups → Centers → Edges → Parity (even n)
  → ReducedSearch (IDA* + residualKey MITM on 4x4/5x5) → 3×3 → BatchSolver
  → BoundHarness (Hardwick |G| L(n) + U(n) + SSTM + OBTM + per-stage OBTM)
  → Cube::movesToNotation / applyNotation SiGN replay
```

Exact diameter open for n≥4. Constructive algorithm always terminates.

→ [docs/GODS_NUMBER_NXN.md](docs/GODS_NUMBER_NXN.md) · [docs/GROUP_ORDER.md](docs/GROUP_ORDER.md)

---

## Bound harness — work backward from U(n) toward L(n)

| n | L(n) ≥ | Constructive U(n) | Community OBTM upper |
|---|--------|-------------------|----------------------|
| 3 | **20** | **20** | **20** (proven) |
| 4 | 35 | **501** | **54** |
| 5 | 40 | **878** | ~130 claimed |
| 6 | Hardwick count | **1321** | open |
| 7 | Hardwick count | **1852** | open |
| 8 | Hardwick count | **2473** | open |
| 9 | Hardwick count | **3182** | open |
| 10 | Hardwick count | **3981** | open |

4×4 probabilistic estimates ~41 HTM / ~48 QTM.

L(n) for n≥4 is `floor(ln|G|/ln|S|)` from Hardwick's exact |G(n)|, lifted to community 35/40 on 4×4/5×5. Still a *counting* lower bound, not a proven diameter.

```kotlin
NativeSolver.create(5)
val sol = NativeSolver.solve()
val report = NativeSolver.boundReport()
NativeSolver.setMitmBudget(4, 150000, 28)
```

---

## Status (2026-08-30)

- [x] GodsAlgorithm + Kociemba IDA* (3×3)
- [x] nxn reduction + parity for any n≥4
- [x] ClusterScheduler + BatchGroups + BatchSolver
- [x] BoundHarness U(n) + OBTM/SSTM + per-stage OBTM
- [x] BoundHarness L(n) counting lower + generatorCount + community OBTM 54
- [x] **Hardwick exact |G(n)| in log-space** (replaces 1.5 n² bit estimate)
- [x] `native/tools/print_bounds.cpp` + [docs/GROUP_ORDER.md](docs/GROUP_ORDER.md)
- [x] JNI MITM budgets + env overrides
- [x] ReducedSearch.cpp real IDA* + MITM
- [x] docs/UNIVERSAL_NXN_ALGORITHM.md contract
- [x] native/tools/desktop_harness.cpp
- [x] Cube::applyNotation SiGN for n>3 + ReductionSolver uses movesToNotation
- [ ] Perfect offline 3×3 pruning DBs
- [ ] Production signed APK + verified native .so
- [ ] Adaptive launcher icons
- [ ] Measure replaySolved rate on 4×4/5×5 after desktop compile

---

## Next steps / approaches to try next time (2026-08-30 post-Hardwick L(n))

1. **Highest leverage:** compile + run `print_bounds 20` then `desktop_harness` on 4×4/5×5 with raised `RCS_MITM_*`. Confirm log10|G| matches OEIS (n=4 ≈ 45.87, n=5 ≈ 74.45). Log MITM hit rate, OBTM vs U(n) vs L(n) vs community 54, and **replaySolved=yes rate**.
2. If replaySolved is low, debug SiGN encode/decode round-trip on ReductionSolver output (single encoder).
3. Per-stage OBTM targeting: cut the fattest phase only (centers vs edges vs parity).
4. Optional: use *fixed-orientation* |G| (A054434, ×24 on even n) if you want L(n) in the face-fixed metric the 3×3 proof used.
5. Verify green CI APK contains lib*.so.
6. 3×3 dense full-index pruning DBs toward the proven 20 ceiling.
7. Production signed APK + Material You + size selector to 20×20.
8. Recalibrate BoundHarness 3.8 scale if new community numbers appear; keep U(n) as hard constructive guarantee.
9. Center BFS node-budget tuning on desktop.
10. Edge pairing quality metrics (`pairedWings`) into BoundHarness.
11. Shorter parity algs that still clear full-depth detectors.
12. Optional full 24-wing Lehmer if 5×5+ residual stays deep.
13. Denser 5×5 center residual under higher MITM budgets.
14. Do not claim exact g(n) for n≥4 until a published diameter proof exists.
15. If a cluster appears: 4×4 coset / IDA* diameter attack is the only path to an *integer* g(4); phone solvers cannot close it.

---

*Exact g(n) for n≥4 remains open. Constructive reduction + Demaine batching + residual MITM is the universal algorithm this repo ships. Hardwick |G(n)| L(n) landed 2026-08-30.*
