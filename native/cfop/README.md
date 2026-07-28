# CFOP Engine

3×3 beginner-method solver (layer-by-layer) that is the foundation for full CFOP.

## Current implementation

1. **White Cross** – search + insert white edges onto D face
2. **White Corners** – sexy-move (`R U R' U'`) insertion cycles
3. **Middle Edges** – left/right insertion algs
4. **Yellow Cross** – `F R U R' U' F'`
5. **OLL corners** – Sune
6. **PLL corners** – A-perm style
7. **PLL edges** – U-perm

## Files
- `CFOPSolver.h` / `CFOPSolver.cpp`

## Next upgrades
- Pattern recognition for OLL (57) and PLL (21)
- Proper F2L pair matching instead of pure insertion cycles
- Kociemba two-phase for optimal solutions
