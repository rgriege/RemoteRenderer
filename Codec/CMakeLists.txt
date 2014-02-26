# Create project
project (Codec)

# define header and source files for the library
set (HEADER_FILES
  CodecPrereqs.h
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
set (FFmpeg_ADDITIONAL_DEFINITIONS -D__STDC_CONSTANT_MACROS)

gather_libraries ()
add_library (Codec SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Codec ${LIBRARIES})

