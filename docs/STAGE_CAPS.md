# Stage caps + leftover commutators (2026-09-07)

When BoundReport `overC` or `overE` is positive, ReductionSolver runs the overflowing stage through `StageCap`:

- Centers budget `C = 8(n-2)²` — one 8-move commutator per extra-center cell.
- Edges budget `E = 96(n-2)` — one 8-move pair per wing.
- `StageCap::eightMoveCommutator` is the canonical 8-move `[A,B][A,B]` family.
- `StageCap::capToBudget` clips a stage sequence to that integer.
- **2026-09-07:** `leftoverCenterCells` / `leftoverUnpairedWings` + `leftoverCommutators` + `capThenRepair`.
  After a clip (or if leftovers remain even under budget), emit one 8-move commutator per leftover instead of dropping the tail silently. Bound report string appends `leftoverC` / `leftoverE`.

This is **not** a proof that remaining unsolved cells are placed. The commutators are the never-break family from NEXT.md item 2. Exact g(n) for n≥4 remains open.
