# Minimum cmake requirement. We should require a quite recent
# cmake for the dependency find macros etc. to be up to date.
cmake_minimum_required (VERSION 2.6)

# Set CMake library search policy
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    cmake_policy (SET CMP0005 NEW)
endif ()

# Create project
project (RemoteRenderer)

# Use relative paths
# This is mostly to reduce path size for command-line limits on windows
if (WIN32)
  set (CMAKE_USE_RELATIVE_PATHS true)
endif ()

# Paths
set (RemoteRenderer_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set (RemoteRenderer_INCLUDE ${TARGET_NAME_ROOT}/includes)
set (RemoteRenderer_BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set (RemoteRenderer_BIN ${RemoteRenderer_ROOT}/bin)
set (RemoteRenderer_LIB ${RemoteRenderer_ROOT}/lib)

# Include necessary submodules
set (CMAKE_MODULE_PATH 
  "${CMAKE_MODULE_PATH}"
  "${RemoteRenderer_ROOT}/cmake"
  "${RemoteRenderer_ROOT}/cmake/utils" 
  "${RemoteRenderer_ROOT}/cmake/packages"
)
include (LinkLibraries)

set (RemoteRenderer_MAJOR_VERSION 0)
set (RemoteRenderer_MINOR_VERSION 2)
set (RemoteRenderer_PATCH_VERSION 0)
set (RemoteRenderer_VERSION ${RemoteRenderer_MAJOR_VERSION}.${RemoteRenderer_MINOR_VERSION}.${RemoteRenderer_PATCH_VERSION})

set (INSTALL_INCLUDE_DIR includes CACHE PATH "Installation directory for header files")

# Disable unnecessary build types
set (CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "Configurations" FORCE)


# CMake install step prefix. I assume linux users want the prefix to
# be the default /usr or /usr/local so this is only adjusted on Windows.
# - Windows: Build the INSTALL project in your solution file.
# - Linux/OSX: make install.
if (MSVC)
    set (CMAKE_INSTALL_PREFIX "${RemoteRenderer_ROOT}/install")
endif ()

# Specify build paths
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_NAME_LIB})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_NAME_LIB})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_NAME_BIN})

# Preprocessing environment variables
if (WIN32)
  if (SDL_HOME AND NOT ENV{SDLDIR})
    set (ENV{SDLDIR} ${SDL_HOME})
  endif (SDL_HOME AND NOT ENV{SDLDIR})
  if (SDL_MIXER_HOME AND NOT ENV{SDLMIXERDIR})
    set (ENV{SDLMIXERDIR} ${SDL_MIXER_HOME})
  endif (SDL_MIXER_HOME AND NOT ENV{SDLMIXERDIR})
endif (WIN32)

add_subdirectory (Codec)
add_subdirectory (ObjectBrowser)
add_subdirectory (RemoteOIS)
add_subdirectory (Server)
add_subdirectory (SnS_Demo)