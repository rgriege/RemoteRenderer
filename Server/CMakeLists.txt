# Create project
project (Server)

# define header and source files for the library
set (HEADER_FILES
  Config.h
  ServerApp.h
)

set (SOURCE_FILES
  Config.cpp
  ServerApp.cpp
)

# Setup for library find
set (POSSIBLE_LIBRARIES WebSocketpp Boost)

set (USE_WebSocketpp ON CACHE BOOL "Toggle WebSocketpp Connections")
set (WebSocketpp_HEADER_ONLY ON)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_COMMAND Boost 1.51.0 REQUIRED COMPONENTS thread system chrono date_time regex)

gather_libraries ()
add_executable (Server ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Server ${LIBRARIES})

if (MSVC)
  foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER ${CONFIG} CONFIG)
    set_target_properties (Server PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_BIN})
    set_target_properties (Server PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_LIB})
    set_target_properties (Server PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_LIB})
  endforeach (CONFIG)
  set_target_properties (Server PROPERTIES OUTPUT_NAME_DEBUG Server_d)
endif ()