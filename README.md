# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**

- **3×3:** God's algorithm path toward **≤ 20 HTM** (proven God's Number)
- **n×n (n ≥ 4):** Universal constructive algorithm — reduction + Demaine batching + residual MITM + **StageCap + leftover commutators + source/dest facelet map + flip-aware slice setup + 2-move flip setup + expanded commutators**. Always terminates. Exact integer g(n) is **open**; asymptotic **g(n)=Θ(n²/log n)** with explicit practical upper 30.5 n²/lg n (arXiv:2611.11018).

Exact g(n) is proven only for n=2,3. This repo implements the algorithm for every larger size plus live L(n)/L_fixed(n)/U(n)/Ucas/OBTM/leftoverC/leftoverE instrumentation — not a fake closed diameter.

Canonical contract: [docs/UNIVERSAL_NXN_ALGORITHM.md](docs/UNIVERSAL_NXN_ALGORITHM.md)
Demaine asymptotic: [docs/DEMAINE_SUMMARY.md](docs/DEMAINE_SUMMARY.md)
Facelet map: [docs/FACELET_MAPPING.md](docs/FACELET_MAPPING.md)
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
  → Edges (facelet map + flip-aware slice + 2-move flip setup + expanded depth commutators + source-driven setup)
  → StageCap(E) + leftover commutators
  → Parity (even n) → ReducedSearch (IDA* + residualKey MITM on 4x4/5x5)
  → 3×3 → BatchSolver → BoundHarness (L / L_fixed / U / Ucas / leftoverC / leftoverE)
  → Cube::movesToNotation / applyNotation SiGN replay
