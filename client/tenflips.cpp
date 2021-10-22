#include "ext/imgui.h"
#include "ext/imgui_impl_sdl.h"
#include "ext/imgui_impl_opengl3.h"

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GLES3/gl3.h>
#include <emscripten.h>

#define STN_NO_SSE
#define STN_USE_MATH
#define STN_USE_STRING
#include "stn.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#include "assets.h"
#include "renderer.h"
#include "cards.h"

#include "shader.cpp"
#include "assets.cpp"
#include "renderer.cpp"

struct game_state
{
    u32 WindowID;
    SDL_Window *Window;
    SDL_GLContext GLContext;

    bool IsInMenu;
    bool IsCreatingGame;
    bool IsInLobby;
    bool HasCreatedGame;
    bool IsPlaying;

    // NOTE(Oskar): Rendering
    renderer Renderer;

    texture Cards;
};

#include "ui.cpp"

STN_INTERNAL void
DrawCard(renderer *Renderer, texture *Texture, card_type Type, vector4 Destination)
{
    vector4 Source = CardTextureOffset[Type];
    PushTexture(Renderer, Texture, Source, Destination);
}

u32 Type = 0;

void
Update(void *Argument)
{
    game_state *State = (game_state *)Argument;

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

    if (io.MouseClicked[0])
    {
        Type++;

        if (Type > CARD_TYPE_FLIPPED)
        {
            Type = 0;
        }
    }

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);


    if (State->IsInMenu)
    {
        UpdateAndRenderMenu(State);
    }

    if (State->IsCreatingGame)
    {
        UpdateAndRenderCreateGame(State);
    }

    if (State->IsInLobby)
    {
        UpdateAndRenderLobby(State);
    }

    // Rendering
    BeginFrame(&State->Renderer);

    // NOTE(Oskar): IMGUI Rendering stuff.
    {
        ImGui::Render();
        SDL_GL_MakeCurrent(State->Window, State->GLContext);
        //glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
    if (State->IsPlaying)
    {
        vector4 Destination = Vector4Init(0.0f, 0.0f, 100.0f, 150.0f);
        DrawCard(&State->Renderer, &State->Cards, (card_type)Type, Destination);
        EndFrame(&State->Renderer, &State->Cards);
    }

    SDL_GL_SwapWindow(State->Window);
}

int 
main()
{
    game_state State = {};

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

    State.IsInMenu = true;
    State.IsCreatingGame = false;
    State.HasCreatedGame = false;
    State.IsInLobby = false;
    State.IsPlaying = false;

    // NOTE(Oskar): Schedule the main loop handler
    emscripten_set_main_loop_arg(Update, &State, -1, 1);

    return 0;
}