# CMakeLists.txt for Panelz2/src/Input
# Author: Adam O'Brien

set(HEADERS Input.h VersionInfo.h)

set(SOURCE Input.cxx)

add_library(Input ${HEADERS} ${SOURCE})
target_link_libraries(Input boost_program_options)
