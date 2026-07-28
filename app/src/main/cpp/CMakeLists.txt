cmake_minimum_required(VERSION 3.22.1)

project("rubikssolver")

add_library(
    rubikssolver
    SHARED
    native-lib.cpp
    # Will expand with:
    # ../../../../native/common/Cube.cpp
    # ../../../../native/cfop/...
    # ../../../../native/reduction/...
)

find_library(log-lib log)

target_link_libraries(
    rubikssolver
    ${log-lib}
)

target_include_directories(rubikssolver PRIVATE
    ${CMAKE_SOURCE_DIR}/../../../../native/common
)
