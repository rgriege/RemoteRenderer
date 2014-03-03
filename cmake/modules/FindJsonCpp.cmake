# - Locate JsonCpp library
# This module defines
#  JsonCpp_LIBRARIES, the name of the library to link against
#  JsonCpp_FOUND, if false, do not try to link to JsonCpp
#  JsonCpp_INCLUDE_DIRS, where to find JsonCpp.h
#

find_path(JsonCpp_INCLUDE_DIRS json.h PATH_SUFFIXES json jsoncpp/json)
get_parent_dir(JsonCpp_INCLUDE_DIRS)
set (JsonCpp_INCLUDE_DIRS ${JsonCpp_INCLUDE_DIRS_PARENT})

find_library(JsonCpp_LIBRARIES NAMES jsoncpp PATH_SUFFIXES lib)

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(JsonCpp
                                  REQUIRED_VARS JsonCpp_LIBRARIES JsonCpp_INCLUDE_DIRS)

set (JsonCpp_FOUND ${JSONCPP_FOUND})
