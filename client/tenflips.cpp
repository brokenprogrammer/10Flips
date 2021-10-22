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
#include "input.h"

#include "shader.cpp"
#include "assets.cpp"
#include "renderer.cpp"
#include "input.cpp"

struct game_state
{
    u32 WindowID;
    SDL_Window *Window;

    input Input;

    // NOTE(Oskar): Rendering
    renderer Renderer;

    texture Cards;
};

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

    SDL_Event Event;
    while (SDL_PollEvent(&Event)) 
    {
        HandleEvent(&Event, &State->Input);
    }

    if (State->Input.LeftMouseButtonPressed)
    {
        Type++;

        if (Type > CARD_TYPE_FLIPPED)
        {
            Type = 0;
        }
    }

    // Draw here
    BeginFrame(&State->Renderer);
    vector4 Destination = Vector4Init(0.0f, 0.0f, 100.0f, 150.0f);
    DrawCard(&State->Renderer, &State->Cards, (card_type)Type, Destination);

    // Destination = Vector4Init(105.0f, 0.0f, 100.0f, 150.0f);
    // DrawCard(&State->Renderer, &State->Cards, KING_OF_CLUBS, Destination);

    EndFrame(&State->Renderer, &State->Cards);

    ResetInput(&State->Input);
    SDL_GL_SwapWindow(State->Window);
}

int 
main()
{
    game_state State = {};

    SDL_Init(SDL_INIT_VIDEO);

    State.Window = SDL_CreateWindow("10Flips",
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    WINDOW_WIDTH, WINDOW_HEIGHT, 
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE| SDL_WINDOW_SHOWN);
    State.WindowID = SDL_GetWindowID(State.Window);


    // NOTE(Oskar): Setup SDL + OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GLContext glc = SDL_GL_CreateContext(State.Window);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    InitializeRenderer(&State.Renderer);
    State.Cards = LoadTexture("assets/8BitDeckAssets.png");

    printf("Cards stuff: %d, %d\n", State.Cards.Width, State.Cards.Height);
    printf("Starting the game!\n");

    // NOTE(Oskar): Schedule the main loop handler
    emscripten_set_main_loop_arg(Update, &State, -1, 1);

    return 0;
}