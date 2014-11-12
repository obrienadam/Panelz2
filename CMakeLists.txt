# CMakeLists.txt for Panelz2
# Author: Adam O'Brien

cmake_minimum_required (VERSION 2.6.2 FATAL_ERROR)

project(Panelz2)

# Includes

set(INCLUDE_DIR src/Input
                src/Geometry
                src/LinearAlgebra
                src/Airfoil
                /usr/include/petsc
                /usr/local/include/petsc)

include_directories(${INCLUDE_DIR})

# Compiler flags

set(CMAKE_CC_FLAGS "-g -Wall -O3 -Wsign-compare")
set(CMAKE_CXX_FLAGS "-g -Wall -O3 -Wno-reorder -std=c++11 -Wsign-compare")

# Output paths

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Libraries

add_subdirectory(src)

# Modules

add_subdirectory(Modules)
