# Play Store readiness — checklist status

Branch: `play-store-readiness`  
Date: 2026-09-11

Honesty rule: do **not** claim integer g(4). leftoverE must be verified on device / `desktop_harness`, not assumed from code review alone.

| Item | Status | Notes |
|------|--------|-------|
| A. Edge pairing commutator rewrite | **Done (code)** / **Partial (verify)** | `pairOne`/`pairAll` use depth-specific A B A' B' (+ 8-move variants) with try/undo + `repairLeftovers` (≤24 rounds). Public API unchanged. **Run `./artifacts/rcs_harness 4 10` (or `4 1`) to measure leftoverE** — last known measurement before this branch was leftoverE≈7. |
| B. BillingManager + PremiumStore | **Done** | Product id `premium_unlock`; SharedPreferences cache; `DEBUG_PREMIUM` / debug preference bypass; free tier size==3. |
| C. MainActivity UX | **Done** | Size 3/4/5 chips, paywall, scramble/solve/reset/restore, boundReport after nxn, billing lifecycle. |
| D. Gradle release config | **Done** | versionName 1.0.0, minify/shrink, optional signing props, billing-ktx 7.1.1, arm ABIs only, buildConfig. |
| E. Adaptive icons | **Done** | Vector background/foreground/monochrome + anydpi-v26 adaptive XML; manifest icon/roundIcon. |
| F. Strings | **Done** | App name “Cube Solver”, premium / unlock CTAs, tagline. |
| G. CI release artifacts | **Done** | Debug APK kept; release assemble/bundle + `.so` check + AAB upload. |
| H. Docs (PLAY_STORE / PRIVACY / checklist) | **Done** | This file + listing/privacy guides. |
| I. PR | **Done** when opened | Via GitHub API (Cloud Agent unavailable). |
| Play Console listing / IARC / Data safety | **Manual** | Publisher steps in PLAY_STORE.md. |
| Keystore + signed AAB upload | **Manual** | Provide KEYSTORE_* gradle properties / CI secrets; never commit secrets. |
| Create `premium_unlock` IAP | **Manual** | Play Console → In-app products. |
| leftoverE = 0 on random 4×4 | **Needs verification** | Code path rewritten; harness confirmation required before claiming completeness. |
| Perfect 3×3 pruning DBs | **Not in scope** | Still open on main roadmap. |

## Suggested verification

```bash
# Desktop edge leftover check
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
./artifacts/rcs_harness 4 10
```

Look for `leftoverE=0` across trials. Exact g(4) remains open regardless of leftoverE.
