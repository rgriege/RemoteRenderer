#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# - Try to find Codec
# Once done, this will define
#
#  Codec_FOUND - system has Codec
#  Codec_INCLUDE_DIRS - the Codec include directories 
#  Codec_LIBRARIES - link these to use Codec
#  Codec_BINARY_REL / Codec_BINARY_DBG - DLL names (windows only)

set (Codec_INCLUDE_DIR ${RemoteRenderer_ROOT}/Codec)

if (WIN32)
    set (Codec_LIBRARY_DBG ${RemoteRenderer_LIB}/Codec_d.lib)
    set (Codec_LIBRARY_REL ${RemoteRenderer_LIB}/Codec.lib)
	set (Codec_BINARY_DBG ${RemoteRenderer_BIN}/Codec_d.dll)
	set (Codec_BINARY_REL ${RemoteRenderer_BIN}/Codec.dll)
    mark_as_advanced(Codec_BINARY_REL Codec_BINARY_DBG)
else ()
    set (Codec_LIBRARY_DBG ${RemoteRenderer_LIB}/libCodec_d.so)
    set (Codec_LIBRARY_REL ${RemoteRenderer_LIB}/libCodec.so)
endif()
make_library_set(Codec_LIBRARY)

findpkg_finish(Codec)
