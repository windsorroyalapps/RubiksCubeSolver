#!/usr/bin/env python3
"""Print asymptotic upper bound 30.5 * n**2 / log2(n) for nxn Rubik's cube.

Exact g(n) open for n>=4. This is the explicit practical upper from arXiv:2611.11018.
Asymptotic not tight for small n; see BoundHarness for constructive U(n).
"""
import math
import sys

def upper(n: int) -> float:
    if n < 2:
        return 0.0
    return 30.5 * n * n / math.log2(n)

def main():
    ns = list(range(4, 21))
    if len(sys.argv) > 1:
        ns = [int(x) for x in sys.argv[1:]]
    print("n | ~30.5 n^2 / lg n")
    print("--|---------------")
    for n in ns:
        print(f"{n:2d} | {upper(n):8.1f}")

if __name__ == "__main__":
    main()
