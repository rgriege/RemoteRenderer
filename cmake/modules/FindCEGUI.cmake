#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# - Try to find CEGUI
# Once done, this will define
#
#  CEGUI_FOUND - system has CEGUI
#  CEGUI_INCLUDE_DIRS - the CEGUI include directories 
#  CEGUI_LIBRARIES - link these to use CEGUI
#  CEGUI_BINARY_REL / CEGUI_BINARY_DBG - DLL names (windows only)

include(FindPkgMacros)
findpkg_begin(CEGUI)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(CEGUI_HOME)

# construct search paths
set(CEGUI_PREFIX_PATH ${CEGUI_HOME} ${ENV_CEGUI_HOME})
create_search_paths(CEGUI)
# redo search if prefix path changed
clear_if_changed(CEGUI_PREFIX_PATH
  CEGUI_LIBRARY_FWK
  CEGUI_LIBRARY_REL
  CEGUI_LIBRARY_DBG
  CEGUI_INCLUDE_DIR
)

use_pkgconfig(CEGUI_PKGC CEGUI)

# For CEGUI, prefer static library over framework (important when referencing CEGUI source build)
set(CMAKE_FIND_FRAMEWORK "LAST")

find_path(CEGUI_INCLUDE_DIR NAMES CEGUI.h HINTS ${CEGUI_INC_SEARCH_PATH} ${CEGUI_PKGC_INCLUDE_DIRS} PATH_SUFFIXES CEGUI)

set (CEGUI_FIND_COMPONENTS CEGUIBase CEGUIExpatParser CEGUIFalagardWRBase CEGUIOgreRenderer CEGUISILLYImageCodec)
if (WIN32)
  list (APPEND CEGUI_FIND_COMPONENTS tolua++)
else ()
  list (APPEND CEGUI_FIND_COMPONENTS CEGUItoluapp)
endif (WIN32)

set (CEGUI_LIBRARY_REL "")
set (CEGUI_LIBRARY_DBG "")
foreach (component ${CEGUI_FIND_COMPONENTS})
  set (${component}_LIBRARY_NAMES ${component})
  find_library(${component}_LIBRARY_REL NAMES ${${component}_LIBRARY_NAMES} HINTS ${CEGUI_LIB_SEARCH_PATH} ${CEGUI_PKGC_LIBRARY_DIRS})
  get_debug_names(${component}_LIBRARY_NAMES)
  find_library(${component}_LIBRARY_DBG NAMES ${${component}_LIBRARY_NAMES_DBG} HINTS ${CEGUI_LIB_SEARCH_PATH} ${CEGUI_PKGC_LIBRARY_DIRS})
  mark_as_advanced(${component}_LIBRARY_REL ${component}_LIBRARY_DBG)
  if (${component}_LIBRARY_REL)
    SET (${component}_FOUND ON)
  else ()
    SET (${component}_FOUND OFF)
    message ("${component} not found in ${CEGUI_LIB_SEARCH_PATH}.")
  endif ()
  if (NOT ${component}_FOUND)
    set (CEGUI_FOUND OFF)
  else ()
    list (APPEND CEGUI_LIBRARY_REL ${${component}_LIBRARY_REL})
    list (APPEND CEGUI_LIBRARY_DBG ${${component}_LIBRARY_DBG})
  endif ()
endforeach (component)

make_library_set(CEGUI_LIBRARY)

findpkg_finish(CEGUI)

# add parent of CEGUI folder to support CEGUI/CEGUI.h
add_parent_dir(CEGUI_INCLUDE_DIRS CEGUI_INCLUDE_DIR)

# Reset framework finding
set(CMAKE_FIND_FRAMEWORK "FIRST")
