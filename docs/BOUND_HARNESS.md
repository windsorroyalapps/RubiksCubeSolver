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

`asymptoticTarget(n) ≈ 4 * n² / ln(n)` (Demaine Θ shape; constant is a placeholder).

## Code

| File | Role |
|------|------|
| `BoundHarness.h/.cpp` | U(n), asymptotic target, `BoundReport` |
| `ReductionSolver` | Fills `StageLengths` per solve; `lastBoundReportString()` |
| JNI `nativeBoundReport` | Read last report after nxn solve |
| JNI `nativeConstructiveUpper(n)` | Query U(n) |

### StageLengths

- `centers`, `edges`, `parity`, `reduce3x3`
- `afterBatch` = final length after `BatchSolver::optimize`

### Report fields

- `withinU` — final ≤ U(n)?
- `final/U` — how close to constructive upper
- `final/asym` — vs n²/log n shape

## Use

1. Solve nxn scramble
2. Read bound report
3. Optimize the fattest stage (usually centers/edges)
4. When a *proven* always-≤ U' algorithm exists, lower U(n) — work backward toward g(n)
