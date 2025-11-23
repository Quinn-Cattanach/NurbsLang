cmake_minimum_required(VERSION 3.20)
project(NurbsProject)
set(CMAKE_CXX_STANDARD 17)

# Add src headers
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Library path - MUST come before add_executable
link_directories(/opt/homebrew/lib)

# Add executable
file(GLOB SOURCES "${SRC_DIR}/*.cpp")
add_executable(nurbs ${SOURCES})

# Include paths
target_include_directories(nurbs PRIVATE
    ${SRC_DIR}                 # your own headers
    /opt/homebrew/include      # G+Smo + other deps
    /opt/homebrew/include/gismo
)

# Link libraries
target_link_libraries(nurbs gismo)