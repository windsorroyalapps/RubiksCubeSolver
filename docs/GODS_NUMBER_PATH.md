# God's Algorithm path (target ≤ 20 HTM for all 3×3)

Proven: every 3×3 position is solvable in **at most 20** half-turn moves (Rokicki et al. 2010).

## GodsAlgorithm pipeline (what `nativeSolve` runs)

```text
1. Multi-probe Kociemba
   - Standard Kociemba solve
   - Also: for each of 12 short pre-moves, apply → solve → prepend pre-move
   - Keep the shortest total sequence

2. If length still > 20
   - Bounded optimal IDA* with depth ≤ 20 + pruning heuristic
   - Same-face move pruning

3. If still empty
   - CFOP fallback (last resort)
```

## Supporting machinery

| Component | Role |
|-----------|------|
| MoveTables | twist/flip/slice × 18 moves (1–2 move closure + 25k walks) |
| Pruning BFS | distance lower bounds for IDA* |
| Coord IDA* | phase-1 in integer space (depth 14) |
| GodsAlgorithm | multi-probe + ≤20 optimal cleanup |

## Honest limits

- Tables are **sampled/propagated**, not a full algebraic encode of every index.
- Optimal cleanup is **capped at 20** for mobile; some hard positions may still use CFOP with longer solutions.
- A Rokicki-scale exhaustive proof used ~35 CPU-years and perfect tables — not a phone cold start.

What we ship is the **real architecture** aimed at the 20-bound for all positions: multi-probe two-phase + optimal pass limited by God's Number.

## JNI

`size == 3` → `GodsAlgorithm::solveToNotation`
