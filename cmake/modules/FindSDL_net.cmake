# Locate SDL_net library
# This module defines
# SDL_net_LIBRARIES, the name of the library to link against
# SDL_net_FOUND, if false, do not try to link against
# SDL_net_INCLUDE_DIRS, where to find the headers
#
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
#
# Created by Eric Wing. This was influenced by the FindSDL.cmake 
# module, but with modifications to recognize OS X frameworks and 
# additional Unix paths (FreeBSD, etc).

#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(SDL_net_INCLUDE_DIRS SDL_net.h
  HINTS
  $ENV{SDLNETDIR}
  $ENV{SDLDIR}
  PATH_SUFFIXES include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include/SDL
  /usr/include/SDL
  /usr/local/include/SDL12
  /usr/local/include/SDL11 # FreeBSD ports
  /usr/include/SDL12
  /usr/include/SDL11
  /usr/local/include
  /usr/include
  /sw/include/SDL # Fink
  /sw/include
  /opt/local/include/SDL # DarwinPorts
  /opt/local/include
  /opt/csw/include/SDL # Blastwave
  /opt/csw/include 
  /opt/include/SDL
  /opt/include
)
FIND_LIBRARY(SDL_net_LIBRARIES 
  NAMES SDL_net
  HINTS
  $ENV{SDLNETDIR}
  $ENV{SDLDIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

SET(SDL_net_FOUND "NO")
IF(SDL_net_LIBRARIES AND SDL_net_INCLUDE_DIRS)
  SET(SDL_net_FOUND "YES")
ENDIF(SDL_net_LIBRARIES AND SDL_net_INCLUDE_DIRS)

