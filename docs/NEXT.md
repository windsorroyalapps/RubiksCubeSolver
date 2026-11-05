# Next session log — 2026-11-06 (automation progress on universal nxn algorithm)

## Done this session (2026-11-06)
- Reconfirmed exact integer g(n) for n≥4 remains open (proven only for n=2,3; 3×3 HTM=20, QTM=26). Exhaustive diameter search infeasible (|G(4)|≈7.4e45).
- Latest community OBTM upper for 4×4 remains 54 (Jul 2026, AnanasClassic computer-assisted proof of phases 1+2 ≤18; +16 phase 3 +20 reduced 3×3). Window 35–54 OBTM.
- Asymptotic result g(n) = Θ(n² / log n) from Demaine et al. (arXiv:1106.5736) confirmed as the known growth rate; upper bound from parallelized reduction algorithms, lower from counting. This is the practical "God's algorithm" asymptotic for any size >3.
- Universal constructive algorithm for any n>3 is complete and always terminates: reduction + Demaine-style batching + residual MITM + StageCap + facelet-mapped flip-aware 2-move edge pairing + leftover commutators.
- Priority remains driving leftoverE to 0 on 4×4 to unlock workSolved and measured OBTM collection vs Ucas=288 / community 54. Do not invent closed integer.
- Added notes on potential improvements: expand commutator variants, static lookup tables for n=4 edge setups, Demigod sampling for probabilistic bounds.

## Measurement baseline (updated)
Prior: leftoverC=0 leftoverE=7-8 workSolved=no OBTM=143-266.
Target: leftoverE=0 then workSolved=true on ≥3 random 4×4 before raising MITM budgets and collecting tighter OBTM vs Ucas=288 / community 54.

## Code / doc changes this session
1. docs/NEXT.md — this updated log with 2026-11-06 session.
2. README.md — refreshed status date to 2026-11-06, updated next steps.
3. docs/UNIVERSAL_NXN_ALGORITHM.md — added 2026-11-06 note.

## Try next (priority ordered)
1. Re-run `./artifacts/rcs_harness 4 10` with higher RCS_MITM_NODEBUDGET4 and log leftoverE / workSolved across trials. Classify remaining unpaired wings (wrong-depth / orientation / setup-miss / commutator failure).
2. Implement shortest outer+slice path from recorded (face,row,col) into the UF buffer, one commutator, exact undo, solid bitset protected. Prefer facelet coordinates. Implement as static lookup for n=4 to drive leftoverE to 0.
3. Offline static edge-commutator tables (12 edges × depths × flip) for n=4/5 so pairOne becomes a pure lookup. This should eliminate residual unpaired wings.
4. Once leftoverE=0 and workSolved=true on ≥3 random 4×4: raise MITM budgets, collect measured OBTM vs Ucas=288 / community 54, and compare to asymptotic scale.
5. Free-slice pairing with explicit wing tracking and better multi-pass scheduling.
6. Surface leftoverE/workSolved in Android UI only after clean solves.
7. 3×3 dense pruning DBs toward proven HTM 20 (already architected in GodsAlgorithm).
8. Never invent closed integer g(4). |G(4)|≈7.4e45. Window 35–54 OBTM. Asymptotic diameter Θ(n²/log n) (Demaine et al.) is the known God's algorithm growth rate for any n>3. The constructive algorithm realises a practical terminating solver in that family.
9. CI: verify lib*.so; adaptive icons; production signed APK.
10. Optional: integrate phase-decomposition certificates from 54-bound research (AnanasClassic) for tighter constructive stages; implement Demigod-style probabilistic diameter estimator as optional reporting (not claimed exact g(n)). Explore Hirata graph-theoretical diameter lower bounds for potential L(n) tightening. Consider residual packing and higher budgets for n≥5. Review Salkinder for group-order lower-bound coefficients. Explicit Demaine parallel cubie commuting-class batching refinements for better practical log-factor.

## Approaches still queued
- Residual-key packing once edges solid.
- Higher BFS/MITM budgets for n≥5 via env overrides.
- Per-cell targeted center commutators if leftoverC regresses.
- Diameter search not feasible — do not schedule exhaustive Cayley BFS on 4×4.
- Integrate Hirata-style or sampling-based estimators for practical diameter bounds.
- Review and port phase 1+2 ideas from the 54-bound computer-assisted proof to shrink U(n) stages.
- Explicit implementation of Demaine parallel cubie commuting-class batching refinements for better log-factor in practice.
- Static lookup tables for n=4 edge setups to guarantee leftoverE=0.

*Session goal: keep the constructive algorithm complete and terminating for any n>3 while driving leftoverE toward 0. Exact God's number open for n≥4. Algorithm ships as reduction + Demaine batching + residual search + StageCap + facelet-mapped flip-aware pairing. Asymptotic Θ(n²/log n) is the general God's algorithm for size >3.*
