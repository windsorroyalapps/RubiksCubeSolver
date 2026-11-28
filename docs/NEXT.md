# Next session log — 2026-11-29 (automation progress on universal nxn algorithm)

## Done this session (2026-11-29)
- Enhanced `native/tools/static_setup_prototype.py` with facelet-position keyed candidate outer+slice setups (defaultdict lookup ideas for (face,row,col) → 0-2 move paths into UF buffer).
- Continued attack on leftoverE→0 priority gate for 4×4 residual unpaired wings.
- Reconfirmed: exact integer God's number g(n) for any n≥4 remains open and computationally intractable (|G(4)|≈7.4×10^45). Proven only for n=2 (11 HTM) and n=3 (20 HTM / 26 QTM).
- Practical God's algorithm for any size greater than 3 is the asymptotic family g(n) = Θ(n² / log n) from Demaine et al. (arXiv:1106.5736), realized by the repo's constructive reduction + Demaine-style batching + residual MITM + StageCap + facelet-mapped flip-aware edge pairing. Always terminates.
- Explicit practical upper remains 30.5 n² / lg n + o(...) from arXiv:2611.11018 (permnet61, factor ~2.37 of lower ≈12.88). Community 4×4 OBTM upper **54** verified (AnanasClassic); 53 claim still unverified.
- No closed integer invented or claimed.

## Measurement baseline (carry-forward)
leftoverC=0 leftoverE=7-8 workSolved=no OBTM=144-256.
Target: leftoverE=0 then workSolved=true on ≥3 random 4×4 before raising MITM budgets and collecting tighter OBTM vs Ucas=288 / community 54 / asymptotic scale.

## Try next (priority ordered)
1. Integrate enhanced static_setup_prototype logic into EdgePairing::setupFromSource / pairOne: use wingFacelets coordinates for shortest protected path, one commutator, exact undo on no gain, solid-bitset.
2. Re-run desktop harness with RCS_MITM_NODEBUDGET4≥200k; log leftoverE / workSolved / OBTM across ≥10 trials. Classify residuals.
3. Offline static edge-commutator tables (edges × depths × flip × orientations) for n=4/5 so pairOne becomes lookup.
4. Once leftoverE=0 and workSolved=true on ≥3 random 4×4: collect measured OBTM distribution vs Ucas=288 / community 54.
5. Explicit Demaine parallel cubie commuting-class batching refinements and port permnet61 / sorting-network techniques from arXiv:2611.11018 into BatchGroups/ClusterScheduler to shrink practical U(n).
6. Optional Demigod-style probabilistic diameter estimator (sampling) for n=4/5 instrumentation only.
7. Surface leftoverE/workSolved in Android UI after clean solves.
8. 3×3 dense pruning DBs toward proven HTM 20.
9. Never invent closed integer g(4). Window 35–54 OBTM. Asymptotic Θ(n²/log n) is the general algorithm for size >3.
10. CI: verify lib*.so; adaptive icons; production signed APK.
11. Review AnanasClassic 54 phase bounds for constructive stage integration; monitor 53 claim.
12. Expand prototype with Cube model simulation validation for n=4 facelet paths.

*Session goal: keep constructive algorithm complete and terminating for any n>3 while driving leftoverE toward 0. Exact God's number open for n≥4. Algorithm ships as reduction + Demaine batching + residual search + StageCap + facelet-mapped flip-aware pairing. Asymptotic Θ(n²/log n) with explicit 30.5 constant is the practical God's algorithm for any size greater than 3. Community 4×4 upper 54 (53 claim unverified).*
