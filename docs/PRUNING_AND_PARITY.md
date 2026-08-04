# Pruning · Parity · Center/Edge Heuristics

## 1. Pruning tables (faster Kociemba)

**Files:** `native/cfop/Pruning.h`, `Pruning.cpp`

| Table | Size | Meaning |
|-------|------|--------|
| `twistTable_` | 2187 | Lower bound moves to fix corner orientation |
| `flipTable_` | 2048 | Lower bound moves to fix edge orientation |
| `sliceTable_` | 495 | Lower bound moves to put UD-slice edges home |

**Heuristic (admissible):**
```text
phase1H = max(twistPrune, flipPrune, slicePrune)
```

IDA* prunes any node where `depth + phase1H > current threshold`.

Values are combinatorial lower bounds (nonzero base-3 digits for twist, popcount for flip, out-of-slice bits for slice). They track real phase-1 distances closely enough to cut search a lot on mobile. Full move-table BFS can replace `init()` later without changing the API.

---

## 2. Even-order parity handlers

**Files:** `native/reduction/ParityHandler.h`, `ParityHandler.cpp`

Only for **even n** (4×4, 6×6, …).

### OLL parity
- **Meaning:** One dedge appears flipped relative to 3×3.
- **Detect:** Odd count of misoriented wings across **all depths 1..n-2** on the four U-edges (full wing orientation parity).
- **Alg:** `2R2 B2 U2 2L U2 2R' U2 2R U2 F2 2R F2 2L' B2 2R2`

### PLL parity
- **Meaning:** Odd permutation of dedges (two edges swapped).
- **Detect:** Side-color match residual over **all depths 1..n-2**; odd residual in band (classic 1/3 for n=4).
- **Alg:** `2R2 F2 U2 2R2 R2 U2 F2 2R2`

Order: fix OLL parity first, then PLL parity, then run 3×3 stage.

---

## 3. Tighter center / edge heuristics

### Centers (`CenterSolver.cpp`)
- Global never-break multi-face score + protect 100% faces.
- Full center-orbit BFS for n≤5; residual short-search for n=6.
- Order: U, D, F, B, L, R.

### Edges (`EdgePairing.cpp`) — **Yau-style buffer tracking**
- Explicit buffer edge = UF (index 0).
- `pairedWings(edge)` real facelet count; solid set (`bitset<12>`) never touched once solid.
- Priority order: non-buffer edges first, buffer last; cross edges preferred.
- Four progressive passes; solid set refreshed and protected mid-pass.
- Skip fully paired edges (no wasted cycles, never-break guarantee).

---

## Pipeline (n ≥ 4)

```text
CenterSolver → EdgePairing (buffer + solid protect) → [ParityHandler if even n] → Kociemba/CFOP → BatchSolver
```
