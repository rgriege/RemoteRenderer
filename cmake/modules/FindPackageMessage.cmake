# FIND_PACKAGE_MESSAGE(<name> "message for user" "find result details")
#
# This macro is intended to be used in FindXXX.cmake modules files.
# It will print a message once for each unique find result.
# This is useful for telling the user where a package was found.
# The first argument specifies the name (XXX) of the package.
# The second argument specifies the message to display.
# The third argument lists details about the find result so that
# if they change the message will be displayed again.
# The macro also obeys the QUIET argument to the find_package command.
#
# Example:
#
#  if(X11_FOUND)
#    FIND_PACKAGE_MESSAGE(X11 "Found X11: ${X11_X11_LIB}"
#      "[${X11_X11_LIB}][${X11_INCLUDE_DIR}]")
#  else()
#   ...
#  endif()

#=============================================================================
# CMake - Cross Platform Makefile Generator
# Copyright 2000-2011 Kitware, Inc., Insight Software Consortium
# All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:

# * Redistributions of source code must retain the above copyright
  # notice, this list of conditions and the following disclaimer.

# * Redistributions in binary form must reproduce the above copyright
  # notice, this list of conditions and the following disclaimer in the
  # documentation and/or other materials provided with the distribution.

# * Neither the names of Kitware, Inc., the Insight Software Consortium,
  # nor the names of their contributors may be used to endorse or promote
  # products derived from this software without specific prior written
  # permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# ------------------------------------------------------------------------------

# The above copyright and license notice applies to distributions of
# CMake in source and binary form.  Some source files contain additional
# notices of original copyright by their contributors; see each source
# for details.  Third-party software packages supplied with CMake under
# compatible licenses provide their own copyright notices documented in
# corresponding subdirectories.

# ------------------------------------------------------------------------------

# CMake was initially developed by Kitware with the following sponsorship:

 # * National Library of Medicine at the National Institutes of Health
   # as part of the Insight Segmentation and Registration Toolkit (ITK).

 # * US National Labs (Los Alamos, Livermore, Sandia) ASC Parallel
   # Visualization Initiative.

 # * National Alliance for Medical Image Computing (NAMIC) is funded by the
   # National Institutes of Health through the NIH Roadmap for Medical Research,
   # Grant U54 EB005149.

 # * Kitware, Inc.
#=============================================================================


function(FIND_PACKAGE_MESSAGE pkg msg details)
  # Avoid printing a message repeatedly for the same find result.
  if(NOT ${pkg}_FIND_QUIETLY)
    string(REGEX REPLACE "[\n]" "" details "${details}")
    set(DETAILS_VAR FIND_PACKAGE_MESSAGE_DETAILS_${pkg})
    if(NOT "${details}" STREQUAL "${${DETAILS_VAR}}")
      # The message has not yet been printed.
      message(STATUS "${msg}")

      # Save the find details in the cache to avoid printing the same
      # message again.
      set("${DETAILS_VAR}" "${details}"
        CACHE INTERNAL "Details about finding ${pkg}")
    endif()
  endif()
endfunction()
