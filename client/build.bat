@echo off

if not exist build mkdir build
pushd build

call emcc ../tenflips.cpp -O2 -Wno-writable-strings -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s FULL_ES3=1 -s USE_WEBGL2=1 -s WASM=1  --preload-file assets/8BitDeckAssets.png --use-preload-plugins -o tenflips.html

popd