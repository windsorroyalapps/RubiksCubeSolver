# Source/dest facelet mapping (2026-10-09 two-move flip setup)

Gate for leftoverE=0. Not a diameter proof.

For an unpaired depth `d` on destination edge `e`:

1. `EdgePairing::wingFacelets(n, e, d, a, b)` records both stickers as `(face, row, col)`. Coordinates match `wingColorsAt`.
2. `locateTargetWings` scans the other 11 edges at that depth for the colour pair that belongs on `e`, and tags each hit `orient = +1` (home order) or `orient = -1` (flipped).
3. `wingOrientation` is the same classifier on a single slot: +1 home, -1 flipped, 0 wrong pair. `pairedWings` counts either non-zero orientation (a flipped-but-coloured wing is still a paired wing for the reduction; orientation is repaired by the slice setup).
4. If a source edge is found and is not solid, an outer setup is chosen from the source edge index. A flipped hit also applies `flipSliceSetup` (quarter turn of the source's second face at depth `d`) before the 8/9-move depth commutator.
5. If that single quarter does not raise `pairedWings`, a second attempt applies `twoMoveFlipSetup`: outer quarter of the source first face, then the slice quarter. Both are undone exactly if the commutator does not gain.
6. If `pairedWings(e)` does not increase, setup + flip-slice + commutator are undone exactly. Solid edges are not the insertion target.
7. The trailing no-gain `U` that previously broke already-paired wings stays removed.
8. The fallback budget alternates a flip-slice on odd wings and a 2-move setup on wings `mod 4 == 3`, both undone on no gain.

This is still a constructive search, not a closed commutator table. A single 4x4 edge has 24 wing facelets; full 3-cycle tables (12 edges x depths x flip) remain the next offline step.

Exact integer g(n) for n>=4 is still open. This mapping only tightens the constructive solver toward leftoverE=0.
