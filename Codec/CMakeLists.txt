# Create project
project (Codec)

# define header and source files for the library
set (HEADER_FILES
  Init.h
  rgbData.h
  Decoder.h
  Encoder.h
)

# add "header library"
include_directories (../Stream)

set (SOURCE_FILES
  Init.cpp
  rgbData.cpp
  Decoder.cpp
  Encoder.cpp
)

# Setup for library find
set (POSSIBLE_LIBRARIES FFmpeg)
set (FFmpeg_COMMAND FFmpeg REQUIRED COMPONENTS avcodec avutil swscale)

gather_libraries ()
add_library (Codec SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Codec ${LIBRARIES})
if (WIN32)
  get_target_property (cur_flags Codec LINK_FLAGS)
  if (NOT cur_flags)
    set (cur_flags "")
  endif (NOT cur_flags)
  set_target_properties (Codec PROPERTIES LINK_FLAGS "${cur_flags} /SAFESEH:NO")
endif ()

if (MSVC)
  foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER ${CONFIG} CONFIG)
    set_target_properties (Codec PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_BIN})
    set_target_properties (Codec PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_LIB})
    set_target_properties (Codec PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_LIB})
  endforeach (CONFIG)
  set_target_properties (Codec PROPERTIES OUTPUT_NAME_DEBUG Codec_d)
endif ()