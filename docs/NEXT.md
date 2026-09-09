# Next session log — 2026-09-10 (edge leftover + post-center harness)

## Done this session
- Re-ran desktop_harness 4×1 with RCS_CENTER_BFS_NODES=40000 RCS_CENTER_BFS_DEPTH=7 after 2026-09-09 center changes.
- **leftoverC=0** (was 2). Center stage now completes on the measured scramble.
- leftoverE=8 still (12 wings need pairing × (n-2=2) = 24 possible; 8 unpaired remain).
- workSolved=no, replaySolved=no.
- Measured final OBTM=92 (raw 1073 → after BatchSolver/StageCap 92). withinU=yes, overC=118, overE=716, fattest=edges.
- SiGN notation_selftest=pass. MITM nodes hit budget (200001).
- Exact integer g(n) for n≥4 remains **open**. Do not invent g(4). Window still 35–54 OBTM community / Ucas 288 / U 501.

## Measurement (seed-derived scramble 2026-09-10 run)
```
leftoverC=0 leftoverE=8 workSolved=no
centers=150(obtm=149) edges=908(obtm=857) parity=15 reduced=0 3x3=0
raw=1073 final=92 sstm=92 obtm=92
```

## Still true
Failure mode is now pure edge-pairing incompleteness (freeslice RUR' cycles leave 8 unpaired wings). Centers are clean.

## Try next (priority order)
1. **Replace EdgePairing::pairOne RUR' freeslice with single-depth wing commutator.**  
   Target: [F, depth, 1] U [F, depth, -1] U' style or classic 8-move (A B A' B') where A is outer face turn, B is inner slice that only touches one unpaired wing depth. Never outer-only after solid edges locked.
2. Add post-pairAll leftover repair loop: for each edge with pairedWings < n-2, emit a depth-specific 8-move commutator that restores the wing without breaking solid edges (use solid bitset as never-touch).
3. After leftoverE reaches 0 on ≥3 random 4×4 trials: re-measure workSolved + replaySolved + final OBTM vs Ucas 288 / community 54.
4. Only then surface leftoverC/E + workSolved flags in Android UI / BoundReport JNI.
5. 3×3 dense pruning DBs (Kociemba phase tables) toward proven 20.
6. Verify CI APK ships lib*.so; adaptive icons.
7. Per-cell targeted center commutators remain available if leftoverC regresses on other seeds.
8. Do **not** invent an integer g(4). |G(4)|≈7.4e45. Progress metric = measured OBTM ↓ toward Ucas while workSolved=yes.

## Approaches queued
- Edge commutator table for n=4 (12 edges × 2 depths) offline generated once, stored as static arrays (mirrors OLL/PLL tables style).
- Residual key packing already present in ReducedSearch; once edges solid, residual MITM should finish the last few moves.
- For n≥6 residual centers use the same orbit BFS with higher node budgets via env.

*Session goal: drive leftoverE → 0 so the universal constructive algorithm reaches workSolved=yes on 4×4. Exact diameter still open research.*
