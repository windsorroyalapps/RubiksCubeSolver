# Asymptotic God's Number Bounds for n×n×n (n > 3)

Exact integer God's number g(n) is known only for n=2 (11 HTM) and n=3 (20 HTM). For n≥4 it remains open due to |G(n)| ~ exp(Θ(n²)).

## Known results
- Lower bound: Ω(n² / log n) from counting argument (Demaine et al., arXiv:1106.5736). Explicit ~12.88 n² / lg n for the leading constant.
- Upper bound: O(n² / log n) via parallel batching of commuting cubie classes.
- Explicit practical upper: 30.5 n² / lg n + o(n² / lg n) (arXiv:2611.11018, Nov 2026). Within factor ~2.37 of lower for large n. Linear time, O(√N lg N) extra space. First explicit constant (prior implicit ~10^40).
- Community 4×4 OBTM: upper 54 (verified Jul 2026 AnanasClassic, public GitHub artifact); 53 / BT 51 claim (Nov 6 2026 manuscript by Jiwon Byun) unverified as of 2026-11-24.

## Approximate bounds for small n
(Note: asymptotic not tight for n≤20; constructive U(n) in BoundHarness is higher but explicit.)

| n | ~lower 12.88 | ~upper 30.5 |
|---|---------------|-------------|
| 4 | 103 | 244 |
| 5 | 139 | 328 |
| 6 | 179 | 425 |
| 7 | 225 | 532 |
| 8 | 275 | 651 |
| 9 | 329 | 779 |
| 10 | 388 | 918 |
| 20 | 1192 | 2823 |
| 50 | ~5700 | ~13500 |
| 100 | ~19400 | ~45900 |

These are upper bounds only for large n; for n=4 the community window is 35–54 OBTM.

## Practical God's algorithm in this repo
The universal constructive pipeline (reduction + Demaine-style BatchGroups + residual MITM + StageCap + facelet-mapped flip-aware edge pairing) always terminates for any n≥4 (memory permitting) and realises a concrete U(n). It follows the Θ(n² / log n) family. Next work: drive leftoverE	o0 on 4×4 to measure tighter practical lengths, then port tighter constants from arXiv:2611.11018.

See BoundHarness, docs/GODS_NUMBER_NXN.md, docs/UNIVERSAL_NXN_ALGORITHM.md.

*Updated 2026-11-24 automation session. Algorithm for any size >3 is this constructive Θ(n²/log n) solver. Exact diameter open.*
