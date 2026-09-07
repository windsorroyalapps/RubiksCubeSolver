# Next session log — 2026-09-08 (cap completeness)

## Done this session
- Fixed `StageCap::capThenRepair`: leftovers are measured on the **pre-stage** cube after applying candidate sequences, not on the fully-applied raw cube while returning a clipped prefix.
- If clip increases leftover vs full raw, keep full raw. Ucas is a budget, not a correctness cut.
- `ReductionSolver` logs `workSolved` (is the work cube solved *before* notation?).
- Compiled and ran `desktop_harness` 4x4 x 1.
- Still **no exact integer g(n) for n>=4**.

## 4x4 x 1 (completeness-first cap, seed 20260828, scrambleLen=16)
- notation_selftest=pass
- workSolved=no
- replaySolved=no
- leftoverC=2 leftoverE=8
- centers=666 edges=908 parity=15 reduced=0 3x3=0 final sstm/obtm=103 withinU=yes vsOBTM54=over
- MITM hits=0

SiGN short selftest still passes. Failure is **solver completeness**, not encode/decode.

## Branch decision
Stay off UI. Do not invent g(4).

Active branch: **CenterSolver + EdgePairing must drive leftover to 0**. The 2026-09-07 leftover commutators do not substitute for unfinished reduction.

## Do not claim
Exact integer God's number for any n>=4. |G(4)|~7.4e45. Published window is still 35-54 OBTM.

## Try next
1. CenterOrbitBfs: drop the outer-turn fallback when bestGain<=0; raise maxNodes on desktop.
2. EdgePairing stop when leftoverUnpairedWings==0 / pairedWings==n-2 on all 12 edges.
3. After workSolved>0, re-measure replaySolved.
4. Then per-cell targeting if needed.
5. Keep Ucas honest only on *correct* solutions.
6. Do not invent an integer g(4).
