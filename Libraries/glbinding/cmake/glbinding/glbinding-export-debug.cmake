#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "glbinding::glbinding" for configuration "Debug"
set_property(TARGET glbinding::glbinding APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(glbinding::glbinding PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/glbindingd.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/./glbindingd.dll"
  )

list(APPEND _cmake_import_check_targets glbinding::glbinding )
list(APPEND _cmake_import_check_files_for_glbinding::glbinding "${_IMPORT_PREFIX}/lib/glbindingd.lib" "${_IMPORT_PREFIX}/./glbindingd.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
