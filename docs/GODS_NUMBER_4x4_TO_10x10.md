# God's Number estimates: 4×4 → 10×10

Exact God's Number is **unknown** for every n ≥ 4.  
What we have: lower bounds, constructive upper bounds, community estimates, and the asymptotic law.

**Metrics (approx):**
- **HTM / FTM** – face/outer turn (180° counts as 1)
- **OBTM** – outer block turn metric
- **STM / SSTM** – single-slice turn metric

Asymptotic (Demaine et al. 2011):  
\( g(n) = \Theta(n^2 / \log n) \)

Verified against `./artifacts/print_bounds` (Hardwick |G| + the U(n) formula in BoundHarness).

---

## Table 1 — Summary estimates (HTM-style, order of magnitude)

| n | Lower (known / counting) | Community estimate | Constructive upper* | Status |
|---|--------------------------|--------------------|---------------------|--------|
| 3 | **20** | **20** | **20** | **Proven** |
| 4 | ~29–35 (count + OBTM) | **~40–48** | **501** | Open (OBTM ≤54) |
| 5 | Hardwick L=**47** | **~55–70** | **878** | Open |
| 6 | Hardwick L=**67** | **~80–100** | **1727** | Open |
| 7 | Hardwick L=**92** | **~110–140** | **2472** | Open |
| 8 | Hardwick L=**117** | **~140–180** | **3689** | Open |
| 9 | Hardwick L=**149** | **~180–230** | **4802** | Open |
| 10 | Hardwick L=**179** | **~220–280** | **6387** | Open |

\*Constructive upper = reduction-style formula below (always solvable in at most that many moves; far from tight). Older copies of this table under-counted U(n) for n≥6; the formula in code was always the source of truth.

---

## Table 2 — 4×4 reported bounds (various metrics)

| Metric | Lower | Upper | Notes |
|--------|-------|-------|--------|
| OBTM | 35 | **54** | cubezzz / speedsolving (2015-era, tightened from 55) |
| STM / SSTM | 32 | 53 | same sources |
| Block turn | 29 | 53 | same sources |
| Older FTM-style | ≥33 | 67–82 | forum upper bounds (block / single / face) |
| Conjecture (HTM) | — | ~41 | informal conjecture |
| Conjecture (QTM) | — | ~48 | same |

Nobody has proven exact g(4).

---

## Table 3 — 5×5 reported bounds

| Metric | Lower | Upper | Notes |
|--------|-------|-------|--------|
| OBTM | — | ~130 | speedsolving upper-bound claim |
| Hardwick counting L | **47** | — | `print_bounds` STM generators |
| Conjecture (HTM) | — | ~58 | informal |
| Conjecture (QTM) | — | ~68 | informal |
| Gap | large | large | literature notes gap >2× on some bounds |

---

## Table 4 — Constructive upper bound (reduction algorithm)

From community analysis of a concrete reduction solver:

- Odd n: \( 92n^2 - 307n + 113 \)
- Even n: \( 92n^2 - 307n + 257 \)

| n | Formula value (max moves sufficient) |
|---|--------------------------------------|
| 4 | 92·16 − 307·4 + 257 = **501** |
| 5 | 92·25 − 307·5 + 113 = **878** |
| 6 | 92·36 − 307·6 + 257 = **1727** |
| 7 | 92·49 − 307·7 + 113 = **2472** |
| 8 | 92·64 − 307·8 + 257 = **3689** |
| 9 | 92·81 − 307·9 + 113 = **4802** |
| 10 | 92·100 − 307·10 + 257 = **6387** |

These are **guaranteed solvable** lengths for that algorithm family — not God's Number (which is much smaller).

---

## Table 5 — Asymptotic scale \( n^2 / \ln n \)

Raw order-of-magnitude (natural log). **Not** calibrated to equal 20 at n=3; only shows growth shape. BoundHarness uses 3.8 · n² / ln n.

| n | n² | ln n | n² / ln n | 3.8 · that |
|---|-----|------|-----------|-------------|
| 4 | 16 | 1.39 | ~11.5 | ~44 |
| 5 | 25 | 1.61 | ~15.5 | ~59 |
| 6 | 36 | 1.79 | ~20.1 | ~76 |
| 7 | 49 | 1.95 | ~25.2 | ~96 |
| 8 | 64 | 2.08 | ~30.8 | ~117 |
| 9 | 81 | 2.20 | ~36.9 | ~140 |
| 10 | 100 | 2.30 | ~43.4 | ~165 |

---

## How to read this

1. **Exact g(n) for n≥4 is open** (and computing diameter is NP-hard in related models).
2. **Best practical numbers** for 4×4 live in the **30s–50s** depending on metric (OBTM upper now 54); 5×5 likely **50s–70s** HTM-scale, with weaker published proofs.
3. **Constructive uppers** (hundreds–thousands) come from reduction algorithms like the one in this repo — correct but not optimal.
4. **True God's Number** grows like \( \Theta(n^2/\log n) \), so 10×10 is only a few times harder than 5×5 in optimal-move terms, not 4× harder like n² alone.

---

## Repo link

Our solver uses reduction (centers → edges → parity → 3×3) + Demaine-style batching, which realises a **constructive** (not optimal) upper bound for every n and approaches the asymptotic order.
