# Bound harness — work backward from U(n)

## Constructive upper U(n)

| n | U(n) max moves (sufficient) |
|---|-----------------------------|
| 3 | 20 (proven God's Number) |
| 4 | **501** |
| 5 | **878** |
| 6 | **1321** |
| 7 | **1852** |
| 8 | **2473** |
| 9 | **3182** |
| 10 | **3981** |

Formula (n ≥ 4):

- even: `92n² - 307n + 257`
- odd:  `92n² - 307n + 113`

## Asymptotic shape

`asymptoticTarget(n) ≈ 3.8 * n² / ln(n)` (Demaine Θ shape; scale fitted to community 4×4/5×5 estimates).

## Dual metrics (SSTM / OBTM)

| Metric | Definition in this harness |
|--------|----------------------------|
| **SSTM** | Every `Move` counts as 1 (single-slice / face turn) |
| **OBTM** | Consecutive `depth==0` turns on the **same face** collapse to 1; every inner slice (`depth>0`) still counts 1 |

For n=4 the report also emits `vs4x4OBTM55=under|over` against the published community outer-block upper of **55**.

## Per-stage OBTM (2026-08-20)

Highest remaining diagnostic leverage. Each stage now reports both SSTM and OBTM on its own subsequence:

```
centers=N(obtm=M) edges=... parity=... reduced=... 3x3=... sstm=... obtm=... vs4x4OBTM55=...
```

This immediately shows which phase (centers / edges / parity / residual / 3×3) is furthest from the community 4×4 OBTM ≤55 ceiling and where optimization effort should go next.

## Code

| File | Role |
|------|------|
| `BoundHarness.h/.cpp` | U(n), asymptotic target, `BoundReport`, `countSstm`, `countObtm`, per-stage OBTM fields |
| `ReductionSolver` | Fills `StageLengths` (SSTM + OBTM per stage); feeds final sequence for dual metrics; `lastBoundReportString()` |
| JNI `nativeBoundReport` | Read last report after nxn solve (includes sstm/obtm + per-stage obtm) |
| JNI `nativeConstructiveUpper(n)` | Query U(n) |

### StageLengths

- `centers`, `edges`, `parity`, `reduced`, `reduce3x3` (SSTM)
- `centersObtm`, `edgesObtm`, `parityObtm`, `reducedObtm`, `reduce3x3Obtm`
- `afterBatch` = final length after `BatchSolver::optimize`
- `finalSstm` / `finalObtm` (when sequence provided)

### Report fields

- `withinU` — final ≤ U(n)?
- `final/U` — how close to constructive upper
- `final/asym` — vs n²/log n shape
- `sstm` / `obtm` — dual metric finals
- `vs4x4OBTM55` — only for n=4
- full per-stage `(obtm=…)` in the string

## Use

1. Solve nxn scramble
2. Read bound report (look at sstm + obtm + per-stage obtm)
3. Optimize the fattest stage (usually centers/edges)
4. When a *proven* always-≤ U' algorithm exists, lower U(n) — work backward toward g(n)
5. On 4×4 watch `obtm` (and which stage owns the bulk) against the 55 ceiling while tightening the constructive pipeline
