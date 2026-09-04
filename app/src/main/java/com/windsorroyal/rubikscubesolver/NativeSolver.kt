package com.windsorroyal.rubikscubesolver

/**
 * Kotlin ↔ JNI ↔ C++ bridge for the Rubik solver.
 *
 * Native library: librubikssolver.so (CMake target "rubikssolver")
 * JNI source: app/src/main/cpp/native-lib.cpp
 *
 * Naming rule: Java_com_windsorroyal_rubikscubesolver_NativeSolver_<method>
 * must match each `external fun native...` below.
 */
object NativeSolver {

    init {
        System.loadLibrary("rubikssolver")
    }

    // ---- JNI externals (implemented in native-lib.cpp) ----

    external fun nativeCreate(size: Int)
    external fun nativeApplyMove(face: Int, depth: Int, turns: Int)
    external fun nativeApplyNotation(notation: String)
    external fun nativeIsSolved(): Boolean
    external fun nativeToString(): String
    external fun nativeSize(): Int
    external fun nativeSolve(): String

    /** Last BoundHarness report after an nxn solve (stage lengths vs U(n)). */
    external fun nativeBoundReport(): String

    /** Constructive upper bound U(n) for working backward toward God's Number. */
    external fun nativeConstructiveUpper(n: Int): Int

    /** Piece-budget cascade family Ucas(n). Solver target, not a diameter. */
    external fun nativeConstructiveUpperCascade(n: Int): Int

    /** Hardwick + community counting lower L(n). */
    external fun nativeCountingLower(n: Int): Int

    /** Face-fixed counting lower L_fixed(n). */
    external fun nativeCountingLowerFixed(n: Int): Int

    /** 2026-08-23: set residual MITM nodeBudget + depthCap for n=4 or n=5. */
    external fun nativeSetMitmBudget(n: Int, nodeBudget: Long, depthCap: Int)
    external fun nativeGetMitmNodeBudget(n: Int): Long
    external fun nativeGetMitmDepthCap(n: Int): Int

    // ---- Kotlin convenience API ----

    fun create(size: Int = 3) = nativeCreate(size)

    fun apply(face: Int, depth: Int = 0, turns: Int = 1) =
        nativeApplyMove(face, depth, turns)

    fun applyNotation(notation: String) = nativeApplyNotation(notation)

    fun isSolved(): Boolean = nativeIsSolved()

    fun dump(): String = nativeToString()

    fun size(): Int = nativeSize()

    /** 3x3 → GodsAlgorithm; n>3 → ReductionSolver (+ BoundHarness). */
    fun solve(): String = nativeSolve()

    /** Bound harness string; call after [solve] on nxn. */
    fun boundReport(): String = nativeBoundReport()

    fun constructiveUpper(n: Int): Int = nativeConstructiveUpper(n)
    fun constructiveUpperCascade(n: Int): Int = nativeConstructiveUpperCascade(n)
    fun countingLower(n: Int): Int = nativeCountingLower(n)
    fun countingLowerFixed(n: Int): Int = nativeCountingLowerFixed(n)

    /**
     * Raise (or lower) residual MITM budgets for n=4 or n=5.
     * Mobile defaults: 4x4 → 100k nodes / depth 24; 5x5 → 50k / 18.
     * Desktop stress tests can push 80k–200k+ and deeper half-depth.
     */
    fun setMitmBudget(n: Int, nodeBudget: Long, depthCap: Int) =
        nativeSetMitmBudget(n, nodeBudget, depthCap)

    fun getMitmNodeBudget(n: Int): Long = nativeGetMitmNodeBudget(n)
    fun getMitmDepthCap(n: Int): Int = nativeGetMitmDepthCap(n)
}
