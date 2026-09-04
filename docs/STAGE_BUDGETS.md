# Per-stage Ucas budgets (2026-09-05)

Ucas(n) is split so ReductionSolver can target *which* stage is fat, not just the sum.

```
Ucas(n) = C + E + P + K + S
C = 8(n-2)^2     extra-center cells × 8-move commutator
E = 96(n-2)      12 edges × (n-2) wings × 8-move pair
P = 20 if n even else 0
K = 20           3×3 God's number
S = 6n           slice-setup / residual overhead
```

Locked totals (unchanged): Ucas(4)=288, Ucas(5)=410, Ucas(10)=1380.

| n | C | E | P | K | S | Ucas |
|---|---|---|---|---|---|------|
| 4 | 32 | 192 | 20 | 20 | 24 | 288 |
| 5 | 72 | 288 | 0 | 20 | 30 | 410 |
| 6 | 128 | 384 | 20 | 20 | 36 | 588 |
| 7 | 200 | 480 | 0 | 20 | 42 | 742 |
| 8 | 288 | 576 | 20 | 20 | 48 | 952 |
| 9 | 392 | 672 | 0 | 20 | 54 | 1138 |
| 10 | 512 | 768 | 20 | 20 | 60 | 1380 |

`BoundReport` now emits `UcasC/UcasE/UcasP`, `overC/overE`, and `fattest=` so the next session cuts only the overflowing stage.

These are **solver budgets**, not God's number. Exact g(n) for n≥4 remains open.
