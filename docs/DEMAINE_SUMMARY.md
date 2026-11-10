# Demaine et al. (arXiv:1106.5736) Summary — Asymptotic God's Algorithm for n×n×n, n>3

## Key Result
God's number g(n) = Θ(n² / log n) for the n×n×n Rubik's Cube (and n×n×1 variant).

- **Upper bound O(n² / log n)**: Parallelize standard O(n²) folk algorithms. Each slice move transforms n^Θ(1) cubies. Partition cubies into O(n² / log n) commuting classes; solve each class with O(log n) parallel moves.
- **Lower bound Ω(n² / log n)**: Counting argument. |G(n)| = 2^Θ(n²) configurations, O(n) generators per position → average-case diameter at least that order. Matches upper asymptotically.

This is the practical God's algorithm for any size greater than 3. Exact integer diameter remains open for n≥4 because |G(4)| ≈ 7.4×10^45 makes exhaustive search impossible.

## Relation to this repo
The constructive pipeline (reduction + BatchGroups + residual MITM + StageCap + facelet-mapped edge pairing) realises a terminating solver in this family. U(n) is a concrete polynomial upper (currently ~92n²); Ucas is a tighter piece-budget target. Measured OBTM on 4×4 is currently 144-256 vs community 54 and Ucas 288. Driving leftoverE	o0 then collecting tighter measured lengths is the path to close the practical gap while the asymptotic remains the proven growth rate.

## Next refinements
- Explicit commuting-class partitioning to better realise the log-factor in practice.
- Static edge tables for n=4 to force leftoverE=0.
- Demigod-style sampling for probabilistic diameter estimates (not exact g(n)).
- Phase-bound certificates from the verified 4×4 OBTM≤54 computer-assisted result.

See arXiv:1106.5736 and docs/GODS_NUMBER_NXN.md.
