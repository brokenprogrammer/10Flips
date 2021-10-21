@echo off

if not exist build mkdir build
pushd build

emcc ../tenflips.c -O2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' --preload-file assets/8BitDeckAssets.png --use-preload-plugins -o tenflips.html

popd