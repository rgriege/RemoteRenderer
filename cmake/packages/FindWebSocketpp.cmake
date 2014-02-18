#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# - Try to find WebSocketpp
# Once done, this will define
#
#  WebSocketpp_FOUND - system has WebSocketpp
#  WebSocketpp_INCLUDE_DIRS - the WebSocketpp include directories

include(FindPkgMacros)
findpkg_begin(WebSocketpp)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(WebSocketpp_HOME)

# construct search paths
set(WebSocketpp_PREFIX_PATH ${WebSocketpp_HOME} ${ENV_WebSocketpp_HOME})
# redo search if prefix path changed
clear_if_changed(WebSocketpp_PREFIX_PATH
  WebSocketpp_INCLUDE_DIR
)

set(WebSocketpp_LIBRARY_NAMES WebSocketpp)
get_debug_names(WebSocketpp_LIBRARY_NAMES)

use_pkgconfig(WebSocketpp_PKGC WebSocketpp)

# For WebSocketpp, prefer static library over framework (important when referencing WebSocketpp source build)
set(CMAKE_FIND_FRAMEWORK "LAST")

if (WebSocketpp_HOME)
  # WebSocketpp uses the 'websocketpp' path for its headers in the source release, not 'include'
  set(WebSocketpp_INC_SEARCH_PATH ${WebSocketpp_INC_SEARCH_PATH} ${WebSocketpp_HOME}/websocketpp)
endif()
if (APPLE AND WebSocketpp_HOME)
  # WebSocketpp source build on Mac stores libs in a different location
  # Also this is for static build
  set(WebSocketpp_LIB_SEARCH_PATH ${WebSocketpp_LIB_SEARCH_PATH} ${WebSocketpp_HOME}/Mac/XCode-2.2/build)
endif()
find_path(WebSocketpp_INCLUDE_DIR NAMES connection.hpp connection_base.hpp endpoint.hpp endpoint_base.hpp HINTS ${WebSocketpp_INC_SEARCH_PATH} ${WebSocketpp_PKGC_INCLUDE_DIRS})

# do the final processing for the package find.
# skip if already processed during this run
if (NOT WebSocketpp_FOUND)
  if (WebSocketpp_INCLUDE_DIR)
    set(WebSocketpp_FOUND TRUE)
	get_parent_dir(WebSocketpp_INCLUDE_DIR)
    set(WebSocketpp_INCLUDE_DIRS ${WebSocketpp_INCLUDE_DIR_PARENT})
    if (NOT WebSocketpp_FIND_QUIETLY)
	  message(STATUS "Found WebSocketpp: ${WebSocketpp_INCLUDE_DIRS}")
    endif ()
  else ()
    if (NOT WebSocketpp_FIND_QUIETLY)
	  message(STATUS "Could not locate WebSocketpp")
    endif ()
    if (WebSocketpp_FIND_REQUIRED)
	  message(FATAL_ERROR "Required library WebSocketpp not found! Install the library (including dev packages) and try again. If the library is already installed, set the missing variables manually in cmake.")
  endif ()
endif ()

mark_as_advanced(WebSocketpp_INCLUDE_DIR)
endif ()

# Reset framework finding
set(CMAKE_FIND_FRAMEWORK "FIRST")
