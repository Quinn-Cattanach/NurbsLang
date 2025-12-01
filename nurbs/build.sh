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

    # --- NEW: Copy artifacts to external folder ---
    TARGET_DIR="$PROJECT_ROOT/../nurbs-lang-editor/public/external"

    echo -e "${YELLOW}Copying WASM artifacts to: $TARGET_DIR${NC}"

    # Create the directory if it doesn't exist
    mkdir -p "$TARGET_DIR"

    # Copy generated .wasm and .js files
    # We use || true to prevent script failure if no files are found (though they should be there)
    cp *.wasm *.js "$TARGET_DIR/" || echo -e "${RED}Warning: No .wasm/.js files found to copy.${NC}"
    # ----------------------------------------------

else
    echo -e "${GREEN}Building for native platform...${NC}"

    cmake "$PROJECT_ROOT" -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

echo -e "${GREEN}Build complete!${NC}"
