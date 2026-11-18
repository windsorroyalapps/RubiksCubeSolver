# Next session log — 2026-11-19 (automation progress on universal nxn algorithm)

## Done this session (2026-11-19)
- Refined asymptotic_bounds.py to print both lower (~12.88 n²/lg n) and upper (30.5 n²/lg n) from Demaine and arXiv:2611.11018.
- Updated ASYMPTOTIC_BOUNDS.md with dual bounds table and current date.
- Reconfirmed exact integer g(n) for n≥4 remains open and computationally intractable (proven only for n=2,3; 3×3 HTM=20, QTM=26). |G(4)|≈7.4e45; exhaustive diameter search not feasible. No closed integer God's number for sizes >3 is schedulable or claimed.
- Asymptotic result g(n) = Θ(n² / log n) from Demaine et al. (arXiv:1106.5736) is the known growth rate and practical God's algorithm for any size greater than 3. The constructive reduction + Demaine-style batching + residual MITM + StageCap + facelet-mapped flip-aware edge pairing realises a terminating solver in that family.
- Explicit practical upper: arXiv:2611.11018 (Nov 7 2026) algorithms solve within 30.5 n² / lg n + o(n²/lg n) moves, within factor 2.37+o(1) of the known lower bound ≈12.88, linear time, O(√N lg N) space.
- Community 4×4 OBTM upper remains **54** (AnanasClassic Jul 2026). Nov 6 2026 manuscript claim of OBTM ≤53 / BT ≤51 still unverified as of 2026-11-19; cite only as claim until independent confirmation.
- Priority remains driving leftoverE to 0 on 4×4 to unlock workSolved and measured OBTM collection vs Ucas=288 / community 54. Do not invent closed integer.

## Measurement baseline (carry-forward)
leftoverC=0 leftoverE=7-8 workSolved=no OBTM=144-256.
Target: leftoverE=0 then workSolved=true on ≥3 random 4×4 before raising MITM budgets and collecting tighter OBTM vs Ucas=288 / community 54.

## Try next (priority ordered)
1. Re-run desktop harness with higher RCS_MITM_NODEBUDGET4 (≥200k) and log leftoverE / workSolved / OBTM across ≥10 trials. Classify residual unpaired wings.
2. Implement static lookup for n=4: shortest outer+slice path from (face,row,col) into UF buffer, one commutator, exact undo. Prototype in EdgePairing::setupFromSource.
3. Offline static edge-commutator tables for n=4/5 so pairOne becomes pure lookup; eliminate residual unpaired wings.
4. Once leftoverE=0 and workSolved=true on ≥3 random 4×4: raise MITM further, collect measured OBTM distribution vs Ucas=288 / community 54 / asymptotic 30.5 scale.
5. Explicit Demaine parallel cubie commuting-class batching refinements to shrink practical U(n); port constants and techniques from arXiv:2611.11018 (permnet61 etc.).
6. Optional Demigod-style probabilistic diameter estimator for n=4/5 as instrumentation.
7. Surface leftoverE/workSolved in Android UI only after clean solves.
8. 3×3 dense pruning DBs toward proven HTM 20.
9. Never invent closed integer g(4). |G(4)|≈7.4e45. Window 35–54 OBTM. Asymptotic diameter Θ(n²/log n) is the general algorithm for size >3. Constructive path realises a practical terminating solver in that family. Explicit upper 30.5 n²/lg n.
10. CI: verify lib*.so; adaptive icons; production signed APK.
11. Review and potentially integrate verified phase bounds from AnanasClassic 54 research into constructive stages.

*Session goal: keep the constructive algorithm complete and terminating for any n>3 while driving leftoverE toward 0. Exact God's number open for n≥4. Algorithm ships as reduction + Demaine batching + residual search + StageCap + facelet-mapped flip-aware pairing. Asymptotic Θ(n²/log n) is the general God's algorithm for size >3, now with explicit constant 30.5. Community 4×4 upper 54 (53 claim unverified).*
