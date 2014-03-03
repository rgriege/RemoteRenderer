# Create project
project (Server)

# define header and source files for the library
set (HEADER_FILES
  ServerApp.h
)

set (SOURCE_FILES
  ServerApp.cpp
)

# Setup for library find
set (POSSIBLE_LIBRARIES WebSocketpp Boost LibraryConfig JsonCpp)

set (USE_WebSocketpp ON CACHE BOOL "Toggle WebSocketpp Connections")
set (WebSocketpp_HEADER_ONLY ON)
if (NOT MSVC)
  set (WebSocketpp_ADDITIONAL_DEFINITIONS -D_WEBSOCKETPP_CPP11_STL_)
endif ()

if (WIN32)
  set (Boost_USE_STATIC_LIBS ON)
else ()
  set (Boost_ADDITIONAL_DEFINITIONS -DBOOST_ALL_DYN_LINK)
endif ()
set (Boost_COMMAND Boost 1.46.0 REQUIRED COMPONENTS thread system chrono date_time regex)

gather_libraries ()
add_executable (Server ${HEADER_FILES} ${SOURCE_FILES})
if (UNIX)
  list (APPEND LIBRARIES pthread)
endif (UNIX)
target_link_libraries (Server ${LIBRARIES})

if (MSVC)
  set_target_properties (Server PROPERTIES OUTPUT_NAME_DEBUG Server_d)
endif ()
