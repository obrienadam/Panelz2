# CMakeLists.txt for Panelz2/src/Airfoil
# Author: Adam O'Brien

set(HEADERS Input.h VersionInfo.h Freestream.h Airfoil.h)

set(SOURCE Input.cxx Freestream.cxx Airfoil.cxx)

add_library(Airfoil ${HEADERS} ${SOURCE})
target_link_libraries(Airfoil Geometry
                              boost_program_options)
