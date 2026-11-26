#!/usr/bin/env python3
"""Print asymptotic bounds for nxn Rubik's cube God's number.

Exact g(n) open for n>=4. Upper 30.5 n^2 / lg n from arXiv:2611.11018.
Lower ~12.88 n^2 / lg n from counting (Demaine et al.).
Asymptotic not tight for small n; see BoundHarness for constructive U(n).
Updated 2026-11-27: reconfirmed constants; exact integer remains open.
"""
import math
import sys

def upper(n: int) -> float:
    if n < 2:
        return 0.0
    return 30.5 * n * n / math.log2(n)

def lower(n: int) -> float:
    if n < 2:
        return 0.0
    return 12.88 * n * n / math.log2(n)

def main():
    ns = list(range(4, 21))
    if len(sys.argv) > 1:
        ns = [int(x) for x in sys.argv[1:]]
    print("n | ~lower 12.88 | ~upper 30.5 | note")
    print("--|---------------|---------------|------")
    for n in ns:
        print(f"{n:2d} | {lower(n):8.1f} | {upper(n):8.1f} | asymptotic")

if __name__ == "__main__":
    main()
