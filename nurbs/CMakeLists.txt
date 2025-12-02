cmake_minimum_required(VERSION 3.20)
project(NurbsProject)
set(CMAKE_CXX_STANDARD 17)

# Claude wrote this

# Force CMake to NOT use response files
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS OFF)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_LIBRARIES OFF)

# Add src headers
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Add executable sources
file(GLOB SOURCES "${SRC_DIR}/*.cpp")

# Detect if we're building for WebAssembly
if(EMSCRIPTEN)
    message(STATUS "Building for WebAssembly with Emscripten")

    # New location: gismo is one level above nurbs/
    set(GISMO_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../gismo" CACHE PATH "G+Smo source directory")

    # Build dir should also be next to gismo
    set(GISMO_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/gismo" CACHE PATH "G+Smo build directory")

    # Check if gismo.h exists
    if(EXISTS "${GISMO_SOURCE_DIR}/src/gismo.h")
        message(STATUS "✓ Found gismo.h at ${GISMO_SOURCE_DIR}/src/gismo.h")
    else()
        message(FATAL_ERROR "✗ gismo.h NOT found at ${GISMO_SOURCE_DIR}/src/gismo.h")
    endif()

    # Use include_directories() instead of target_include_directories()
    # This applies globally and is more reliable with some toolchains
    # IMPORTANT: Include parent directory of 'src' so that <gismo/gismo.h> resolves correctly
    include_directories(
        ${SRC_DIR}
        ${GISMO_SOURCE_DIR}
        ${GISMO_SOURCE_DIR}/src
        ${GISMO_SOURCE_DIR}/external
        ${GISMO_SOURCE_DIR}/external/gsEigen
        ${GISMO_BUILD_DIR}
        ${GISMO_BUILD_DIR}/gsCore
        ${GISMO_SOURCE_DIR}/optional
        ${GISMO_SOURCE_DIR}/optional/gsElasticity/src
    )

    # Print include directories for debugging
    message(STATUS "Include directories:")
    message(STATUS "  ${SRC_DIR}")
    message(STATUS "  ${GISMO_SOURCE_DIR} (parent for gismo/...)")
    message(STATUS "  ${GISMO_SOURCE_DIR}/external")
    message(STATUS "  ${GISMO_SOURCE_DIR}/external/gsEigen")
    message(STATUS "  ${GISMO_BUILD_DIR}")
    message(STATUS "  ${GISMO_BUILD_DIR}/gsCore")
    message(STATUS "  ${GISMO_SOURCE_DIR}/optional")

    # Add executable
    add_executable(nurbs ${SOURCES})

    # Emscripten-specific compiler and linker flags
    set(EMSCRIPTEN_COMPILE_FLAGS
        "-fexceptions"
    )

    # ---------------------------------------------------------
    # UPDATED LINKER FLAGS BELOW
    # ---------------------------------------------------------
    set(EMSCRIPTEN_LINK_FLAGS
        "-s WASM=1"
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s MODULARIZE=1"
        "-s EXPORT_NAME='createNurbsModule'"
        # --- FIX: removed malloc/free from this list ---
        "SHELL:-s EXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"FS\",\"HEAPF32\",\"HEAPU32\",\"HEAP8\"]"
        # -----------------------------------------------
        "SHELL:-s EXPORTED_FUNCTIONS=[\"_main\",\"_malloc\",\"_free\"]"
        "-s DISABLE_EXCEPTION_CATCHING=0"
        "-s ASSERTIONS=1"
        "--bind"
        "-fexceptions"
    )

    # Optimization flags for release builds
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        list(APPEND EMSCRIPTEN_LINK_FLAGS
            "-O3"
            "-s ELIMINATE_DUPLICATE_FUNCTIONS=1"
            "-s AGGRESSIVE_VARIABLE_ELIMINATION=1"
        )
    else()
        list(APPEND EMSCRIPTEN_LINK_FLAGS
            "-O1"
            "-g"
        )
    endif()

    # Apply Emscripten flags
    target_compile_options(nurbs PRIVATE ${EMSCRIPTEN_COMPILE_FLAGS})
    target_link_options(nurbs PRIVATE ${EMSCRIPTEN_LINK_FLAGS})
    # target_compile_definitions(nurbs PRIVATE OPTIMIZER_ENABLED)

    # Set output file extensions
    set_target_properties(nurbs PROPERTIES
        SUFFIX ".js"
        OUTPUT_NAME "nurbs"
    )

    # Find and link G+Smo library
    if(EXISTS "/Users/quinncattanach/classes/domain_specific_language/NurbsLang/gismo/build-wasm/lib/libgismo.a")
        message(STATUS "✓ Found libgismo.a at ${GISMO_BUILD_DIR}/lib/libgismo.a")
        target_link_libraries(nurbs ${GISMO_BUILD_DIR}/lib/libgismo.a)
    else()
        message(FATAL_ERROR "✗ libgismo.a NOT found at ${GISMO_BUILD_DIR}/lib/libgismo.a")
    endif()

else()
    message(STATUS "Building for native platform")

    set(GISMO_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../gismo")
    set(GISMO_BUILD_DIR "${GISMO_SOURCE_DIR}/build")

    link_directories(/opt/homebrew/lib)
    link_directories(${GISMO_BUILD_DIR}/lib)

    add_executable(nurbs ${SOURCES})
    target_compile_definitions(nurbs PRIVATE OPTIMIZER_ENABLED)

    target_include_directories(nurbs PRIVATE
        ${SRC_DIR}
        /opt/homebrew/include
        /opt/homebrew/include/gismo
        ${GISMO_BUILD_DIR}/optional/gsElasticity/src/
        # Add gsElasticity from source tree (relative path)
        ${GISMO_SOURCE_DIR}/optional
        # Also add main gismo includes
        ${GISMO_SOURCE_DIR}/src
        ${GISMO_BUILD_DIR}
    )

    message(STATUS "Native build paths:")
    message(STATUS "  GISMO_SOURCE_DIR: ${GISMO_SOURCE_DIR}")
    message(STATUS "  gsElasticity: ${GISMO_SOURCE_DIR}/optional")

    target_link_libraries(nurbs gismo)
    target_link_libraries(nurbs "${GISMO_BUILD_DIR}/lib/libgismo.dylib")
endif()
