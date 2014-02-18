#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# - Try to find RemoteOIS
# Once done, this will define
#
#  RemoteOIS_FOUND - system has RemoteOIS
#  RemoteOIS_INCLUDE_DIRS - the RemoteOIS include directories 
#  RemoteOIS_LIBRARIES - link these to use RemoteOIS
#  RemoteOIS_BINARY_REL / RemoteOIS_BINARY_DBG - DLL names (windows only)

set (RemoteOIS_INCLUDE_DIR ${RemoteRenderer_ROOT}/RemoteOIS/includes)
set (RemoteOIS_LIBRARY_DBG ${RemoteRenderer_LIB}/RemoteOIS_d.lib)
set (RemoteOIS_LIBRARY_REL ${RemoteRenderer_LIB}/RemoteOIS.lib)
make_library_set(RemoteOIS_LIBRARY)

if (WIN32)
	set (RemoteOIS_BINARY_DBG ${RemoteRenderer_BIN}/RemoteOIS_d.dll)
	set (RemoteOIS_BINARY_REL ${RemoteRenderer_BIN}/RemoteOIS.dll)
endif()
mark_as_advanced(RemoteOIS_BINARY_REL RemoteOIS_BINARY_DBG)

findpkg_finish(RemoteOIS)
