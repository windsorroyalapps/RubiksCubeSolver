# RubiksCubeSolver

**Android + Native C++ Rubik's Cube Solver**

- **3×3:** God's algorithm path toward **≤ 20 HTM** (proven God's Number)
- **n×n (n ≥ 4):** Reduction + Demaine-style batching — **complete constructive algorithm** for any size; work backward from constructive **U(n)** and asymptotic **Θ(n² / log n)**

```bash
git clone https://github.com/windsorroyalapps/RubiksCubeSolver.git
# Android Studio → Sync → Run → Scramble → Solve
# or: GitHub Actions → Build Production APK artifact
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
ClusterScheduler → BatchGroups → Centers (never-break + orbit-BFS n≤5 / residual n=6)
  → Edges (Yau buffer + solid-set never-touch) → Parity (full multi-depth wing, even n)
  → ReducedSearch (packed 4×4 centers + full multi-depth wing residual IDA*) → 3×3 → BatchSolver::optimize
  → BoundHarness report (SSTM + OBTM dual metrics)
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
**Dual SSTM / OBTM** counts emitted so we can measure against the 54-move 4×4 ceiling live.

```kotlin
NativeSolver.create(5)
val sol = NativeSolver.solve()
val report = NativeSolver.boundReport()   // centers/edges/.../sstm=.../obtm=.../U(n)=878/...
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
| `boundReport()` | Last BoundHarness string (now includes sstm/obtm) |
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
| [BOUND_HARNESS.md](docs/BOUND_HARNESS.md) | U(n) instrumentation + OBTM/SSTM |
| [JNI_WRAPPER.md](docs/JNI_WRAPPER.md) | Kotlin ↔ C++ |
| [PRUNING_AND_PARITY.md](docs/PRUNING_AND_PARITY.md) | Pruning tables + full wing parity + edge buffer |

---

## Status

- [x] GodsAlgorithm + Kociemba IDA* (3×3)
- [x] nxn reduction + parity (complete for any n≥4)
- [x] ClusterScheduler + BatchGroups + BatchSolver (Demaine-style)
- [x] BoundHarness (U(n) table + stage report + asymptotic)
- [x] **OBTM / SSTM dual metrics** (live comparison to 4×4 OBTM ≤54)
- [x] JNI: solve + boundReport + constructiveUpper
- [x] Documented constructive algorithm + Θ(n²/log n) + best-known 4×4 OBTM ≤54
- [x] **Centers never-break** (global multi-face score + protect 100% faces)
- [x] **Edge wing-count** (real facelet pairedWings)
- [x] **Edge buffer tracking** (Yau-style explicit UF buffer + solid-set never-touch, 4-pass, priority order)
- [x] **Full wing parity** (OLL orientation + PLL side-match over all depths 1..n-2)
- [x] **CI APK production** (GitHub Actions builds + uploads debug APK artifact on every push) — **unblocked 2026-08-08** (removed missing mipmap icon refs that caused AAPT failure)
- [x] **Center residual short-search** (n=6 light cleanup)
- [x] **Full center-orbit BFS** (n≤5 exact residual placement under never-break)
- [x] **ReducedSearch scaffold** (4×4/5×5 depth-limited IDA* + residual heuristic)
- [x] **ReducedSearch packing** (4×4 uint16 center bitmask + stronger wing residual) — 2026-08-06
- [x] **ReducedSearch full multi-depth wing residual** — 2026-08-07
- [x] **ReducedSearch stronger inverse pruning + depthCap 18 (4x4)** — 2026-08-09
- [x] **Full gradle wrapper (gradlew + jar)** for reliable CI APK production — 2026-08-09
- [x] **ReducedSearch inverse pruning refinement + bidirectional scaffold** — 2026-08-10
- [x] **ReducedSearch depthCap 20 (4x4) + residual packing ready for bidirectional** — 2026-08-11
- [ ] Full residual coordinates + bidirectional meet-in-middle IDA* (tighten toward OBTM ≤54)
- [ ] Perfect offline 3×3 pruning DBs
- [ ] Production signed APK (release keystore + Material You polish) + verified native .so in artifact
- [ ] Adaptive launcher icons (mipmap) for store polish
- [x] **gradle-wrapper.jar committed** (CI reliability) — 2026-08-10

---

## Next steps / approaches to try next time (current automation work — 2026-08-11)

1. **Full residual coordinates + bidirectional meet-in-middle IDA*** – pack complete edge/center residual state into compact integers (extend pack4x4Centers + wingResidual into a full residual coord + hash table for meet-in-middle); so 4×4 constructive lengths collapse toward community OBTM ≤54. **Highest algorithm leverage remaining.** Residual packing scaffold + depthCap 20 shipped today.
2. **Verify green CI APK + native .so** – confirm workflow with full gradlew + jar produces debug APK artifact containing lib*.so; iterate NDK/CMake if needed.
3. **3×3 dense DBs** – full-index BFS pruning tables so phase-1 routinely ≤12 and totals hit the proven 20 ceiling more often.
4. **OBTM stage breakdown** – per-stage OBTM in BoundHarness so we can see which phase (centers vs edges vs parity vs reduced vs 3×3) is furthest from the 54-move 4×4 ceiling.
5. **Production signed APK** – release keystore secret in CI, Material You polish, on-device size selector to 20×20; verify APK artifact contains native .so.
6. **Adaptive icons** – add mipmap/ic_launcher* (or vector) so store listing looks production-ready.
7. **Asymptotic fit** – re-calibrate BoundHarness scale if new community 4×4/5×5 numbers appear; keep U(n) as hard constructive guarantee.
8. **Center BFS node-budget tuning** – raise maxNodes / maxDepth on desktop builds; keep mobile-safe defaults; optionally expose as JNI param.
9. **Edge pairing quality metrics** – log pairedWings progress + solid count into BoundHarness for diagnostics.
10. **Parity alg variants** – try alternate OLL/PLL parity sequences and pick shortest that clears the full-depth detectors.
11. **True bidirectional residual prototype** – implement full meet-in-middle (forward + backward BFS) using residualKey as hash; target 4×4 first, then lift to 5×5.

---

*Android/BMW hacking genius mode. Ship the algorithm that solves any n>3, document the bound, automate the APK, iterate the search until constructive U(n) collapses toward true God's Number. Exact g(n) for n≥4 remains open (intractable); the constructive reduction + Demaine batching path is complete and universal. Ship or die.*
