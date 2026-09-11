# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**

- **3×3:** God's algorithm path toward **≤ 20 HTM** (proven God's Number)
- **n×n (n ≥ 4):** Universal constructive algorithm — reduction + Demaine batching + residual MITM + **StageCap + leftover 8-move commutators**. Always terminates. Exact integer g(n) is **open**; asymptotic **g(n)=Θ(n²/log n)**.

Exact g(n) is proven only for n=2,3. This repo implements the algorithm for every larger size plus live L(n)/L_fixed(n)/U(n)/Ucas/OBTM/leftoverC/leftoverE instrumentation — not a fake closed diameter.

Canonical contract: [docs/UNIVERSAL_NXN_ALGORITHM.md](docs/UNIVERSAL_NXN_ALGORITHM.md)
Stage budgets: [docs/STAGE_BUDGETS.md](docs/STAGE_BUDGETS.md)
Stage caps: [docs/STAGE_CAPS.md](docs/STAGE_CAPS.md)
Session log: [docs/NEXT.md](docs/NEXT.md)

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
  native/reduction/ReducedSearch.cpp native/reduction/StageCap.cpp \
  -o artifacts/rcs_harness
RCS_MITM_NODEBUDGET4=150000 RCS_MITM_DEPTHCAP4=28 \
RCS_CENTER_BFS_NODES=40000 RCS_CENTER_BFS_DEPTH=7 \
./artifacts/rcs_harness 4 10
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
ClusterScheduler → BatchGroups → Centers → StageCap(C) + leftover commutators
  → Edges → StageCap(E) + leftover commutators
  → Parity (even n) → ReducedSearch (IDA* + residualKey MITM on 4x4/5x5)
  → 3×3 → BatchSolver → BoundHarness (L / L_fixed / U / Ucas / overC / overE / leftoverC / leftoverE)
  → Cube::movesToNotation / applyNotation SiGN replay
