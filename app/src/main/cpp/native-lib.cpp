#include <jni.h>
#include <string>
#include <memory>
#include "Cube.h"
#include "GodsAlgorithm.h"
#include "ReductionSolver.h"
#include "BoundHarness.h"
#include "ReducedSearch.h"

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
        notation = GodsAlgorithm::solveToNotation(*g_cube);
    } else {
        notation = ReductionSolver::solveToNotation(*g_cube);
    }

    if (!notation.empty()) {
        g_cube->applyNotation(notation);
    }
    return env->NewStringUTF(notation.c_str());
}

JNIEXPORT jstring JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeBoundReport(
        JNIEnv* env, jobject) {
    if (!g_cube || g_cube->size() < 4) {
        // Still useful: show static U(n) table row for current size if any
        if (g_cube) {
            int n = g_cube->size();
            StageLengths empty{};
            auto r = BoundHarness::report(n, empty);
            return env->NewStringUTF(r.toString().c_str());
        }
        return env->NewStringUTF("");
    }
    return env->NewStringUTF(ReductionSolver::lastBoundReportString().c_str());
}

JNIEXPORT jint JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeConstructiveUpper(
        JNIEnv*, jobject, jint n) {
    return static_cast<jint>(BoundHarness::constructiveUpper(static_cast<int>(n)));
}

// 2026-08-23: expose MITM budgets so desktop / stress tests can raise nodeBudget + depthCap
JNIEXPORT void JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeSetMitmBudget(
        JNIEnv*, jobject, jint n, jlong nodeBudget, jint depthCap) {
    ReducedSearch::setNodeBudget(static_cast<int>(n), static_cast<size_t>(nodeBudget));
    ReducedSearch::setDepthCap(static_cast<int>(n), static_cast<int>(depthCap));
}

JNIEXPORT jlong JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeGetMitmNodeBudget(
        JNIEnv*, jobject, jint n) {
    return static_cast<jlong>(ReducedSearch::getNodeBudget(static_cast<int>(n)));
}

JNIEXPORT jint JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeGetMitmDepthCap(
        JNIEnv*, jobject, jint n) {
    return static_cast<jint>(ReducedSearch::getDepthCap(static_cast<int>(n)));
}

} // extern "C"
