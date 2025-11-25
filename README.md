
# NurbsLang

## Dependencies

This project assumes you are using homebrew on macos. These are the dependencies you must install:
```
brew install gismo
brew install emscripten
```

## Compilation

To compile the library, run:
```
cd nurbs
chmod +x ./build.sh
./build.sh
./build.sh wasm
```

Passing the `wasm` argument will build for wasm. This requires the gismo submodule at root.