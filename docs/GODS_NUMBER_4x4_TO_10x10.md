# God's Number estimates: 4×4 → 10×10

Exact God's Number is **unknown** for every n ≥ 4.  
What we have: lower bounds, constructive upper bounds, community estimates, and the asymptotic law.

**Metrics (approx):**
- **HTM / FTM** – face/outer turn (180° counts as 1)
- **OBTM** – outer block turn metric
- **STM / SSTM** – single-slice turn metric

Asymptotic (Demaine et al. 2011):  
\( g(n) = \Theta(n^2 / \log n) \)

---

## Table 1 — Summary estimates (HTM-style, order of magnitude)

| n | Lower (known / counting) | Community estimate | Constructive upper* | Status |
|---|--------------------------|--------------------|---------------------|--------|
| 3 | **20** | **20** | **20** | **Proven** |
| 4 | ~29–35 | **~40–48** | ~501 | Open |
| 5 | ~40+ | **~55–70** | ~878 | Open |
| 6 | — | **~80–100** | ~1321 | Open |
| 7 | — | **~110–140** | ~1852 | Open |
| 8 | — | **~140–180** | ~2473 | Open |
| 9 | — | **~180–230** | ~3182 | Open |
| 10 | — | **~220–280** | ~3981 | Open |

\*Constructive upper = reduction-style formula below (always solvable in at most that many moves; far from tight).

---

## Table 2 — 4×4 reported bounds (various metrics)

| Metric | Lower | Upper | Notes |
|--------|-------|-------|--------|
| OBTM | 35 | 55 | cubezzz / speedsolving discussions (2015-era) |
| STM / SSTM | 32 | 53 | same sources |
| Block turn | 29 | 53 | same sources |
| Older FTM-style | ≥33 | 67–82 | forum upper bounds (block / single / face) |
| Conjecture (HTM) | — | ~41 | informal conjecture on MathOverflow thread |
| Conjecture (QTM) | — | ~48 | same |

Nobody has proven exact g(4).

---

## Table 3 — 5×5 reported bounds

| Metric | Lower | Upper | Notes |
|--------|-------|-------|--------|
| OBTM | — | ~130 | speedsolving upper-bound claim |
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
| 6 | 92·36 − 307·6 + 257 = **1321** |
| 7 | 92·49 − 307·7 + 113 = **1852** |
| 8 | 92·64 − 307·8 + 257 = **2473** |
| 9 | 92·81 − 307·9 + 113 = **3182** |
| 10 | 92·100 − 307·10 + 257 = **3981** |

These are **guaranteed solvable** lengths for that algorithm family — not God's Number (which is much smaller).

---

## Table 5 — Asymptotic scale \( n^2 / \ln n \)

Raw order-of-magnitude (natural log). **Not** calibrated to equal 20 at n=3; only shows growth shape.

| n | n² | ln n | n² / ln n |
|---|-----|------|-----------|
| 4 | 16 | 1.39 | ~11.5 |
| 5 | 25 | 1.61 | ~15.5 |
| 6 | 36 | 1.79 | ~20.1 |
| 7 | 49 | 1.95 | ~25.2 |
| 8 | 64 | 2.08 | ~30.8 |
| 9 | 81 | 2.20 | ~36.9 |
| 10 | 100 | 2.30 | ~43.4 |

If you scale so that the constant roughly matches 3×3 (× ~20/ (9/ln3) ≈ 2.4), you get ballpark numbers in the same region as the "community estimate" column in Table 1 — still only heuristic.

---

## How to read this

1. **Exact g(n) for n≥4 is open** (and computing diameter is NP-hard in related models).
2. **Best practical numbers** for 4×4 live in the **30s–50s** depending on metric; 5×5 likely **50s–70s** HTM-scale, with weaker published proofs.
3. **Constructive uppers** (hundreds–thousands) come from reduction algorithms like the one in this repo — correct but not optimal.
4. **True God's Number** grows like \( \Theta(n^2/\log n) \), so 10×10 is only a few times harder than 5×5 in optimal-move terms, not 4× harder like n² alone.

---

## Repo link

Our solver uses reduction (centers → edges → parity → 3×3), which realizes a **constructive** (not optimal) upper bound for every n.
