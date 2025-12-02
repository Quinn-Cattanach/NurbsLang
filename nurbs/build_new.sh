#!/bin/bash
# build.sh — build NURBS project
set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

BUILD_TYPE="native"

# Check first argument
if [[ "$1" == "wasm" ]]; then
    BUILD_TYPE="wasm"
fi

PROJECT_ROOT="$(pwd)"
GISMO_ROOT="$PROJECT_ROOT/../gismo"
GISMO_BUILD="$GISMO_ROOT/build-$BUILD_TYPE"
BUILD_DIR="$PROJECT_ROOT/build-$BUILD_TYPE"

mkdir -p "$BUILD_DIR"
mkdir -p "$GISMO_BUILD"

echo -e "${YELLOW}Rebuilding G+Smo (with optional modules)…${NC}"
cd "$GISMO_BUILD"

if [[ "$BUILD_TYPE" == "wasm" ]]; then
    # --- G+Smo WASM build ---
    if ! command -v emcc &> /dev/null; then
        echo -e "${RED}Emscripten not found!${NC}"
        exit 1
    fi
    echo -e "${GREEN}Configuring G+Smo for WebAssembly...${NC}"

    # Must wipe CMake cache so it re-detects gsElasticity
    rm -rf CMakeCache.txt CMakeFiles/

    emcmake cmake "$GISMO_ROOT" -DCMAKE_BUILD_TYPE=Release
    emmake make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

else
    # --- G+Smo native build ---
    echo -e "${GREEN}Configuring G+Smo for native build...${NC}"

    rm -rf CMakeCache.txt CMakeFiles/

    cmake "$GISMO_ROOT" -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

echo -e "${GREEN}G+Smo build complete.${NC}"

# ---------------------------
# Now build NurbsProject
# ---------------------------
cd "$BUILD_DIR"

if [[ "$BUILD_TYPE" == "wasm" ]]; then
    echo -e "${GREEN}Building NurbsProject for WebAssembly...${NC}"

    emcmake cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Release
    emmake make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

    TARGET_DIR="$PROJECT_ROOT/../nurbs-lang-editor/public/external"
    echo -e "${YELLOW}Copying WASM artifacts to: $TARGET_DIR${NC}"

    mkdir -p "$TARGET_DIR"
    cp *.wasm *.js "$TARGET_DIR/" || echo -e "${RED}Warning: No .wasm/.js files found to copy.${NC}"

else
    echo -e "${GREEN}Building NurbsProject for native platform...${NC}"

    cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

echo -e "${GREEN}Build complete!${NC}"
