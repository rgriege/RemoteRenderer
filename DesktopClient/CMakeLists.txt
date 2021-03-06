# Create project
project (DesktopClient)

set (HEADER_FILES
  ClientApp.h
  GameApp.h
  LibraryApp.h
)

if (NOT MSVC)
  include_directories (../utility/)
endif()

set (SOURCE_FILES
  main.cpp
  ClientApp.cpp
  GameApp.cpp
  LibraryApp.cpp
)

include (CMakeDependentOption)

# Setup for library find
set (POSSIBLE_LIBRARIES OIS Boost FFmpeg Codec RemoteOIS LibraryConfig Freeglut JsonCpp)

option (USE_SDL_UDP "Use UDP SDL connections" ON)
CMAKE_DEPENDENT_OPTION (USE_WEBSOCKETPP "Use TCP Websocketpp connections" ON "NOT USE_SDL_UDP" OFF)
CMAKE_DEPENDENT_OPTION (USE_SDL_TCP "Use TCP SDL connections" ON "NOT USE_SDL_UDP; NOT_USE_SDL_TCP" OFF)

if (USE_SDL_UDP)
  list (APPEND POSSIBLE_LIBRARIES SDL SDL_net)
  set (SDL_BUILDING_LIBRARY ON)
  #set (SDL_net_ADDITIONAL_HEADERS )
  #set (SDL_net_ADDITIONAL_SOURCE )
elseif (USE_WEBSOCKETPP)
  list (APPEND POSSIBLE_LIBRARIES WebSocketpp)
  #set (WebSocketpp_ADDITIONAL_HEADERS
  set (WebSocketpp_HEADER_ONLY ON)
  if (NOT MSVC)
    set (WebSocketpp_ADDITIONAL_DEFINITIONS -D_WEBSOCKETPP_CPP11_STL_)
  endif ()
elseif (USE_SDL_TCP)
  list (APPEND POSSIBLE_LIBRARIES SDL SDL_net)
  set (SDL_BUILDING_LIBRARY ON)
endif ()

set (OIS_ADDITIONAL_DEFINITIONS -DOIS_NONCLIENT_BUILD -DOIS_DYNAMIC_LIB)

if (WIN32)
  set (Boost_USE_STATIC_LIBS ON)
else ()
  set (Boost_ADDITIONAL_DEFINITIONS -DBOOST_ALL_DYN_LINK)
endif ()
set (Boost_COMMAND Boost 1.46.0 REQUIRED COMPONENTS thread system chrono date_time regex random)

set (FFmpeg_COMMAND FFmpeg REQUIRED COMPONENTS avcodec avutil swscale)
set (FFmpeg_ADDITIONAL_DEFINITIONS -D__STDC_CONSTANT_MACROS)

gather_libraries ()
add_executable (DesktopClient ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (DesktopClient ${LIBRARIES})

if (WIN32)
  set_target_properties (DesktopClient PROPERTIES OUTPUT_NAME_DEBUG DesktopClient_d)
endif ()
