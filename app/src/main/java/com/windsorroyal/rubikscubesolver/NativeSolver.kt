package com.windsorroyal.rubikscubesolver

/**
 * Thin Kotlin wrapper over the native C++ Cube engine.
 * Supports arbitrary n (3x3 up to large sizes limited by device memory).
 */
object NativeSolver {

    init {
        System.loadLibrary("rubikssolver")
    }

    external fun nativeCreate(size: Int)
    external fun nativeApplyMove(face: Int, depth: Int, turns: Int)
    external fun nativeApplyNotation(notation: String)
    external fun nativeIsSolved(): Boolean
    external fun nativeToString(): String
    external fun nativeSize(): Int

    // Convenience
    fun create(size: Int = 3) = nativeCreate(size)

    fun apply(face: Int, depth: Int = 0, turns: Int = 1) =
        nativeApplyMove(face, depth, turns)

    fun applyNotation(notation: String) = nativeApplyNotation(notation)

    fun isSolved(): Boolean = nativeIsSolved()

    fun dump(): String = nativeToString()

    fun size(): Int = nativeSize()
}
