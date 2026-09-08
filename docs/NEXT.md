# Next session log — 2026-09-09 (center completeness)

## Done this session
- `CenterSolver::solveFace` no longer appends a raw outer turn when `bestGain <= 0`. That fallback burned attempts without reducing `incorrectCenters`.
- `centerOrbitBfs` budgets raised: n=4 maxDepth 7 / maxNodes 40000; n=5 maxDepth 5 / maxNodes 20000. Env `RCS_CENTER_BFS_NODES` / `RCS_CENTER_BFS_DEPTH` override.
- BFS loops while leftover centers decrease (up to 6 rounds).
- `EdgePairing::pairAll` stops when `leftoverUnpairedWings()==0` (all 12 edges have `pairedWings==n-2`).
- Docs/README status updated. Exact integer g(n) for n>=4 still **open**.

## Still true
SiGN short selftest passes. Failure mode remains solver completeness (centers/edges not finishing), not encode/decode.

## Do not claim
Exact integer God's number for any n>=4. |G(4)|~7.4e45. Published window 35–54 OBTM.

## Try next
1. Desktop harness 4x4 after this change: measure leftoverC / leftoverE / workSolved.
2. If leftoverC>0, add per-cell 8-move commutator that names owning face + orthogonal slice (do not use outer-only turns).
3. If leftoverE>0, replace freeslice RUR' cycles with a commutator that only touches one unpaired wing depth.
4. After workSolved=yes, re-measure replaySolved.
5. Surface leftoverC/E + workSolved in Android UI only after workSolved is real.
6. Do not invent an integer g(4).