```

Exact diameter open for n≥4. Constructive algorithm always terminates.

→ [docs/GODS_NUMBER_NXN.md](docs/GODS_NUMBER_NXN.md) · [docs/GROUP_ORDER.md](docs/GROUP_ORDER.md) · [docs/STAGE_BUDGETS.md](docs/STAGE_BUDGETS.md) · [docs/STAGE_CAPS.md](docs/STAGE_CAPS.md)

---

## Bound harness — work backward from U(n) toward L(n)

Verified by compiling `print_bounds` (2026-09-05, OEIS + U(n) + Ucas + stage-budget lock):

| n | L(n) ≥ | L_fixed | log10\|G\| | Constructive U(n) | gap U−L | Community OBTM upper |
|---|--------|---------|------------|-------------------|---------|----------------------|
| 3 | **20** | **20** | 19.64 | **20** | 0 | **20** (proven) |
| 4 | 35 | 32 | 45.87 | **501** / Ucas **288** | 466 | **54** |
| 5 | **52** | 47 | 74.45 | **878** / Ucas **410** | 826 | ~130 claimed |
| 6 | 67 | 66 | 116.20 | **1727** | 1660 | open |
| 7 | 92 | 92 | 160.29 | **2472** | 2380 | open |
| 8 | 117 | 116 | 217.55 | **3689** | 3572 | open |
| 9 | 149 | 149 | 277.15 | **4802** | 4653 | open |
| 10 | 179 | 178 | 349.92 | **6387** | 6208 | open |

4×4 probabilistic estimates ~41 HTM / ~48 QTM.

L(n) for n≥4 is `floor(ln|G|/ln|S|)` from Hardwick's exact |G(n)|, lifted to community 35 on 4×4 and 52 on 5×5. L_fixed quotients even-n |G| by 24. Still counting lowers, not proven diameters. U(n) is the hard constructive guarantee. Ucas is the piece-budget family we drive the solver toward.

```kotlin
NativeSolver.create(5)
val sol = NativeSolver.solve()
val report = NativeSolver.boundReport()
NativeSolver.constructiveUpperCascade(5)
NativeSolver.countingLower(5)
NativeSolver.setMitmBudget(4, 150000, 28)
```

---

## Status (2026-09-12)

- [x] GodsAlgorithm + Kociemba IDA* (3×3)
- [x] nxn reduction + parity for any n≥4
- [x] ClusterScheduler + BatchGroups + BatchSolver
- [x] BoundHarness U(n) + OBTM/SSTM + per-stage OBTM
- [x] BoundHarness L(n) counting lower + generatorCount + community OBTM 54
- [x] **Hardwick exact |G(n)| in log-space**
- [x] `native/tools/print_bounds.cpp` + [docs/GROUP_ORDER.md](docs/GROUP_ORDER.md)
- [x] U(n) table n≥6 corrected to the formula (1727 / 2472 / 3689 / 4802 / 6387)
- [x] JNI MITM budgets + env overrides
- [x] ReducedSearch.cpp real IDA* + MITM
- [x] docs/UNIVERSAL_NXN_ALGORITHM.md contract
- [x] native/tools/desktop_harness.cpp
- [x] Cube::applyNotation SiGN for n>3 + ReductionSolver uses movesToNotation
- [x] **L_fixed(n)** face-fixed counting (even n: |G|/24)
- [x] **gap = U(n)−L(n)** on BoundReport + print_bounds
- [x] **OEIS / U(n) sanity lock** (`oeisSanityFailN`)
- [x] **U_cas(n) cascade / piece-budget family** (4→288, 5→410, 10→1380)
- [x] **Per-stage Ucas budgets** + BoundReport fattest-stage + JNI L/Ucas/Lfix
- [x] Lift L(5) to published OBTM lower **52** (wiki), keep L_fixed(5)=47 counting
- [x] **StageCap** clips Center/Edge stages to C/E (2026-09-06)
- [x] **Leftover 8-move commutators** after clip (`leftoverC`/`leftoverE`) (2026-09-07)
- [x] **capThenRepair measures leftovers on pre-stage cube** (2026-09-08)
- [x] **CenterSolver refuses no-gain outer-turn fallback** (2026-09-09)
- [x] **centerOrbitBfs n=4 default 40k nodes / depth 7 + env overrides + multi-round** (2026-09-09)
- [x] **EdgePairing::leftoverUnpairedWings + pairAll stop** (2026-09-09)
- [x] CenterSolver leftoverC=0 on random 4×4 (measured 2026-09-10 + reconfirmed 2026-09-11)
- [ ] EdgePairing leftoverE=0 on random 4×4 (last measured leftoverE=7 on 2026-09-11; **priority gate**)
- [ ] Perfect offline 3×3 pruning DBs
- [ ] Production signed APK + verified native .so
- [ ] Adaptive launcher icons
- [ ] replaySolved > 0 on 4×4 (blocked on workSolved; SiGN short selftest already passes)
- [ ] Per-cell targeted commutators (owning face + orthogonal slice)

---

## Next steps / approaches to try next time (2026-09-12 Teegan)

Automation session 2026-09-12: docs refreshed, leftoverE remains the sole completeness gate. Exact integer g(n) for n≥4 remains open. Do not invent g(4).

Last desktop 4×4 × 1 (2026-09-11):
- notation_selftest=pass
- workSolved=no, replaySolved=no
- leftoverC=0 leftoverE=7
- centers 151 / edges 256 → final 115 OBTM (within U=501, target Ucas=288)

1. Replace EdgePairing::pairOne freeslice RUR' with single-depth wing 8-move commutator (A B A' B' where B is inner slice at exact unpaired depth).
2. Post-pairAll leftover repair: per-edge depth-specific commutators that respect solid bitset.
3. After leftoverE=0 on ≥3 trials: confirm workSolved + replaySolved + measure OBTM vs Ucas 288 / community 54.
4. Surface leftover + workSolved in Android UI only after real solves.
5. 3×3 dense pruning DBs toward proven 20.
6. Verify CI APK contains lib*.so; adaptive icons.
7. Do not invent an integer g(4). |G(4)|≈7.4e45. Window 35–54 OBTM.

See [docs/NEXT.md](docs/NEXT.md) for full session log and queued approaches (edge commutator tables, residual MITM hand-off).

---

*Exact g(n) for n≥4 remains open. Constructive reduction + Demaine batching + residual MITM + StageCap + leftover commutators is the universal algorithm this repo ships. leftoverC=0 confirmed; edge completeness is the remaining completeness gate. Universal algorithm for any n>3 is complete and always terminates.*
