@echo off

if not exist build mkdir build
pushd build

call em++ ../tenflips.cpp ../ext/imgui.cpp ../ext/imgui_demo.cpp ../ext/imgui_draw.cpp ../ext/imgui_tables.cpp ../ext/imgui_widgets.cpp ../ext/imgui_impl_sdl.cpp ../ext/imgui_impl_opengl3.cpp -O2 -Wno-writable-strings -Wno-switch -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s FULL_ES3=1 -s USE_WEBGL2=1 -s WASM=1  --preload-file assets/8BitDeckAssets.png --use-preload-plugins -o tenflips.html

popd