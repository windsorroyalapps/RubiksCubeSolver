#include <jni.h>
#include <string>
#include <memory>
#include "Cube.h"

// Simple global for demo (production will use instance per session)
static std::unique_ptr<Cube> g_cube;

extern "C" {

JNIEXPORT void JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeCreate(
        JNIEnv* /*env*/,
        jobject /*thiz*/,
        jint size) {
    g_cube = std::make_unique<Cube>(static_cast<int>(size));
}

JNIEXPORT void JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeApplyMove(
        JNIEnv* /*env*/,
        jobject /*thiz*/,
        jint face,
        jint depth,
        jint turns) {
    if (!g_cube) return;
    g_cube->apply(Move{static_cast<int>(face), static_cast<int>(depth), static_cast<int>(turns)});
}

JNIEXPORT void JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeApplyNotation(
        JNIEnv* env,
        jobject /*thiz*/,
        jstring notation) {
    if (!g_cube) return;
    const char* str = env->GetStringUTFChars(notation, nullptr);
    if (str) {
        g_cube->applyNotation(std::string(str));
        env->ReleaseStringUTFChars(notation, str);
    }
}

JNIEXPORT jboolean JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeIsSolved(
        JNIEnv* /*env*/,
        jobject /*thiz*/) {
    if (!g_cube) return JNI_FALSE;
    return g_cube->isSolved() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeToString(
        JNIEnv* env,
        jobject /*thiz*/) {
    if (!g_cube) {
        return env->NewStringUTF("No cube");
    }
    std::string s = g_cube->toString();
    return env->NewStringUTF(s.c_str());
}

JNIEXPORT jint JNICALL
Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeSize(
        JNIEnv* /*env*/,
        jobject /*thiz*/) {
    if (!g_cube) return 0;
    return static_cast<jint>(g_cube->size());
}

} // extern "C"
