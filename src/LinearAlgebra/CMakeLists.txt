# CMakeLists.txt for Panelz2/src/LinearAlgebra
# Eigen is header-only; we just expose it as an interface target.
# Author: Adam O'Brien

set(HEADERS Matrix.h)
set(SOURCES Matrix.cxx)

add_library(Matrix ${HEADERS} ${SOURCES})

target_link_libraries(Matrix PUBLIC Eigen3::Eigen)

target_include_directories(Matrix PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)