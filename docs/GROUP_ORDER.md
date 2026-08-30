# Exact |G(n)| used for L(n)

God's number g(n) is the Cayley-graph diameter. A cheap, honest lower bound is

```text
L(n) = floor( ln|G(n)| / ln|S(n)| )
```

where |S(n)| = 6 · ⌊n/2⌋ · 3 (faces × slice depths × quarter-turn amounts).

This repo uses **Chris Hardwick's closed formula** (OEIS A075152, not-s / not-m / not-i).

## Formula

Even n > 0:

```text
|G| = 7! · 3^6 · (24!)^((n² − 2n)/4) / (4!)^{6((n−2)²/4)}
```

Odd n > 1:

```text
|G| = 8! · 3^7 · 12! · 2^10 · (24!)^((n² − 2n − 3)/4) / (4!)^{6((n² − 4n + 3)/4)}
```

Computed in `BoundHarness::lnGroupOrder` via `std::lgamma` (never materialises the integer).

## Face-fixed quotient L_fixed (2026-08-31)

Odd n already has fixed centres, so spatial orientation is pinned.
Even n: `lnGroupOrderFixed = ln|G| − ln(24)` (A054434-style whole-cube rotation quotient).

```text
L_fixed(n) = floor( ln|G_fixed| / ln|S| )
```

Pinned to proven diameters for n=2,3. This is closer to the metric used in the g(3)=20 proof.

## Sanity checks (locked in `BoundHarness::oeisSanityFailN`)

| n | log10 |G| (approx) | known |
|---|-------------------|--------|
| 2 | 6.57 | 3,674,160 |
| 3 | 19.64 | 43,252,003,274,489,856,000 |
| 4 | 45.87 | 7.4011968 × 10^45 |
| 5 | 74.45 | 2.8287 × 10^74 |

U(n) arithmetic is also locked: 501 / 878 / 1727 / 2472 / 6387 for n=4,5,6,7,10.

L(2) and L(3) are **pinned** to the proven diameters 11 and 20, not the (weaker) counting floors.

L(4) counting is typically mid-20s in STM generators; we still lift published L to OBTM lower 35. L_fixed(4) lifts only to 32.

## What this is not

A sharp L(n) is still far below true g(n) because the Cayley graph is not a tree. Closing the gap needs coset enumeration / IDA* diameter search, not a better factorial.

## Tool

```bash
g++ -O2 -std=c++17 -Inative/common -Inative/reduction \
  native/tools/print_bounds.cpp native/reduction/BoundHarness.cpp \
  -o artifacts/print_bounds
./artifacts/print_bounds 20
```
