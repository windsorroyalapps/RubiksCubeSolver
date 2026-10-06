# Source/dest facelet mapping (2026-10-07)

Gate for leftoverE=0. Not a diameter proof.

For an unpaired depth `d` on destination edge `e`:

1. `EdgePairing::wingFacelets(n, e, d, a, b)` records both stickers as `(face, row, col)`. Coordinates match `wingColorsAt`.
2. `locateTargetWings` scans the other 11 edges at that depth for the colour pair that belongs on `e`.
3. If a source edge is found and is not solid, an outer setup is chosen from the source edge index (U/D/L/R/F/B quarter or half), then an 8/9-move depth commutator runs at `d`.
4. If `pairedWings(e)` does not increase, the sequence is undone exactly. Solid edges are not the insertion target.
5. The trailing no-gain `U` that previously broke already-paired wings was removed.

This is still a constructive search, not a closed commutator table. A single 4x4 edge has 24 wing facelets; full 3-cycle tables remain a later offline step.

Exact integer g(n) for n>=4 is still open. This mapping only tightens the constructive solver toward leftoverE=0.
