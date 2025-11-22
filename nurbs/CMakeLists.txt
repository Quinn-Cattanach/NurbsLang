cmake_minimum_required(VERSION 3.16)
project(nurbs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GISMO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../gismo")
set(GISMO_BUILD "${GISMO_ROOT}/build")

set(GISMO_INCLUDE_DIR "${GISMO_ROOT}/src")
set(GISMO_LIB_DIR "${GISMO_BUILD}/lib")

file(GLOB_RECURSE NURBS_SRC "src/*.cpp" "src/*.h")

add_executable(nurbs_app ${NURBS_SRC})

target_include_directories(nurbs_app PRIVATE
    "${GISMO_ROOT}/src"
    "${GISMO_ROOT}/external/gsEigen/src"
)

file(GLOB GISMO_LIBS "${GISMO_LIB_DIR}/*.a")

target_link_libraries(nurbs_app PRIVATE
    ${GISMO_LIBS}
)

target_compile_definitions(nurbs_app PRIVATE
    EIGEN_NO_DEBUG
)

message(STATUS "Using gismo from: ${GISMO_ROOT}")
message(STATUS "Using gismo libs: ${GISMO_LIBS}")
