# - Locate Freeglut library
# This module defines
#  Freeglut_LIBRARIES, the name of the library to link against
#  Freeglut_FOUND, if false, do not try to link to Freeglut
#  Freeglut_INCLUDE_DIRS, where to find Freeglut.h
#

find_path(Freeglut_INCLUDE_DIRS freeglut.h PATH_SUFFIXES GL)

find_library(Freeglut_LIBRARIES NAMES glut PATH_SUFFIXES lib)

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Freeglut
                                  REQUIRED_VARS Freeglut_LIBRARIES Freeglut_INCLUDE_DIRS)

set (Freeglut_FOUND ${FREEGLUT_FOUND})
