# Install script for directory: /Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/opt/homebrew/Cellar/emscripten/4.0.20/libexec/cache/sysroot")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsAssembler" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsCore" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsDomain" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsExpressions" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsHSplines" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsIO" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsIeti" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsMSplines" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsMatrix" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsMesh2" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsModeling" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsMultiGrid" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsNurbs" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsOptimizer" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsParallel" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsPde" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsSolver" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsTensor" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gsUtils" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

