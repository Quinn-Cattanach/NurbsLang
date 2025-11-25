# Install script for directory: /Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "static" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/lib/libgismo.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "static" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/lib/libgismo.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE FILE MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/src/gismo.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo/gsCore" TYPE FILE MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/gsCore/gsExport.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/external/gsEigen" REGEX "/[^/]*\\.txt$" EXCLUDE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/external/tclap" FILES_MATCHING REGEX "/[^/]*\\.h$" PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE FILE MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/external/eiquadprog.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo" TYPE FILE MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/external/gdcpp.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo/rapidxml" TYPE FILE MESSAGE_NEVER FILES
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/external/rapidxml/rapidxml.hpp"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/external/rapidxml/rapidxml_print.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gismodata" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/filedata/")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo/gsCore" TYPE FILE MESSAGE_NEVER RENAME "gsConfig.h" FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/gsCore/gsConfig_install.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gismo/gsCore" TYPE FILE MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/gsCore/gsConfigExt.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gismo" TYPE FILE MESSAGE_NEVER FILES
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/CMakeFiles/gismoConfig.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/gismoConfigVersion.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/gismoUse.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/gsConfig.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/gsDebugExtra.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/CodeCoverage.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/AddCompilerFlag.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/AddCXXCompileOptions.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/OptimizeForArchitecture.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/AddCXXCompilerFlag.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/CheckCXXCompilerFlag.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/AutodetectArm.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/AutodetectPpc.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/AutodetectX86.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/ChecksArm.txt"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/ChecksX86.txt"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/CommonMacros.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/HandleArmOptions.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/HandlePpcOptions.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/HandleX86Options.cmake"
    "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa/cpuinfo_x86.cxx"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE MESSAGE_NEVER RENAME "gismo.pc" FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/gismo.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gismo/" TYPE DIRECTORY MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/cmake/ofa" USE_SOURCE_PERMISSIONS)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "doc" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/gismo/" TYPE DIRECTORY OPTIONAL MESSAGE_NEVER FILES "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/doc/html/" USE_SOURCE_PERMISSIONS FILES_MATCHING REGEX "/[^/]*\\.css$" REGEX "/[^/]*\\.html$" REGEX "/[^/]*\\.js$" REGEX "/[^/]*\\.jpg$" REGEX "/[^/]*\\.png$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/external/cmake_install.cmake")
  include("/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/src/cmake_install.cmake")
  include("/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/doc/snippets/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
  file(WRITE "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/nurbs/build-wasm/gismo/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
