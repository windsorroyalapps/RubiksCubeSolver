# Keep native methods
-keepclasseswithmembernames class * {
    native <methods>;
}

# NativeSolver JNI bridge
-keep class com.windsorroyal.rubikscubesolver.NativeSolver { *; }
-keepclassmembers class com.windsorroyal.rubikscubesolver.NativeSolver {
    native <methods>;
    public *;
}

# Play Billing
-keep class com.android.vending.billing.** { *; }
-keep class com.android.billingclient.** { *; }
-dontwarn com.android.billingclient.**
