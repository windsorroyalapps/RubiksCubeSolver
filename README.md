# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**

- **3×3:** God's algorithm path toward **≤ 20 HTM** (proven God's Number)
- **n×n (n ≥ 4):** Universal constructive algorithm — reduction + Demaine batching + residual MITM. Always terminates. Exact integer g(n) is **open**; asymptotic **g(n)=Θ(n²/log n)**.

Exact g(n) is proven only for n=2,3. This repo implements the algorithm for every larger size plus live L(n)/L_fixed(n)/U(n)/OBTM instrumentation — not a fake closed diameter.

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
  → BoundHarness (Hardwick |G| L(n) + L_fixed + U(n) + gap + SSTM + OBTM)
  → Cube::movesToNotation / applyNotation SiGN replay
```

Exact diameter open for n≥4. Constructive algorithm always terminates.

→ [docs/GODS_NUMBER_NXN.md](docs/GODS_NUMBER_NXN.md) · [docs/GROUP_ORDER.md](docs/GROUP_ORDER.md)

---

## Bound harness — work backward from U(n) toward L(n)

Verified by compiling `print_bounds` (2026-08-31, OEIS + U(n) lock):

| n | L(n) ≥ | L_fixed | log10\|G\| | Constructive U(n) | gap U−L | Community OBTM upper |
|---|--------|---------|------------|-------------------|---------|----------------------|
| 3 | **20** | **20** | 19.64 | **20** | 0 | **20** (proven) |
| 4 | 35 | 32 | 45.87 | **501** | 466 | **54** |
| 5 | **47** | 47 | 74.45 | **878** | 831 | ~130 claimed |
| 6 | 67 | 66 | 116.20 | **1727** | 1660 | open |
| 7 | 92 | 92 | 160.29 | **2472** | 2380 | open |
| 8 | 117 | 116 | 217.55 | **3689** | 3572 | open |
| 9 | 149 | 149 | 277.15 | **4802** | 4653 | open |
| 10 | 179 | 178 | 349.92 | **6387** | 6208 | open |

4×4 probabilistic estimates ~41 HTM / ~48 QTM.

L(n) for n≥4 is `floor(ln|G|/ln|S|)` from Hardwick's exact |G(n)|, lifted to community 35 on 4×4. L_fixed quotients even-n |G| by 24. Still counting lowers, not proven diameters. U(n) is the hard constructive guarantee.

```kotlin
NativeSolver.create(5)
val sol = NativeSolver.solve()
val report = NativeSolver.boundReport()
NativeSolver.setMitmBudget(4, 150000, 28)
```

---

## Status (2026-08-31)

- [x] GodsAlgorithm + Kociemba IDA* (3×3)
- [x] nxn reduction + parity for any n≥4
- [x] ClusterScheduler + BatchGroups + BatchSolver
- [x] BoundHarness U(n) + OBTM/SSTM + per-stage OBTM
- [x] BoundHarness L(n) counting lower + generatorCount + community OBTM 54
- [x] **Hardwick exact |G(n)| in log-space** (replaces 1.5 n² bit estimate)
- [x] `native/tools/print_bounds.cpp` + [docs/GROUP_ORDER.md](docs/GROUP_ORDER.md)
- [x] U(n) table n≥6 corrected to the formula (1727 / 2472 / 3689 / 4802 / 6387)
- [x] JNI MITM budgets + env overrides
- [x] ReducedSearch.cpp real IDA* + MITM
- [x] docs/UNIVERSAL_NXN_ALGORITHM.md contract
- [x] native/tools/desktop_harness.cpp
- [x] Cube::applyNotation SiGN for n>3 + ReductionSolver uses movesToNotation
- [x] **L_fixed(n)** face-fixed counting (even n: |G|/24, A054434 spirit)
- [x] **gap = U(n)−L(n)** on BoundReport + print_bounds
- [x] **OEIS / U(n) sanity lock** (`oeisSanityFailN`) so n=2..5 log10|G| and U(6/7/10) cannot silently regress
- [ ] Perfect offline 3×3 pruning DBs
- [ ] Production signed APK + verified native .so
- [ ] Adaptive launcher icons
- [ ] Measure replaySolved rate on 4×4/5×5 after desktop compile

---

## Next steps / approaches to try next time (2026-08-31 post-L_fixed)

1. **Highest leverage:** compile and run `desktop_harness` on 4×4/5×5 with raised `RCS_MITM_*`. Log MITM hit rate, OBTM vs U(n) vs L(n) vs L_fixed vs community 54, and **replaySolved=yes rate**. This session compiled `print_bounds` only (full solver link is heavier).
2. If replaySolved is low, debug SiGN encode/decode round-trip on ReductionSolver output (single encoder).
3. Per-stage OBTM targeting: cut the fattest phase only (centers vs edges vs parity). BoundReport already splits stages — use harness histograms to pick the cut.
4. Optional next bound: publish L_fixed in the Android JNI bound string (Java side still shows L/U only).
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
16. Consider a second constructive U(n) family (block-turn / cascade pairing) that might beat 92n²−307n+c without claiming diameter.

---

*Exact g(n) for n≥4 remains open. Constructive reduction + Demaine batching + residual MITM is the universal algorithm this repo ships. L_fixed + U−L gap + OEIS lock landed 2026-08-31.*
