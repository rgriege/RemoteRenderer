# Create project
project (LibraryConfig)

# define header and source files for the library
set (HEADER_FILES
  Config.h
  Game.h
)

set (SOURCE_FILES
  Config.cpp
  Game.cpp
)

set (POSSIBLE_LIBRARIES JsonCpp)
gather_libraries ()

find_package (WebSocketpp)
if (WebSocketpp_FOUND)
  include_directories (${WebSocketpp_INCLUDE_DIRS})
endif ()

add_library (LibraryConfig SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (LibraryConfig ${LIBRARIES})

if (MSVC)
  set_target_properties (LibraryConfig PROPERTIES OUTPUT_NAME_DEBUG LibraryConfig_d)
endif ()
