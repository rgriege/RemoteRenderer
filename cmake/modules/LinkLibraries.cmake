
# Find libraries
macro (gather_libraries)
  foreach (lib ${POSSIBLE_LIBRARIES})
    if (NOT DEFINED USE_${lib} OR USE_${lib})
      if (DEFINED ${lib}_COMMAND)
        find_package (${${lib}_COMMAND})
      else ()
        find_package (${lib})
      endif ()
      if (${lib}_FOUND)
        if (DEFINED ${lib}_INCLUDE_DIRS)
          include_directories (${${lib}_INCLUDE_DIRS})
        endif ()
        list (APPEND HEADER_FILES ${${lib}_ADDITIONAL_HEADERS})
        list (APPEND SOURCE_FILES ${${lib}_ADDITIONAL_SOURCE})
        add_definitions (${${lib}_ADDITIONAL_DEFINITIONS})
        if (NOT ${lib}_HEADER_ONLY)
          list (APPEND LIBRARIES ${${lib}_LIBRARIES})
        endif ()
      else ()
        message (FATAL_ERROR "Failed to find required dependency: ${lib}")
      endif ()
    endif ()
  endforeach (lib)
endmacro (gather_libraries)
