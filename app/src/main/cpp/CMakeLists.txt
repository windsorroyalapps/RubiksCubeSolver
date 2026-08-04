cmake_minimum_required(VERSION 3.22.1)

project("rubikscubesolver")

add_library(
    rubikscubesolver
    SHARED
    native-lib.cpp
    # common
    ${CMAKE_SOURCE_DIR}/../../../native/common/Cube.cpp
    # cfop / 3x3
    ${CMAKE_SOURCE_DIR}/../../../native/cfop/CFOPSolver.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/cfop/CoordCube.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/cfop/GodsAlgorithm.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/cfop/Kociemba.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/cfop/MoveTables.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/cfop/Pruning.cpp
    # reduction / nxn
    ${CMAKE_SOURCE_DIR}/../../../native/reduction/BatchGroups.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/reduction/BatchSolver.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/reduction/BoundHarness.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/reduction/CenterSolver.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/reduction/ClusterScheduler.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/reduction/EdgePairing.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/reduction/ParityHandler.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/reduction/ReducedSearch.cpp
    ${CMAKE_SOURCE_DIR}/../../../native/reduction/ReductionSolver.cpp
)

find_library(log-lib log)

target_include_directories(rubikscubesolver PRIVATE
    ${CMAKE_SOURCE_DIR}/../../../native/common
    ${CMAKE_SOURCE_DIR}/../../../native/cfop
    ${CMAKE_SOURCE_DIR}/../../../native/reduction
)

target_link_libraries(rubikscubesolver ${log-lib})

# C++17
set_target_properties(rubikscubesolver PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)
