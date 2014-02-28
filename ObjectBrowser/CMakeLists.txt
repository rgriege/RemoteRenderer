# Create project
project (ObjectBrowser)

# define header and source files for the library
set (HEADER_FILES
  ObjectBrowser.h
  GameState.h
  ../RemoteOgreFramework/AppState.h
  ../RemoteOgreFramework/AppStateManager.h
  ../RemoteOgreFramework/RemoteOgreFramework.h
)

include_directories (${RemoteRenderer_ROOT}/RemoteOgreFramework)

set (SOURCE_FILES
  ObjectBrowser.cpp
  GameState.cpp
  ../RemoteOgreFramework/AppStateManager.cpp
  ../RemoteOgreFramework/RemoteOgreFramework.cpp
)

# Setup for library find
set (POSSIBLE_LIBRARIES OGRE OIS WebSocketpp Boost FFmpeg Codec RemoteOIS)

if (WIN32)
  set (OGRE_ADDITIONAL_DEFINITIONS "/wd4251 /wd4275 /D _UNICODE /D UNICODE")
endif (WIN32)

set (OIS_ADDITIONAL_DEFINITIONS -DOIS_NONCLIENT_BUILD -DOIS_DYNAMIC_LIB)

set (WebSocketpp_HEADER_ONLY ON)
if (NOT WIN32)
  set (WebSocketpp_ADDITIONAL_DEFINITIONS -D_WEBSOCKETPP_CPP11_STL_)
endif (NOT WIN32)

if (WIN32)
  set (Boost_USE_STATIC_LIBS ON)
else ()
  set (Boost_ADDITIONAL_DEFINITIONS -DBOOST_ALL_DYN_LINK)
endif ()
set (Boost_COMMAND Boost 1.46.0 REQUIRED COMPONENTS thread system chrono date_time regex)

set (FFmpeg_COMMAND FFmpeg REQUIRED COMPONENTS avcodec avutil swscale)
set (FFmpeg_ADDITIONAL_DEFINITIONS -D__STDC_CONSTANT_MACROS)

gather_libraries ()
add_executable (ObjectBrowser WIN32 ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (ObjectBrowser ${LIBRARIES})

if (WIN32)
  get_target_property (cur_flags ObjectBrowser LINK_FLAGS)
  if (NOT cur_flags)
    set (cur_flags "")
  endif (NOT cur_flags)
  set_target_properties (ObjectBrowser PROPERTIES LINK_FLAGS "${cur_flags} /SUBSYSTEM:WINDOWS")
  set_target_properties (ObjectBrowser PROPERTIES OUTPUT_NAME_DEBUG ObjectBrowser_d)
endif ()
