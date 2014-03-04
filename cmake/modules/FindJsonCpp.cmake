# - Locate JsonCpp library
# This module defines
#  JsonCpp_LIBRARIES, the name of the library to link against
#  JsonCpp_FOUND, if false, do not try to link to JsonCpp
#  JsonCpp_INCLUDE_DIRS, where to find JsonCpp.h
#

include(FindPkgMacros)
findpkg_begin(JsonCpp)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(JsonCpp_HOME)

# construct search paths
set(JsonCpp_PREFIX_PATH ${JsonCpp_HOME} ${ENV_JsonCpp_HOME})
create_search_paths(JsonCpp)
# redo search if prefix path changed
clear_if_changed(JsonCpp_PREFIX_PATH)

# For JsonCpp, prefer static library over framework (important when referencing JsonCpp source build)
set(CMAKE_FIND_FRAMEWORK "LAST")

find_path(JsonCpp_INCLUDE_DIRS json.h HINTS ${JsonCpp_INC_SEARCH_PATH} PATH_SUFFIXES json jsoncpp/json)
get_parent_dir(JsonCpp_INCLUDE_DIRS)
set (JsonCpp_INCLUDE_DIRS ${JsonCpp_INCLUDE_DIRS_PARENT})

find_library(JsonCpp_LIBRARIES NAMES jsoncpp HINTS ${JsonCpp_LIB_SEARCH_PATH})

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(JsonCpp
                                  REQUIRED_VARS JsonCpp_LIBRARIES JsonCpp_INCLUDE_DIRS)

set (JsonCpp_FOUND ${JSONCPP_FOUND})

mark_as_advanced (JsonCpp_INCLUDE_DIRS JsonCpp_LIBRARIES)

# Reset framework finding
set(CMAKE_FIND_FRAMEWORK "FIRST")