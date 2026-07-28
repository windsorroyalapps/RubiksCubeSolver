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
    ${NATIVE_ROOT}/reduction/ReductionSolver.cpp
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
