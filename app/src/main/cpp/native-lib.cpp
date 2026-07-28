#include <jni.h>
#include <string>
#include <memory>
#include "Cube.h"
#include "CFOPSolver.h"
#include "Kociemba.h"
#include "ReductionSolver.h"

static std::unique_ptr<Cube> g_cube;

extern "C" {

JNIEXPORT void JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeCreate(
        JNIEnv*, jobject, jint size) {
    g_cube = std::make_unique<Cube>(static_cast<int>(size));
}

JNIEXPORT void JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeApplyMove(
        JNIEnv*, jobject, jint face, jint depth, jint turns) {
    if (!g_cube) return;
    g_cube->apply(Move{static_cast<int>(face), static_cast<int>(depth), static_cast<int>(turns)});
}

JNIEXPORT void JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeApplyNotation(
        JNIEnv* env, jobject, jstring notation) {
    if (!g_cube) return;
    const char* str = env->GetStringUTFChars(notation, nullptr);
    if (str) {
        g_cube->applyNotation(std::string(str));
        env->ReleaseStringUTFChars(notation, str);
    }
}

JNIEXPORT jboolean JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeIsSolved(
        JNIEnv*, jobject) {
    if (!g_cube) return JNI_FALSE;
    return g_cube->isSolved() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeToString(
        JNIEnv* env, jobject) {
    if (!g_cube) return env->NewStringUTF("No cube");
    return env->NewStringUTF(g_cube->toString().c_str());
}

JNIEXPORT jint JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeSize(
        JNIEnv*, jobject) {
    if (!g_cube) return 0;
    return static_cast<jint>(g_cube->size());
}

JNIEXPORT jstring JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeSolve(
        JNIEnv* env, jobject) {
    if (!g_cube) return env->NewStringUTF("");

    std::string notation;
    if (g_cube->size() == 3) {
        // Prefer Kociemba path (falls back to CFOP until tables are ready)
        notation = Kociemba::solveToNotation(*g_cube);
    } else {
        notation = ReductionSolver::solveToNotation(*g_cube);
    }

    if (!notation.empty()) {
        g_cube->applyNotation(notation);
    }
    return env->NewStringUTF(notation.c_str());
}

} // extern "C"
