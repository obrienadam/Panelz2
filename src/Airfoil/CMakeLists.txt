# CMakeLists.txt for Panelz2/src/Airfoil
# Author: Adam O'Brien
# Boost removed; CLI parsing now lives in the main executable (absl::flags).

set(HEADERS Input.h VersionInfo.h Freestream.h Airfoil.h)
set(SOURCE   Input.cxx Freestream.cxx Airfoil.cxx)

add_library(Airfoil ${HEADERS} ${SOURCE})

target_link_libraries(Airfoil PUBLIC Geometry Matrix)

target_include_directories(Airfoil PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
