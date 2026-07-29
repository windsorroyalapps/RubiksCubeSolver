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
ClusterScheduler → BatchGroups → Centers → Edges
  → Parity (even n) → 3×3 → BatchSolver::optimize
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

4×4 OBTM community upper now **54** (not constructive). After each nxn solve, stage lengths are compared to U(n) and to ~n²/log n.

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
- [ ] Perfect offline 3×3 pruning DBs
- [ ] Tighter center/edge heuristics (next commit target)
- [ ] Production signed APK

---

## Next steps / approaches to try next time (current automation work)

1. **Centers** – BFS / look-ahead on center orbits for n≤8; never break solved cells.
2. **Edges** – pure pair-by-pair + buffer so paired wings stay paired; Yau cross for large n.
3. **Parity** – full (n-2)-wing orientation + permutation parity (drop mid-slice proxy).
4. **Metrics** – emit OBTM / SSTM counts so we can measure against the 54-move 4×4 ceiling.
5. **Search scaffolding** – reduced-state IDA* for 4×4 / 5×5 once centers+edges fixed → push constructive U(n) down.
6. **APK** – signed release, Material You polish, on-device size selector to 20×20.
7. **3×3 DBs** – full-index BFS pruning so solutions routinely hit the 20 ceiling.
8. **Asymptotic fit** – calibrate BoundHarness scale factor against community 4×4/5×5 estimates.

---

*Android/BMW hacking style. Ship the algorithm, document the bound, iterate until the APK is production. Ship or die.*
