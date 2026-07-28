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
- **Detect:** Odd count of misoriented outer U-edges (U/D color not on U/D).
- **Alg:** `2R2 B2 U2 2L U2 2R' U2 2R U2 F2 2R F2 2L' B2 2R2`

### PLL parity
- **Meaning:** Odd permutation of dedges (two edges swapped).
- **Detect:** Exactly 1 or 3 of the four U-edge side colors already match their centers.
- **Alg:** `2R2 F2 U2 2R2 R2 U2 F2 2R2`

Order: fix OLL parity first, then PLL parity, then run 3×3 stage.

---

## 3. Tighter center / edge heuristics

### Centers (`CenterSolver.cpp`)
- `centerScore(face)` = % of inner facelets already correct.
- For each attempt, **simulate** commutators at every inner depth, pick the depth with best score gain, then commit.
- Stop when score ≥ 95 or attempts exhausted.
- Order: U, D, F, B, L, R.

### Edges (`EdgePairing.cpp`)
- `pairedWings(edge)` estimates whether that dedge is already matched.
- **Skip** fully paired edges (no wasted cycles).
- Multi-depth freeslice cycles only while the edge still needs work.
- Two global passes over all 12 edges.

---

## Pipeline (n ≥ 4)

```text
CenterSolver → EdgePairing → [ParityHandler if even n] → Kociemba/CFOP
```
