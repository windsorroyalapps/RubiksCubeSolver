#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_windsorroyal_rubikscubesolver_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "RubiksCubeSolver native engine ready - CFOP + Reduction pipeline online";
    return env->NewStringUTF(hello.c_str());
}
