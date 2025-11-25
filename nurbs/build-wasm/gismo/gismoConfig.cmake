######################################################################
## gismoConfig.cmake --- G+Smo CMake Configuration file
## This file is part of the G+Smo library.
##
## https://github.com/gismo/gismo
## https://gismo.github.io/
##
## Author: Angelos Mantzaflaris
######################################################################

######################################################################
##
## Config file for the G+SMO library
## It defines the following variables
## GISMO_VERSION      - version string of G+SMO
## GISMO_INCLUDE_DIRS - include directories for G+SMO
## GISMO_LIBRARIES    - libraries to link against
## GISMO_MODULE_PATH  - path containing cmake modules
## GISMO_USE_FILE     - the cmake use-file
## GISMO_BUILD_TYPE   - the cmake build type used to compile G+Smo
##
######################################################################

# Compute paths
#get_filename_component(GISMO_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

# Our library dependencies (contains definitions for IMPORTED targets)
#if(NOT TARGET gismo AND NOT gismo_BINARY_DIR)
#  include("${GISMO_CMAKE_DIR}/gismoTargets.cmake")
#endif()

# These are IMPORTED targets created by gismoTargets.cmake (add TARGETS gsView ...)

set(GISMO_VERSION       "25.7.0")
set(GISMO_VERSION_MAJOR "25")
set(GISMO_VERSION_MINOR "7")
set(GISMO_VERSION_PATCH "0")
set(gismo_VERSION       "25.7.0")
set(gismo_VERSION_MAJOR "25")
set(gismo_VERSION_MINOR "7")
set(gismo_VERSION_PATCH "0")

set(GISMO_INCLUDE_DIRS "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src;/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/external;/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo;/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/optional")
set(GISMO_PUBLIC_HEADER "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gismo.h")
set(GISMO_LIBRARY_DIR  "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/lib")
set(GISMO_LIBRARIES           gismo)
set(GISMO_STATIC_LIBRARIES    gismo_static)
set(TARGET_ARCHITECTURE "none")
set(GISMO_MODULE_PATH  "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake")
set(GISMO_USE_FILE     "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/gismoUse.cmake")
set(GISMO_BUILD_TYPE   "Release")

if(NOT GISMO_IS_CONFIGURED_MESSAGE)
  message(STATUS "G+Smo is configured at ${GISMO_LIBRARY_DIR}")
  set(GISMO_IS_CONFIGURED_MESSAGE 1 CACHE INTERNAL "Display once G+Smo library path")
endif()

if(NOT CMAKE_BUILD_TYPE)
   # Set automatically the build type
   set(CMAKE_BUILD_TYPE ${GISMO_BUILD_TYPE} CACHE STRING
   "Type of build (set to comply with G+Smo library)" FORCE)
elseif(NOT "x${CMAKE_BUILD_TYPE}" STREQUAL "x${GISMO_BUILD_TYPE}")
  message(WARNING "G+Smo is configured in ${GISMO_BUILD_TYPE} mode which does not coincide with this project! Conflicts may occur. Re-run CMake with CMAKE_BUILD_TYPE=${GISMO_BUILD_TYPE} to fix this, or update gismo_DIR.")
endif()

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${GISMO_MODULE_PATH})
