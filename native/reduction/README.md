# Reduction Engine (nxn) — Practical God's Algorithm

```text
Centers → Edges → [Parity if even] → 3x3 → BatchSolver::optimize
```

## Demaine connection

- **Lower bound:** too many configs, O(n) moves/step ⇒ Ω(n²/log n)
- **Upper bound:** batch shared slice moves ⇒ O(n²/log n)
- **BatchSolver:** compress + window dedupe implements the parallel idea in practice

See [docs/DEMAINE_BATCHING.md](../../docs/DEMAINE_BATCHING.md).

## Files

| File | Role |
|------|------|
| CenterSolver | Multi-axis center commutators |
| EdgePairing | Freeslice pairing + skip |
| ParityHandler | Even-n OLL/PLL parity |
| BatchSolver | Shared-move collapse (n² → n²/log n spirit) |
| ReductionSolver | Orchestrator |

## Notes

- Works for 4×4 … up to device memory
- Exact g(n) still open; asymptotic Θ(n²/log n)
