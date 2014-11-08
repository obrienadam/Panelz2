# CMakeLists.txt for Panelz2/src/LinearAlgebra
# Author: Adam O'Brien

set(HEADERS Matrix.h)

set(SOURCES Matrix.cxx)

add_library(Matrix ${HEADERS} ${SOURCES})
target_link_libraries(Matrix petsc)