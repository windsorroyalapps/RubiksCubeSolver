# Kociemba two-phase solver (exploration)

Herbert Kociemba's algorithm splits the 3×3 into two searches over nested subgroups.

## Group ladder

```text
G = full cube group
  ↓ Phase 1  (any of 18 HTM moves)
G1 = oriented corners + oriented edges + UD-slice edges in equator
  ↓ Phase 2  (only U,D, F2,B2,L2,R2)
{id} = solved
```

Phase 1 is short because G1 is large; phase 2 is short because the allowed move set is small.
Together they give near-optimal solutions (often ≤20 with good tables).

## Phase 1 coordinates (in our code)

| Coord | Range | Meaning |
|-------|-------|--------|
| `twist` | 0..2186 | Corner orientation (3^7) |
| `flip` | 0..2047 | Edge orientation (2^11) |
| `slice` | 0..494 | Which 4 edges sit in the UD slice (C(12,4)) |

`CoordCube::isG1()` ⇐ all three zero.

**Search:** IDA* in **integer space** via `MoveTables::twistMove/flipMove/sliceMove`, pruned by `Pruning::phase1Heuristic = max(twist,flip,slice)`.

Depth cap in repo: **16**. Multi-probe: optional U-premove if first path is long.

## Phase 2

Generators restricted so G1 is preserved:

- U, D: quarter and half turns
- F, B, L, R: **half turns only**

Search: facelet IDA* until `isSolved()`, depth cap **12**.  
(Production solvers use phase-2 coords: corner perm, edge perm, UD perm + tables.)

## Our pipeline (`Kociemba.cpp`)

```text
fromCube → phase1 coord IDA*
        → phase2 restricted IDA*
        → CFOP fallback if either phase fails
```

`GodsAlgorithm` wraps this with more probes + optional ≤20 cleanup.

## Why two phases beat plain BFS

| Approach | Branching | Depth needed |
|----------|-----------|--------------|
| Full optimal BFS | ~13 effective | up to 20 |
| Phase 1 only to G1 | ~13 | typically ≤12 |
| Phase 2 in G1 | ~10 | typically ≤18 |
| Combined | product of two short searches | near-optimal total |

## Files

| File | Role |
|------|------|
| `CoordCube.*` | Extract twist/flip/slice from facelets |
| `MoveTables.*` | Coord transitions for 18 moves |
| `Pruning.*` | BFS lower bounds |
| `Kociemba.*` | Phase 1 + 2 IDA* |
| `GodsAlgorithm.*` | Multi-probe + cleanup |

## Next upgrades

1. Exact algebraic encode/decode for all twist/flip/slice indices
2. Phase-2 coordinates + move tables (no facelet apply in P2)
3. More phase-1 probes (axis starts)
4. Offline perfect pruning DBs
