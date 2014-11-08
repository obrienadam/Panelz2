# CMakeLists.txt for Panelz2/src/Geometry
# Author: Adam O'Brien

set(HEADERS Vector3D.h Point3D.h)

set(SOURCE Vector3D.cc)

add_library(Geometry ${HEADERS} ${SOURCE})
