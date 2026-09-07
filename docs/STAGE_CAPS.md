# Stage caps + leftover commutators (2026-09-08)

When BoundReport `overC` or `overE` is positive, ReductionSolver runs the overflowing stage through `StageCap`:

- Centers budget `C = 8(n-2)²`
- Edges budget `E = 96(n-2)`
- `StageCap::eightMoveCommutator` is the canonical 8-move `[A,B][A,B]` family
- `StageCap::capToBudget` clips a stage sequence to that integer

**2026-09-07:** leftover measurement + generic commutators after clip.

**2026-09-08 completeness-first:** `capThenRepair(raw, preStage, budget, centers?)`

1. Apply full `raw` to a copy of `preStage`. Measure leftover_full.
2. Apply clipped prefix. Measure leftover_cap.
3. If leftover_cap > leftover_full → **return full raw** (do not emit an unsolved prefix just to hit Ucas).
4. If leftover_cap == 0 → return clipped.
5. Else try leftover commutators on the *capped* state; keep them only if leftover strictly drops.

Ucas remains the solver *target*. Completeness remains the algorithm contract.

This is **not** a diameter proof. Exact g(n) for n≥4 remains open.
