# Cluster scheduling (Demaine parallel setup)

## Idea

Demaine upper bound: one slice move affects many cubies.  
If we **order** work so clusters that need the same move are adjacent, one turn serves a whole group.

## `ClusterScheduler`

| API | Role |
|-----|------|
| `unsolvedCenters` | List inner facelets not matching their face color |
| `needFor` | Preferred `(face, depth, turns)` for that cluster |
| `schedule` | Sort by preferred move key (then priority) |
| `batchGroups` | Split ordered list into runs of identical preferred moves |

### Preferred move heuristic

- Depth from how far the cell sits from the rim
- Slice on **opposite** face (classic center gather)
- Priority: U/D first, then belt; deeper cells slightly preferred

## Use in `CenterSolver::solve`

```text
Phase A (x3 passes):
  schedule(work) → batchGroups
  for each group: apply ONE commutator (shared preferred move)

Phase B:
  score-guided solveFace(U,D,F,B,L,R) cleanup
```

Then `BatchSolver::optimize` still post-processes the full reduction sequence.

## Pipeline

```text
ClusterScheduler → CenterSolver → Edges → Parity → 3x3 → BatchSolver
```

Scheduling is the *planning* half of Demaine parallelism; batching is the *collapse* half.
