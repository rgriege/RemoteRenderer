# Create project
project (DesktopClient)
set (SOURCE_FILES
  main.cpp
)

include (CMakeDependentOption)

# Setup for library find
set (POSSIBLE_LIBRARIES OIS WebSocketpp Boost FFmpeg Codec RemoteOIS)

option (USE_SDL_UDP "Use UDP SDL connections")
CMAKE_DEPENDENT_OPTION (USE_WEBSOCKETPP "Use TCP Websocketpp connections" ON "NOT USE_SDL_UDP" OFF)
CMAKE_DEPENDENT_OPTION (USE_SDL_TCP "Use TCP SDL connections" ON "NOT USE_SDL_UDP; NOT_USE_SDL_TCP" OFF)

if (USE_SDL_UDP OR USE_SDL_TCP)
  list (APPEND POSSIBLE_LIBRARIES SDL SDL_net)


if (WIN32)
  set (OGRE_ADDITIONAL_DEFINITIONS "/wd4251 /wd4275 /D _UNICODE /D UNICODE")
endif (WIN32)

set (OIS_ADDITIONAL_DEFINITIONS -DOIS_NONCLIENT_BUILD -DOIS_DYNAMIC_LIB)

set (WebSocketpp_HEADER_ONLY ON)
if (NOT MSVC)
  set (WebSocketpp_ADDITIONAL_DEFINITIONS -D_WEBSOCKETPP_CPP11_STL_)
endif ()

set (Boost_COMMAND Boost 1.46.0 REQUIRED COMPONENTS thread system chrono date_time regex)

set (FFmpeg_COMMAND FFmpeg REQUIRED COMPONENTS avcodec avutil swscale)
set (FFmpeg_ADDITIONAL_DEFINITIONS -D__STDC_CONSTANT_MACROS)

gather_libraries ()
add_executable (DesktopClient ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (DesktopClient ${LIBRARIES})

if (WIN32)
  set_target_properties (DesktopClient PROPERTIES OUTPUT_NAME_DEBUG DesktopClient_d)
endif ()
