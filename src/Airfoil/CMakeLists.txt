# CMakeLists.txt for Panelz2/src/Airfoil
# Author: Adam O'Brien

set(HEADERS Airfoil.h)

set(SOURCE Airfoil.cc)

add_library(Airfoil ${HEADERS} ${SOURCE})
