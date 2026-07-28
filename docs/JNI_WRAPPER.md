# JNI wrapper implementation details

## Stack

```text
Kotlin NativeSolver (object)
    ↓ System.loadLibrary("rubikssolver")
JNI  native-lib.cpp   (extern "C" Java_com_windsorroyal_...)
    ↓
C++  Cube / GodsAlgorithm / ReductionSolver / BoundHarness
```

CMake target name **`rubikssolver`** must match `loadLibrary("rubikssolver")`.

## Name mangling

Each `external fun` in `NativeSolver.kt` maps to:

```text
Java_com_windsorroyal_rubikscubesolver_NativeSolver_<methodName>
```

| Kotlin | C++ JNI symbol |
|--------|----------------|
| `nativeCreate` | `Java_com_windsorroyal_rubikscubesolver_NativeSolver_nativeCreate` |
| `nativeSolve` | `..._nativeSolve` |
| `nativeBoundReport` | `..._nativeBoundReport` |
| `nativeConstructiveUpper` | `..._nativeConstructiveUpper` |

Package is **flat** (`com.windsorroyal.rubikscubesolver`); dots become underscores.

## Global cube handle

```cpp
static std::unique_ptr<Cube> g_cube;
```

- `nativeCreate(size)` replaces `g_cube`.
- All other natives no-op or return defaults if `g_cube` is null.
- Single-cube session model (one active cube per process / classloader).

## Method details

### Lifecycle / state

| JNI | Args | Behavior |
|-----|------|----------|
| `nativeCreate` | `jint size` | `g_cube = make_unique<Cube>(size)` |
| `nativeApplyMove` | face, depth, turns | `g_cube->apply(Move{...})` |
| `nativeApplyNotation` | `jstring` | UTF chars → `applyNotation` → ReleaseStringUTFChars |
| `nativeIsSolved` | — | `jboolean` |
| `nativeToString` | — | facelet dump as `jstring` |
| `nativeSize` | — | `jint` |

### Solve

```cpp
if (size == 3)  GodsAlgorithm::solveToNotation(*g_cube);
else            ReductionSolver::solveToNotation(*g_cube);
// then apply solution notation back onto g_cube
```

Returns solution string (may be empty on failure).

### Bound harness

| JNI | Behavior |
|-----|----------|
| `nativeBoundReport` | If n≥4: `ReductionSolver::lastBoundReportString()` after last solve. If n<4: still builds a report with empty stages + U(n). |
| `nativeConstructiveUpper` | `BoundHarness::constructiveUpper(n)` → 501,878,… for n=4..10 |

## String ownership

- **Input:** `GetStringUTFChars` / `ReleaseStringUTFChars` (notation).
- **Output:** `NewStringUTF(c_str())` — JVM copies; C++ temporaries may die after return.

## Kotlin usage example

```kotlin
NativeSolver.create(5)
NativeSolver.applyNotation("R U R' U' ...")  // or scramble helpers
val sol = NativeSolver.solve()
val report = NativeSolver.boundReport()
// e.g. "n=5 centers=.. final=.. U(n)=878 withinU=yes ..."
val u4 = NativeSolver.constructiveUpper(4)   // 501
```

## Threading note

`g_cube` is not synchronized. Call JNI from one thread (main / single solver worker) unless you add a mutex.

## Files

| Path | Role |
|------|------|
| `app/src/main/java/.../NativeSolver.kt` | `external` decls + Kotlin API |
| `app/src/main/cpp/native-lib.cpp` | JNI implementations |
| `app/src/main/cpp/CMakeLists.txt` | builds `librubikssolver.so` |
