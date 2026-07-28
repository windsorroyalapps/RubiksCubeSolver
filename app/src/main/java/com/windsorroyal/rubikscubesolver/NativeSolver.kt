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
}
