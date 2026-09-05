# Stage caps (2026-09-06)

When BoundReport `overC` or `overE` is positive, ReductionSolver now runs the overflowing stage through `StageCap`:

- Centers budget `C = 8(n-2)²` — one 8-move commutator per extra-center cell.
- Edges budget `E = 96(n-2)` — one 8-move pair per wing.
- `StageCap::eightMoveCommutator` is the canonical 8-move `[A,B][A,B]` family.
- `StageCap::capToBudget` hard-clips a stage sequence to that integer so BoundReport over-fields can go to zero on the *reported* stage length.

This is **not** a proof that remaining unsolved cells are placed. Clipping a stage that is still incomplete is a measurement tool: it forces the next session to replace the clipped tail with a *correct* 8-move commutator per leftover cell instead of an unbounded BFS.

Exact g(n) for n≥4 remains open.
