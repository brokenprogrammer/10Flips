#include "ext/imgui.h"
#include "ext/imgui_impl_sdl.h"
#include "ext/imgui_impl_opengl3.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/websocket.h>

#define STN_NO_SSE
#define STN_USE_MATH
#define STN_USE_STRING
#define STN_USE_MEMORY
#include "stn.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define MEMORY_SIZE STN_Megabytes(2)

#include "state.h"
#include "assets.h"
#include "renderer.h"
#include "cards.h"

struct tenflips_state
{
    u32 WindowID;
    SDL_Window *Window;
    SDL_GLContext GLContext;

    // NOTE(Oskar): Memory
    u32 _MemorySize;
    void *_Memory;
    memory_arena StateArena;

    // NOTE(Oskar): Game State
    state_type GameState;
    state_type NextState;

    // NOTE(Oskar): Rendering
    renderer Renderer;
    texture Cards;

    // NOTE(Oskar): Networking
    EMSCRIPTEN_WEBSOCKET_T WebSocket;
};
STN_GLOBAL tenflips_state *GlobalState = NULL;

#include "state_menu.cpp"
#include "state_lobby.cpp"
#include "shader.cpp"
#include "assets.cpp"
#include "renderer.cpp"
#include "state_game.cpp"
#include "state.cpp"
#include "networking.cpp"

void
Update(void *Argument)
{
    tenflips_state *State = (tenflips_state *)Argument;

    ImGuiIO& io = ImGui::GetIO();

    static bool show_demo_window = false;

    SDL_Event Event;
    while (SDL_PollEvent(&Event)) 
    {
        ImGui_ImplSDL2_ProcessEvent(&Event);
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    state_type NextStateType = StateUpdate(State->GameState, &State->StateArena);
    if (State->NextState == STATE_TYPE_INVALID)
    {
        State->NextState = NextStateType;
    }

    if (State->NextState != STATE_TYPE_INVALID)
    {
        StateCleanUp(State->GameState, &State->StateArena);
        MemoryArenaZero(&State->StateArena);
        State->GameState = State->NextState;
        State->NextState = STATE_TYPE_INVALID;
        StateInit(State->GameState, &State->StateArena);
    }

    // if (show_demo_window)
    //     ImGui::ShowDemoWindow(&show_demo_window);


    // // NOTE(Oskar): IMGUI Rendering stuff.
    {
        ImGui::Render();
        SDL_GL_MakeCurrent(State->Window, State->GLContext);
        //glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    SDL_GL_SwapWindow(State->Window);
}

int 
main()
{
    tenflips_state State = {};

    // NOTE(Oskar): Check for websocket support. If not exist we error out.
    if (!emscripten_websocket_is_supported())
    {
        printf("No Websocket support detected. You can't play this game.\n");
        return 0;
    }

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    State.Window = SDL_CreateWindow("10Flips",
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    WINDOW_WIDTH, WINDOW_HEIGHT, 
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE| SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    State.WindowID = SDL_GetWindowID(State.Window);


    // NOTE(Oskar): Setup SDL + OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    
    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_DisplayMode Current;
    SDL_GetCurrentDisplayMode(0, &Current);

    SDL_GLContext glc = SDL_GL_CreateContext(State.Window);
    State.GLContext = glc;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(State.Window, glc);
    const char* glsl_version = "#version 300 es";
    ImGui_ImplOpenGL3_Init(glsl_version);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    InitializeRenderer(&State.Renderer);
    State.Cards = LoadTexture("assets/8BitDeckAssets.png");

    printf("Cards stuff: %d, %d\n", State.Cards.Width, State.Cards.Height);
    printf("Starting the game!\n");

    // NOTE(Oskar): Init memory
    {
        State._MemorySize = MEMORY_SIZE;
        State._Memory = calloc(1, State._MemorySize);

        State.StateArena = MemoryArenaInit(State._Memory, State._MemorySize);
        u32 StateArenaSize = State.StateArena.MemoryLeft;

        // MemoryArenaAllocate(&State.StateArena, StateArenaSize);
    }

    State.GameState = STATE_TYPE_MENU;
    State.NextState = STATE_TYPE_INVALID;
    StateInit(State.GameState, &State.StateArena);

    EmscriptenWebSocketCreateAttributes WebSocketAttributes = {
        "wss://localhost:44344/game",
        NULL,
        EM_TRUE
    };

    State.WebSocket = emscripten_websocket_new(&WebSocketAttributes);
    emscripten_websocket_set_onopen_callback(State.WebSocket, NULL, WebSocketOnOpen);
    emscripten_websocket_set_onerror_callback(State.WebSocket, NULL, WebSocketOnError);
    emscripten_websocket_set_onclose_callback(State.WebSocket, NULL, WebSocketOnClose);
    emscripten_websocket_set_onmessage_callback(State.WebSocket, NULL, WebSocketOnMessage);

    // NOTE(Oskar): Schedule the main loop handler
    GlobalState = &State;
    emscripten_set_main_loop_arg(Update, &State, -1, 1);

    return 0;
}