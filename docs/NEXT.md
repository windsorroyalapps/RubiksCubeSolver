# Next session log — 2026-09-07 (harness run)

## Done this session
- Compiled `desktop_harness` **with StageCap.cpp** (`g++ -O2 -std=c++17` full file list from README).
- Ran `RCS_MITM_NODEBUDGET4=150000 RCS_MITM_DEPTHCAP4=28 ./rcs_harness 4 10`.
- `notation_selftest=pass` on short SiGN (`R U R' U' 2R Rw`).
- Still **no exact integer g(n) for n≥4**.

## 4×4 × 10 results (leftover-repair path, seed 20260828, scrambleLen=n*8=32)

Header: L=35 Lfix=32 U=501 Ucas=288 UcasC=32 UcasE=192 UcasP=20 asym~43

| trial | obtm | vs54 | overC | overE | leftoverC | leftoverE | fattest | replaySolved |
|------:|-----:|------|------:|------:|----------:|----------:|---------|--------------|
| 1 | 53 | under | 0 | 64 | 0 | 8 | edges | no |
| 2 | 50 | under | 0 | 56 | 0 | 7 | edges | no |
| 3 | 48 | under | 0 | 56 | 0 | 7 | edges | no |
| 4 | 55 | **over** | 16 | 64 | 2 | 8 | edges | no |
| 5 | 51 | under | 0 | 64 | 0 | 8 | edges | no |
| 6 | 50 | under | 0 | 64 | 0 | 8 | edges | no |
| 7 | 54 | under | 0 | 64 | 0 | 8 | edges | no |
| 8 | 50 | under | 0 | 64 | 0 | 8 | edges | no |
| 9 | 51 | under | 0 | 64 | 0 | 8 | edges | no |
| 10 | 48 | under | 0 | 64 | 0 | 8 | edges | no |

- **replaySolved = 0/10**
- avgNodes ≈ 151195 (budget 150000; MITM hits = 0 every trial)
- obtm range 48–55, mean 51.0; 9/10 under community 54, trial 4 over (55)
- leftoverE is the constant leftover (7–8); leftoverC only on trial 4
- fattest stage always **edges**; overE 56–64 vs UcasE=192
- 3x3 stage reported 0 moves every trial
- withinU=yes all trials (vs U=501)

Clip-only comparison was **not** run this session (binary is leftover-repair only).

## Branch decision (same session)
**Do not start UI.** replaySolved=0/10 → NEXT item 3 wins over item 2.

- Targeting (per-cell A/B) is **deferred**. leftoverE is large and stable; that can wait until replay is truthful.
- **Active branch: SiGN encode/decode of the full solver string.** Short selftest passed, so the bug is in long mixed-depth sequences from ReductionSolver (`movesToNotation` ↔ `applyNotation`), or the solver is emitting a sequence that does not actually solve when replayed from the scramble.

## Do not claim
Exact integer God's number for any n≥4. |G(4)|≈7.4e45. Published window is still 35–54 OBTM.

## Try next
1. SiGN round-trip on one failing trial: dump scramble moves + notation + cube.toString() before/after applyNotation. Single encoder.
2. Confirm whether `ReductionSolver::solve` leaves the work cube solved *before* notation (if yes → encode/decode; if no → leftover commutators / StageCap not restoring state).
3. After replaySolved > 0, then per-cell targeting if leftover commutators scramble orbits.
4. Surface leftoverC/leftoverE + Ucas/L/Lfix + overC/overE in Android UI — **blocked on replay rate**.
5. 3×3 dense pruning DBs toward proven 20.
6. Verify green CI APK contains lib*.so.
7. Do not invent an integer g(4).
8. Keep Ucas honest: leftover repair can grow measured length past Ucas; if *correct* solutions exceed Ucas, raise the constant. (Not actionable until replaySolved is real.)
9. Sample-based demigod estimate once harness emits *correct* lengths.
10. Center BFS node-budget tuning; Edge `pairedWings` metric into BoundHarness.