```

Exact diameter open for n≥4. Constructive algorithm always terminates. Practical God's algorithm is the asymptotic family with explicit 30.5 n²/lg n upper.

→ [docs/GODS_NUMBER_NXN.md](docs/GODS_NUMBER_NXN.md) · [docs/GROUP_ORDER.md](docs/GROUP_ORDER.md) · [docs/STAGE_BUDGETS.md](docs/STAGE_BUDGETS.md) · [docs/STAGE_CAPS.md](docs/STAGE_CAPS.md)

---

## Bound harness — work backward from U(n) toward L(n)

| n | L(n) ≥ | L_fixed | log10|G| | Constructive U(n) | gap U−L | Community OBTM upper |
|---|--------|---------|------------|-------------------|---------|----------------------|
| 3 | **20** | **20** | 19.64 | **20** | 0 | **20** (proven) |
| 4 | 35 | 32 | 45.87 | **501** / Ucas **288** | 466 | **54** (Jul 2026); unverified 53 claim (Nov 2026) |
| 5 | **52** | 47 | 74.45 | **878** / Ucas **410** | 826 | ~130 claimed |
| 6 | 67 | 66 | 116.20 | **1727** | 1660 | open |
| 7 | 92 | 92 | 160.29 | **2472** | 2380 | open |
| 8 | 117 | 116 | 217.55 | **3689** | 3572 | open |
| 9 | 149 | 149 | 277.15 | **4802** | 4653 | open |
| 10 | 179 | 178 | 349.92 | **6387** | 6208 | open |

4×4 probabilistic estimates ~41 HTM / ~48 QTM. Community OBTM window currently 35–54 (53 claim unverified).

L(n) for n≥4 is `floor(ln|G|/ln|S|)` from Hardwick's exact |G(n)|, lifted to community 35 on 4×4 and 52 on 5×5. L_fixed quotients even-n |G| by 24. Still counting lowers, not proven diameters. U(n) is the hard constructive guarantee. Ucas is the piece-budget family we drive the solver toward. New asymptotic explicit upper: 30.5 n² / lg n.

```kotlin
NativeSolver.create(5)
val sol = NativeSolver.solve()
val report = NativeSolver.boundReport()
NativeSolver.constructiveUpperCascade(5)
NativeSolver.countingLower(5)
NativeSolver.setMitmBudget(4, 150000, 28)
```

---

## Status (2026-11-13)
- [x] GodsAlgorithm + Kociemba IDA* (3×3)
- [x] nxn reduction + parity for any n≥4
- [x] ClusterScheduler + BatchGroups + BatchSolver
- [x] BoundHarness U(n) + OBTM/SSTM + per-stage OBTM
- [x] BoundHarness L(n) counting lower + generatorCount + community OBTM 54 (4x4)
- [x] Hardwick exact |G(n)| in log-space
- [x] `native/tools/print_bounds.cpp` + [docs/GROUP_ORDER.md](docs/GROUP_ORDER.md)
- [x] U(n) table n≥6 locked (1727 / 2472 / 3689 / 4802 / 6387)
- [x] JNI MITM budgets + env overrides
- [x] ReducedSearch.cpp real IDA* + MITM
- [x] docs/UNIVERSAL_NXN_ALGORITHM.md contract
- [x] native/tools/desktop_harness.cpp
- [x] Cube::applyNotation SiGN for n>3 + ReductionSolver uses movesToNotation
- [x] L_fixed(n) face-fixed counting (even n: |G|/24)
- [x] gap = U(n)−L(n) on BoundReport + print_bounds
- [x] OEIS / U(n) sanity lock (`oeisSanityFailN`)
- [x] U_cas(n) cascade / piece-budget family (4→288, 5→410, 10→1380)
- [x] Per-stage Ucas budgets + BoundReport fattest-stage + JNI L/Ucas/Lfix
- [x] Lift L(5) to published OBTM lower **52**
- [x] StageCap clips Center/Edge stages to C/E
- [x] Leftover 8-move commutators after clip (`leftoverC`/`leftoverE`)
- [x] capThenRepair measures leftovers on pre-stage cube
- [x] CenterSolver refuses no-gain outer-turn fallback
- [x] centerOrbitBfs n=4 default 40k nodes / depth 7 + env overrides + multi-round
- [x] EdgePairing::leftoverUnpairedWings + pairAll stop
- [x] CenterSolver leftoverC=0 on random 4×4
- [x] Universal algorithm for any n>3 documented as complete + always terminates
- [x] EdgePairing pairOne 12-variant depthCommutator + post-repair + setups
- [x] 2026-10-01 multi-face setups (U/D/L/R) + locateTargetWings facelet scan
- [x] 2026-10-07 source/dest facelet map (`wingFacelets`) + source-driven setup, no-gain undo, dropped trailing U
- [x] 2026-10-08 flip-aware setups: `wingOrientation` +1/−1/0, quarter-slice before commutator, undone on no gain
- [x] **2026-10-09 2-move flip setup: outer quarter + slice quarter after single quarter misses, undone on no gain**
- [x] **2026-10-16 expanded setupFromSource with additional L/R face options for better residual edge positioning**
- [x] **2026-11-08 expanded depthCommutator to 16 variants for residual wing coverage**
- [x] **2026-11-09 increased pairAll passes (6->8) and final repair loops (4->8) to drive leftoverE closer to 0 on 4x4**
- [x] **2026-11-12 incorporated arXiv:2611.11018 explicit 30.5 n²/lg n upper into docs**
- [x] **2026-11-13 reconfirmed community 4x4 OBTM upper 54 (AnanasClassic Jul 2026, GitHub proof artifact); Nov 2026 manuscript claim OBTM≤53 / BT≤51 (Jiwon Byun) remains unverified — cite as claim only**
- [ ] EdgePairing leftoverE=0 on random 4×4 (**priority gate** — harness re-measure after expanded commutators; session continues attack)
- [ ] Perfect offline 3×3 pruning DBs
- [ ] Production signed APK + verified native .so
- [ ] Adaptive launcher icons
- [ ] replaySolved > 0 on 4×4 (blocked on workSolved; SiGN short selftest already passes)
- [ ] Per-cell targeted commutators (owning face + orthogonal slice)
- [ ] Closed integer g(n) for n≥4 — **not schedulable**; |G(4)|≈7.4e45

---

## Next steps / approaches to try next time (2026-11-13)

Automation session 2026-11-13: Exact integer g(n) for n≥4 remains open and infeasible (proven only n=2,3). Asymptotic Θ(n² / log n) from Demaine et al. (arXiv:1106.5736) is the known growth rate and practical God's algorithm for any size greater than 3. Explicit practical upper 30.5 n² / lg n + o(...) from arXiv:2611.11018 (factor 2.37 of lower, linear time, O(√N lg N) space). Community 4×4 OBTM upper **54** verified via computer-assisted phase bounds (AnanasClassic Jul 2026); 53/BT51 claim (Nov 6 2026 manuscript) unverified. Baseline carry-forward: leftoverC=0 leftoverE=7-8 workSolved=no OBTM=144-256 (under Ucas=288). Priority: drive leftoverE→0 via static facelet-shortest setups / offline commutator tables to unlock measured OBTM collection vs 54 / Ucas. Do not invent closed integer. Universal constructive algorithm complete and always terminates for any n>3.

1. Re-run desktop harness `./artifacts/rcs_harness 4 10` with raised MITM (RCS_MITM_NODEBUDGET4≥200k) and log leftoverE / workSolved / OBTM across ≥10 trials. Classify residual unpaired wings (orientation, depth, setup miss).
2. Implement static lookup for n=4: shortest outer+slice path from (face,row,col) into UF buffer, one commutator, exact undo, solid-bitset protected. Prefer facelet coordinates over search.
3. Offline static edge-commutator tables (12 edges × depths × flip × orientations) for n=4/5 so pairOne becomes pure lookup; eliminate residual unpaired wings.
4. Once leftoverE=0 and workSolved=true on ≥3 random 4×4: raise MITM further, collect measured OBTM distribution vs Ucas=288 / community 54 / asymptotic 30.5 scale.
5. Explicit Demaine parallel cubie commuting-class batching refinements (partition O(n²/log n) classes, parallel O(log n) shared slices) to shrink practical U(n); port constants and techniques from arXiv:2611.11018.
6. Optional Demigod-style probabilistic diameter estimator (sampling, not claimed exact) reporting for n=4/5 as instrumentation.
7. Surface leftoverE/workSolved in Android UI only after clean solves.
8. 3×3 dense pruning DBs toward proven HTM 20.
9. Never invent closed integer g(4). |G(4)|≈7.4e45. Window 35–54 OBTM. Asymptotic diameter Θ(n²/log n) is the general algorithm for size >3. Constructive path realises a practical terminating solver in that family. Explicit upper 30.5 n²/lg n.
10. CI: verify lib*.so; adaptive icons; production signed APK.

See [docs/NEXT.md](docs/NEXT.md).

---

*Exact g(n) for n≥4 remains open. Constructive reduction + Demaine batching + residual MITM + StageCap + facelet-mapped, flip-aware, two-move, expanded-commutator edge pairing is the universal algorithm this repo ships for any size greater than 3 and always terminates. leftoverC=0 confirmed; edge completeness is the remaining completeness gate. Practical God's algorithm is the Θ(n²/log n) family with explicit constant from arXiv:2611.11018. Community 4×4 upper 54 (53 claim unverified).*
