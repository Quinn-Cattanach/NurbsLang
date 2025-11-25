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
BUILD_DIR="$PROJECT_ROOT/build-$BUILD_TYPE"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

if [[ "$BUILD_TYPE" == "wasm" ]]; then
    # Check Emscripten
    if ! command -v emcc &> /dev/null; then
        echo -e "${RED}Emscripten not found!${NC}"
        exit 1
    fi
    echo -e "${GREEN}Building for WebAssembly...${NC}"

    emcmake cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Release
    emmake make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
else
    echo -e "${GREEN}Building for native platform...${NC}"

    cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

echo -e "${GREEN}Build complete!${NC}"
