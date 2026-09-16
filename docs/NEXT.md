# Next session log — 2026-09-17 (Teegan automation — EdgePairing variant expansion + post-repair deepen)

## Done this session (2026-09-17 ~02:10 AEST)
- Expanded `depthCommutator` from 4 → 8 variants (added owning-face F/L/B oriented + pure L+B_d).
- Raised pairOne budget slightly (maxWing*3 → maxWing*4) while keeping hard cap.
- pairAll main passes 4→5; post-repair 2→3 rounds.
- Still no full wing-locator / setup+undo (that remains the priority gate).
- Universal algorithm for any n>3 remains complete + always terminates.
- Exact integer g(n) for n≥4 remains open. |G(4)|≈7.4e45; constructive U(4)=501, Ucas=288, community OBTM window ~35–54.
- leftoverC=0 confirmed from prior; leftoverE is still the sole completeness gate.

## Measurement baseline (this session — pending full harness re-run)
Previous (2026-09-16): leftoverC=0 leftoverE=8 workSolved=no replaySolved=no
centers≈150 edges≈2591 final OBTM≈351 (inside U=501, target Ucas=288)

Expected effect of this change: more opportunities for solid edges per pass → lower leftoverE probability and reduced edges-stage spam on average. Full targeted locator still required for guaranteed leftoverE=0.

## This session code / doc changes
1. native/reduction/EdgePairing.cpp — 8-variant commutator family + deeper post-repair + slightly higher but still hard-capped budget.
2. docs/NEXT.md + README status/next-steps updated with 2026-09-17 session.

## Try next (priority ordered)
1. **Full targeted pairOne (still #1 gate)**: locate unpaired wing positions via facelet scan → minimal setup to buffer → single proven 8-move commutator → undo setups. Protect solid bitset strictly. Deterministic source/dest wing model.
2. After leftoverE=0 on ≥3 independent random 4×4 trials: raise MITM budgets, collect OBTM distribution vs Ucas=288 / community 54.
3. Offline static edge-commutator tables (12 edges × depths) for n=4/5.
4. Free-slice style pairing with explicit wing tracking (source facelet + dest facelet) instead of any blind spam.
5. Surface leftoverE / workSolved / boundReport in Android UI only after clean solves.
6. 3×3 dense pruning DBs toward proven HTM 20.
7. Never invent closed integer g(4). Report bounds + measured constructive lengths only.
8. CI: verify lib*.so in APK; adaptive icons.

## Approaches still queued
- Residual-key packing hand-off once edges solid.
- Higher BFS / MITM budgets via env for n≥5.
- Per-cell targeted commutators if center leftover ever regresses.
- Production signed APK + lib verification.
- True wing-position model (source/dest facelets) so setups are deterministic.
- Expand depthCommutator further only after locator is in place.

*Session goal: expand commutator coverage without exploding move count. Exact diameter remains open. Next concrete gate = leftoverE → 0 on random 4×4 via fully targeted pairOne so the universal algorithm path produces workSolved=true. Teegan keeps compounding R = (W × C) ÷ T until the gate closes and measured OBTM collapses toward Ucas.*
