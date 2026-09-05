cmake_minimum_required(VERSION 3.22.1)

project("rubikssolver")

set(NATIVE_ROOT ${CMAKE_SOURCE_DIR}/../../../../native)

add_library(
    rubikssolver
    SHARED
    native-lib.cpp
    ${NATIVE_ROOT}/common/Cube.cpp
    ${NATIVE_ROOT}/cfop/CFOPSolver.cpp
    ${NATIVE_ROOT}/cfop/Kociemba.cpp
    ${NATIVE_ROOT}/cfop/CoordCube.cpp
    ${NATIVE_ROOT}/cfop/Pruning.cpp
    ${NATIVE_ROOT}/cfop/MoveTables.cpp
    ${NATIVE_ROOT}/cfop/GodsAlgorithm.cpp
    ${NATIVE_ROOT}/reduction/ReductionSolver.cpp
    ${NATIVE_ROOT}/reduction/CenterSolver.cpp
    ${NATIVE_ROOT}/reduction/EdgePairing.cpp
    ${NATIVE_ROOT}/reduction/ParityHandler.cpp
    ${NATIVE_ROOT}/reduction/BatchSolver.cpp
    ${NATIVE_ROOT}/reduction/ClusterScheduler.cpp
    ${NATIVE_ROOT}/reduction/BatchGroups.cpp
    ${NATIVE_ROOT}/reduction/BoundHarness.cpp
    ${NATIVE_ROOT}/reduction/ReducedSearch.cpp
    ${NATIVE_ROOT}/reduction/StageCap.cpp
)

find_library(log-lib log)

target_include_directories(rubikssolver PRIVATE
    ${NATIVE_ROOT}/common
    ${NATIVE_ROOT}/cfop
    ${NATIVE_ROOT}/reduction
)

target_link_libraries(
    rubikssolver
    ${log-lib}
)
