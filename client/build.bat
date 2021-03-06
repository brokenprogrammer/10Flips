@echo off

if not exist build mkdir build
pushd build
call em++ ../tenflips.cpp ../ext/imgui.cpp ../ext/imgui_demo.cpp ../ext/imgui_draw.cpp ../ext/imgui_tables.cpp ../ext/imgui_widgets.cpp ../ext/imgui_impl_sdl.cpp ../ext/imgui_impl_opengl3.cpp -O3 -Wno-writable-strings -Wno-switch -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s WASM=1 -s FULL_ES3=1 -s USE_WEBGL2=1 -lwebsocket.js --preload-file assets/cardasset.png --preload-file assets/endturn.png --preload-file assets/yourturn.png --preload-file assets/youwon.png --preload-file assets/youlost.png --preload-file assets/background.png --use-preload-plugins --shell-file ../shell_minimal.html -o tenflips.html

popd