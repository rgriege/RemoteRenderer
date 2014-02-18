# Create project
project (SnS_Demo)

# define header and source files for the library
set (HEADER_FILES
  ../RemoteOgreFramework/AppState.h
  ../RemoteOgreFramework/AppStateManager.h
  ../RemoteOgreFramework/RemoteOgreFramework.h
  header/Ball.h
  header/Blob.h
  header/Boss.h
  header/Box.h
  header/BoxChunk.h
  header/cd_wavefront.h
  header/Character.h
  header/CharacterCreateState.h
  header/CharacterSelectState.h
  header/Enemy.h
  header/float_math.h
  header/FloatingNumber.h
  header/FloatingText.h
  header/GameObject.h
  header/GameState.h
  header/Item.h
  header/ItemDatabase.h
  header/MenuState.h
  header/MotionState.h
  header/Plane.h
  header/Player.h
  header/Projectile.h
  header/pugixml.hpp
  header/Robot.h
  header/SoundManager.h
  header/SpacesAndShapes.h
  header/stdafx.h
  header/StringHelper.h
  header/World.h
)

include_directories (header)
include_directories (${RemoteRenderer_ROOT}/RemoteOgreFramework)

set (SOURCE_FILES
  ../RemoteOgreFramework/AppStateManager.cpp
  ../RemoteOgreFramework/RemoteOgreFramework.cpp
  src/Ball.cpp
  src/Blob.cpp
  src/Boss.cpp
  src/Box.cpp
  src/BoxChunk.cpp
  src/cd_wavefront.cpp
  src/Character.cpp
  src/CharacterCreateState.cpp
  src/CharacterSelectState.cpp
  src/Enemy.cpp
  src/float_math.cpp
  src/FloatingNumber.cpp
  src/FloatingText.cpp
  src/GameObject.cpp
  src/GameState.cpp
  src/Item.cpp
  src/ItemDatabase.cpp
  src/MenuState.cpp
  src/MotionState.cpp
  src/Plane.cpp
  src/Player.cpp
  src/Projectile.cpp
  src/pugixml.cpp
  src/Robot.cpp
  src/SoundManager.cpp
  src/SpacesAndShapes.cpp
  src/stdafx.cpp
  src/StringHelper.cpp
  src/World.cpp
)

# Setup for library find
set (POSSIBLE_LIBRARIES OGRE OIS WebSocketpp Boost FFmpeg Codec RemoteOIS CEGUI SDL BULLET SDL_MIXER)

if (WIN32)
  set (OGRE_ADDITIONAL_DEFINITIONS "/wd4251 /wd4275 /D _UNICODE /D UNICODE")
endif (WIN32)

set (OIS_ADDITIONAL_DEFINITIONS -DOIS_NONCLIENT_BUILD -DOIS_DYNAMIC_LIB)

set (WebSocketpp_HEADER_ONLY ON)

set (Boost_USE_STATIC_LIBS ON)
set (Boost_COMMAND Boost 1.51.0 REQUIRED COMPONENTS thread system chrono date_time regex)

set (FFmpeg_COMMAND FFmpeg REQUIRED COMPONENTS avcodec avutil swscale)

gather_libraries ()
add_executable (SnS_Demo ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (SnS_Demo ${LIBRARIES})
if (WIN32)
  get_target_property (cur_flags SnS_Demo LINK_FLAGS)
  if (NOT cur_flags)
    set (cur_flags "")
  endif (NOT cur_flags)
  set_target_properties (SnS_Demo PROPERTIES LINK_FLAGS "${cur_flags} /SAFESEH:NO /SUBSYSTEM:WINDOWS")
endif ()

if (MSVC)
  foreach (CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string (TOUPPER ${CONFIG} CONFIG)
    set_target_properties (SnS_Demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_BIN})
    set_target_properties (SnS_Demo PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_LIB})
    set_target_properties (SnS_Demo PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${RemoteRenderer_LIB})
  endforeach (CONFIG)
  set_target_properties (SnS_Demo PROPERTIES OUTPUT_NAME_DEBUG SnS_Demo_d)
endif ()