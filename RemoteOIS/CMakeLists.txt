# Create project
project (RemoteOIS)

# define header and source files for the library
set (HEADER_FILES
  includes/RemoteOIS.h
  includes/RemoteOISConnectionManager.h
  includes/RemoteOISDefaultDeviceProtocol.h
  includes/RemoteOISDefaultFactoryCreatorProtocol.h
  includes/RemoteOISInputManager.h
  includes/RemoteOISKeyboard.h
  includes/RemoteOISMouse.h
  includes/RemoteOISPrereqs.h
)

# add base header file directory as additional include directory
include_directories (includes)

set (SOURCE_FILES
  src/RemoteOISConnectionManager.cpp
  src/RemoteOISDefaultDeviceProtocol.cpp
  src/RemoteOISInputManager.cpp
  src/RemoteOISKeyboard.cpp
  src/RemoteOISMouse.cpp
)

# Setup for library find
set (POSSIBLE_LIBRARIES OIS WebSocketpp Boost)

set (OIS_ADDITIONAL_DEFINITIONS -DOIS_NONCLIENT_BUILD -DOIS_DYNAMIC_LIB)

set (USE_WebSocketpp ON CACHE BOOL "Toggle WebSocketpp Connections")
set (WebSocketpp_HEADER_ONLY ON)
set (WebSocketpp_ADDITIONAL_HEADERS includes/RemoteOISWebSocketppConnection.h)
set (WebSocketpp_ADDITIONAL_SOURCE src/RemoteOISWebSocketppConnection.cpp)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_COMMAND Boost 1.51.0 REQUIRED COMPONENTS thread system chrono date_time regex)

gather_libraries ()
add_library (RemoteOIS SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (RemoteOIS ${LIBRARIES})

if (MSVC)
  foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER ${CONFIG} CONFIG)
    set_target_properties (RemoteOIS PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_BIN})
    set_target_properties (RemoteOIS PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_LIB})
    set_target_properties (RemoteOIS PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_LIB})
  endforeach (CONFIG)
  set_target_properties (RemoteOIS PROPERTIES OUTPUT_NAME_DEBUG RemoteOIS_d)
endif ()