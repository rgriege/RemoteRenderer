# - Find FFmpeg include dirs and libraries
# Use this module by invoking find_package with the form:
#  find_package(FFmpeg
#    [REQUIRED]             # Fail with error if FFmpeg is not found
#    [COMPONENTS <libs>...] # FFmpeg libraries by their canonical name
#    )                      # e.g. "avcodec" for "libavcodec"

#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# - Try to find FFmpeg
# Once done, this will define
#
#  FFmpeg_FOUND - system has FFmpeg
#  FFmpeg_INCLUDE_DIRS - the FFmpeg include directories 
#  FFmpeg_LIBRARIES - link these to use FFmpeg
#  FFmpeg_BINARY_REL / FFmpeg_BINARY_DBG - DLL names (windows only)

include(FindPkgMacros)
findpkg_begin(FFmpeg)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(FFmpeg_HOME)

# construct search paths
set(FFmpeg_PREFIX_PATH ${FFmpeg_HOME} ${ENV_FFmpeg_HOME})
create_search_paths(FFmpeg)
# redo search if prefix path changed
clear_if_changed(FFmpeg_PREFIX_PATH
  FFmpeg_LIBRARY_FWK
  FFmpeg_LIBRARY_REL
  FFmpeg_LIBRARY_DBG
  FFmpeg_INCLUDE_DIR
)

set(FFmpeg_LIBRARY_NAMES FFmpeg)

use_pkgconfig(FFmpeg_PKGC FFmpeg)

# For FFmpeg, prefer static library over framework (important when referencing FFmpeg source build)
set(CMAKE_FIND_FRAMEWORK "LAST")

if (FFmpeg_HOME)
  # FFmpeg uses the 'includes' path for its headers in the source release, not 'include'
  set(FFmpeg_INC_SEARCH_PATH ${FFmpeg_INC_SEARCH_PATH} ${FFmpeg_HOME}/include)
endif()
if (APPLE AND FFmpeg_HOME)
  # FFmpeg source build on Mac stores libs in a different location
  # Also this is for static build
  set(FFmpeg_LIB_SEARCH_PATH ${FFmpeg_LIB_SEARCH_PATH} ${FFmpeg_HOME}/Mac/XCode-2.2/build)
endif()

set (FFmpeg_FOUND ON)
if (FFmpeg_FIND_COMPONENTS)
  foreach (component ${FFmpeg_FIND_COMPONENTS})
    find_path(${component}_INCLUDE_DIR NAMES ${component}.h HINTS ${FFmpeg_INC_SEARCH_PATH} ${FFmpeg_PKGC_INCLUDE_DIRS} PATH_SUFFIXES lib${component})
    find_library(${component}_LIBRARY NAMES ${component}.lib HINTS ${FFmpeg_LIB_SEARCH_PATH} ${FFmpeg_PKGC_LIBRARY_DIRS})
    if (WIN32)
      set(${component}_BIN_SEARCH_PATH ${FFmpeg_HOME}/bin ${ENV_FFmpeg_HOME}/bin)
      file (GLOB ${component}_BINARY ${FFmpeg_BIN_SEARCH_PATH}/${component}*.dll)
      mark_as_advanced (${component}_BINARY)
    endif()
    mark_as_advanced(${component}_INCLUDE_DIR ${component}_LIBRARY)
    SET (${component}_FOUND (${component}_INCLUDE_DIR AND ${component}_LIBRARY))
    if (NOT ${component}_FOUND)
      set (FFmpeg_FOUND OFF)
    else ()
      list (APPEND FFmpeg_LIBRARIES ${${component}_LIBRARY})
    endif ()
  endforeach (component)
endif ()

if (FFmpeg_FOUND)
  set (FFmpeg_INCLUDE_DIRS ${FFmpeg_HOME}/include)
  message (STATUS "Found the following FFmpeg libraries:")
  foreach (component ${FFmpeg_FIND_COMPONENTS})
    message (STATUS "  ${component}")
  endforeach (component)
else ()
  if (NOT FFmpeg_FIND_QUIETLY)
    message (STATUS "Could not locate the following FFmpeg components:")
    foreach (component ${FFmpeg_FIND_COMPONENTS})
      if (NOT ${component}_FOUND)
        message (STATUS "  ${component}")
      endif ()
    endforeach (component)
  endif ()
  if (FFmpeg_FIND_REQUIRED)
    message(FATAL_ERROR "Required library FFmpeg not found! Install the library (including dev packages) and try again. If the library is already installed, set the missing variables manually in cmake.")
  endif ()
endif ()

# Reset framework finding
set(CMAKE_FIND_FRAMEWORK "FIRST")
