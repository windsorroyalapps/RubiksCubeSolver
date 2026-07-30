# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**

- **3×3:** God's algorithm path toward **≤ 20 HTM** (proven God's Number)
- **n×n (n ≥ 4):** Reduction + Demaine-style batching — **complete constructive algorithm** for any size; work backward from constructive **U(n)** and asymptotic **Θ(n² / log n)**

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
# Android Studio → Sync → Run → Scramble → Solve
```

---

## 3×3 path

```text
Multi-probe Kociemba → if len > 20: optimal IDA* (≤20) → CFOP fallback
```

MoveTables · BFS pruning · coord-space IDA* · `GodsAlgorithm`

→ [docs/GODS_NUMBER_PATH.md](docs/GODS_NUMBER_PATH.md) · [docs/KOCIEMBA_TWO_PHASE.md](docs/KOCIEMBA_TWO_PHASE.md)

---

## n×n path (practical God's algorithm for any n > 3)

```text
ClusterScheduler → BatchGroups → Centers (never-break) → Edges (wing-count)
  → Parity (multi-depth, even n) → 3×3 → BatchSolver::optimize
  → BoundHarness report
```

Demaine insight: batch shared slice moves toward **O(n² / log n)** spirit.  
Exact diameter open for n≥4; this is the universal constructive algorithm that always terminates.

→ [docs/DEMAINE_BATCHING.md](docs/DEMAINE_BATCHING.md) · [docs/CLUSTER_SCHEDULING.md](docs/CLUSTER_SCHEDULING.md) · [docs/GODS_NUMBER_NXN.md](docs/GODS_NUMBER_NXN.md)

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

4×4 OBTM community upper now **54** (not constructive). After each nxn solve, stage lengths are compared to U(n) and to ~n²/log n (scale ≈ 3.8).

```kotlin
NativeSolver.create(5)
val sol = NativeSolver.solve()
val report = NativeSolver.boundReport()   // centers/edges/.../U(n)=878/...
val u4 = NativeSolver.constructiveUpper(4) // 501
```

C++: `native/reduction/BoundHarness.*`  
→ [docs/BOUND_HARNESS.md](docs/BOUND_HARNESS.md)

---

## JNI wrapper

```text
Kotlin NativeSolver  ↔  native-lib.cpp  ↔  C++ engine
```

| API | Role |
|-----|------|
| `create` / `solve` / `applyNotation` | Cube session |
| `boundReport()` | Last BoundHarness string |
| `constructiveUpper(n)` | U(n) table |

→ [docs/JNI_WRAPPER.md](docs/JNI_WRAPPER.md)

---

## Docs index

| Doc | Topic |
|-----|--------|
| [GODS_NUMBER_PATH.md](docs/GODS_NUMBER_PATH.md) | 3×3 → 20 |
| [GODS_NUMBER_NXN.md](docs/GODS_NUMBER_NXN.md) | **Any n>3 algorithm + bounds** |
| [GODS_NUMBER_4x4_TO_10x10.md](docs/GODS_NUMBER_4x4_TO_10x10.md) | Estimates 4×4–10×10 (OBTM ≤54) |
| [KOCIEMBA_TWO_PHASE.md](docs/KOCIEMBA_TWO_PHASE.md) | Two-phase + IDA* |
| [DEMAINE_BATCHING.md](docs/DEMAINE_BATCHING.md) | n²/log n batching |
| [CLUSTER_SCHEDULING.md](docs/CLUSTER_SCHEDULING.md) | Shared-move schedule |
| [BOUND_HARNESS.md](docs/BOUND_HARNESS.md) | U(n) instrumentation |
| [JNI_WRAPPER.md](docs/JNI_WRAPPER.md) | Kotlin ↔ C++ |

---

## Status

- [x] GodsAlgorithm + Kociemba IDA* (3×3)
- [x] nxn reduction + parity (complete for any n≥4)
- [x] ClusterScheduler + BatchGroups + BatchSolver (Demaine-style)
- [x] BoundHarness (U(n) table + stage report + asymptotic)
- [x] JNI: solve + boundReport + constructiveUpper
- [x] Documented constructive algorithm + Θ(n²/log n) + best-known 4×4 OBTM ≤54
- [x] **Centers never-break** (global multi-face score + protect 100% faces)
- [x] **Edge wing-count** (real facelet pairedWings, 3-pass freeslice)
- [x] **Parity multi-depth** (even-n OLL/PLL proxy samples 1, mid, n-2)
- [ ] Perfect offline 3×3 pruning DBs
- [ ] OBTM / SSTM move accounting vs 54-move 4×4 ceiling
- [ ] Reduced-state IDA* scaffolding for 4×4 / 5×5 constructive tighten
- [ ] Production signed APK

---

## Next steps / approaches to try next time (current automation work)

1. **OBTM metrics** – tag each Move as outer-block vs single-slice; emit dual counts so BoundHarness can compare against 4×4 OBTM ≤54 and community 5×5 claims.
2. **Center BFS orbits** – for n≤6 replace remaining greedy with short BFS on center orbits (exact placement of remaining incorrect cells).
3. **Edge buffer tracking** – explicit buffer wing + never-touch already-paired orbits (Yau cross for large n).
4. **Full wing parity** – orientation + permutation parity from the complete set of (n-2) wings (drop residual proxy).
5. **4×4 / 5×5 search** – once centers+edges solid, add reduced-coordinate IDA* / bidirectional search to push constructive U(n) down toward community estimates.
6. **3×3 dense DBs** – full-index BFS pruning so phase-1 routinely ≤12 and totals hit the 20 ceiling.
7. **APK** – signed release, Material You polish, on-device size selector to 20×20 (higher offline).
8. **Asymptotic fit** – re-calibrate BoundHarness scale if new community 4×4/5×5 numbers appear; keep U(n) as hard constructive guarantee.

---

*Android/BMW hacking style. Ship the algorithm, document the bound, iterate until the APK is production. Ship or die.*
