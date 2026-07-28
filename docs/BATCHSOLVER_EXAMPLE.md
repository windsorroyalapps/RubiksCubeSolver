# BatchSolver — concrete move sequence example

Notation: face turns only (depth 0). Same idea applies to `2R`-style slices.

---

## Input (raw reduction-style noise)

Suppose centers/edges emitted this repetitive list:

```text
R  R  U  R  R'  U  U  R  F  F  R  R  U'
```

As a vector of `(face, depth, turns)`:

```text
R1, R1, U1, R1, R-1, U1, U1, R1, F1, F1, R1, R1, U-1
```

---

## Step 1 — `compress`

Merge consecutive same `(face, depth)`; drop full turns.

| Position | Action |
|----------|--------|
| `R R` | → `R2` |
| `R2 U` | keep |
| `U R` | keep |
| `R R'` | → cancel (pop) |
| … `U U` | → `U2` |
| `F F` | → `F2` |
| `R R` | → `R2` |

**After compress:**

```text
R2  U  U2  R  F2  R2  U'
```

Wait — careful walk:

```text
R + R     → R2
R2 + U    → R2 U
U + R     → R2 U R
R + R'    → R2 U        (R and R' cancel)
U + U     → R2 U2
U2 + R    → R2 U2 R
R + F     → R2 U2 R F
F + F     → R2 U2 R F2
F2 + R    → R2 U2 R F2 R
R + R     → R2 U2 R F2 R2
R2 + U'   → R2 U2 R F2 R2 U'
```

**Compressed result:**

```text
R2  U2  R  F2  R2  U'
```

(6 moves; was 13.)

---

## Step 2 — `batchWindow` with W = 4

Partition into windows of 4:

```text
Window 0:  R2  U2  R  F2
Window 1:  R2  U'
```

Within each window, keep **first** of each unique `(face, depth, turns)`:

- Window 0: all different → `R2 U2 R F2`
- Window 1: both different → `R2 U'`

**Still:**

```text
R2  U2  R  F2  R2  U'
```

No change (no duplicates inside a window).

---

## Step 3 — example where batchWindow *does* fire

Raw (already somewhat compressed, but repetitive):

```text
R  U  R  U  R  F  R  F
```

### compress only

No consecutive same face → unchanged (8 moves).

### batchWindow(W=4)

```text
Window 0: R U R U  → first R, first U only → R U
Window 1: R F R F  → R F
```

**After batchWindow(4):**

```text
R  U  R  F
```

(4 moves.)

### then compress

No consecutive merges → **`R U R F`**

### batchWindow(W=8) on original 8-move list

One window: `R U R U R F R F`

Unique firsts: `R, U, F` →

```text
R  U  F
```

More aggressive (3 moves). Higher risk of changing real cube meaning; that’s why `optimize` uses growing windows carefully and is a **heuristic** post-pass.

---

## Full `optimize` on the second example

```text
raw:              R U R U R F R F
compress:         R U R U R F R F     (no change)
batchWindow(4):   R U R F
compress:         R U R F
batchWindow(8):   R U F              (window covers all 4; R appears once)
compress:         R U F
batchWindow(16):  R U F
batchWindow(32):  R U F
```

**Final:** `R U F`

---

## Slice-move example (nxn depth > 0)

```text
raw:  2R  2R  U  2R  2R'
```

`compress` (same face **and** same depth):

```text
2R + 2R  → 2R2
2R2 + U  → 2R2 U
U + 2R   → 2R2 U 2R
2R + 2R' → 2R2 U     (cancel)
```

**Result:** `2R2 U`

---

## Takeaway

| Pass | What the example shows |
|------|------------------------|
| `compress` | `R R`→`R2`, `R R'`→cancel, consecutive only |
| `batchWindow` | Non-adjacent duplicates inside a window collapse to first |
| `optimize` | Alternates both; can turn 8 noisy moves into 3 |

Use BoundHarness `afterBatch` vs `raw` to see the savings on real solves.
