# CMakeLists.txt for Panelz2
# Author: Adam O'Brien
# Updated to use Eigen, absl::flags, and GoogleTest

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(Panelz2 VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─── Output paths ─────────────────────────────────────────────────────────────

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# ─── Compiler flags ───────────────────────────────────────────────────────────

if(MSVC)
    add_compile_options(/W3 /O2)
else()
    add_compile_options(-Wall -Wextra -O2 -Wno-reorder -Wsign-compare)
endif()

# ─── FetchContent dependencies ────────────────────────────────────────────────

include(FetchContent)

# Eigen (header-only linear algebra)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG        3.4.0
    GIT_SHALLOW    TRUE
)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(eigen)

# Abseil (absl::flags for CLI parsing)
FetchContent_Declare(
    abseil-cpp
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG        20240722.0
    GIT_SHALLOW    TRUE
)
set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_ENABLE_INSTALL     OFF)
FetchContent_MakeAvailable(abseil-cpp)

# GoogleTest (unit tests)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
    GIT_SHALLOW    TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ─── Include directories ──────────────────────────────────────────────────────

include_directories(
    src/Geometry
    src/LinearAlgebra
    src/Airfoil
    src/Solver
)

# ─── Libraries ────────────────────────────────────────────────────────────────

add_subdirectory(src)

# ─── Executables ──────────────────────────────────────────────────────────────

add_subdirectory(Modules)

# ─── Tests ────────────────────────────────────────────────────────────────────

enable_testing()
add_subdirectory(tests)
