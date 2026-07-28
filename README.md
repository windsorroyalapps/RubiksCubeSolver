# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**

- **3×3:** God's algorithm path toward **≤ 20 HTM** (proven God's Number)
- **n×n (n ≥ 4):** Reduction + Demaine-style batching; work backward from constructive **U(n)**

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

## n×n path (practical God's algorithm)

```text
ClusterScheduler → BatchGroups → Centers → Edges
  → Parity (even n) → 3×3 → BatchSolver::optimize
  → BoundHarness report
```

Demaine insight: batch shared slice moves toward **O(n² / log n)** spirit.

→ [docs/DEMAINE_BATCHING.md](docs/DEMAINE_BATCHING.md) · [docs/CLUSTER_SCHEDULING.md](docs/CLUSTER_SCHEDULING.md)

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

After each nxn solve, stage lengths are compared to U(n) and to ~n²/log n.

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
| [GODS_NUMBER_4x4_TO_10x10.md](docs/GODS_NUMBER_4x4_TO_10x10.md) | Estimates 4×4–10×10 |
| [KOCIEMBA_TWO_PHASE.md](docs/KOCIEMBA_TWO_PHASE.md) | Two-phase + IDA* |
| [DEMAINE_BATCHING.md](docs/DEMAINE_BATCHING.md) | n²/log n batching |
| [CLUSTER_SCHEDULING.md](docs/CLUSTER_SCHEDULING.md) | Shared-move schedule |
| [BOUND_HARNESS.md](docs/BOUND_HARNESS.md) | U(n) instrumentation |
| [JNI_WRAPPER.md](docs/JNI_WRAPPER.md) | Kotlin ↔ C++ |

---

## Status

- [x] GodsAlgorithm + Kociemba IDA* (3×3)
- [x] nxn reduction + parity
- [x] ClusterScheduler + BatchGroups + BatchSolver
- [x] BoundHarness (U(n) table + stage report)
- [x] JNI: solve + boundReport + constructiveUpper
- [ ] Perfect offline 3×3 pruning DBs
- [ ] Production signed APK

---

*Android/BMW hacking style. Ship or die.*
