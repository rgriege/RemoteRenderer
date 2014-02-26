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
set (WebSocketpp_ADDITIONAL_DEFINITIONS -D_WEBSOCKETPP_CPP11_STL_)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_COMMAND Boost 1.46.0 REQUIRED COMPONENTS thread system chrono date_time regex)

gather_libraries ()
add_executable (Server ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Server ${LIBRARIES})

if (MSVC)
  set_target_properties (Server PROPERTIES OUTPUT_NAME_DEBUG Server_d)
endif ()
