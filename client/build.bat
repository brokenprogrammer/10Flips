@echo off

if not exist build mkdir build
pushd build

emcc ../tenflips.c -s WASM=1 -o tenflips.html

popd