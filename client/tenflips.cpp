#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL_opengles2.h>
#include <emscripten.h>

#define STN_NO_SSE
#define STN_USE_MATH
#define STN_USE_STRING
#include "stn.h"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

struct game_state
{
    u32 WindowID;
    SDL_Window *Window;

    b32 MouseButtonDown;
    u32 MouseX;
    u32 MouseY;

    b32 FingerDown;
    f32 FingerDownX;
    f32 FingerDownY;

    SDL_Texture *CardsTexture;
    SDL_Rect CardsTextureSize;

};

// TODO(Oskar): Finish this.
int 
HandleEvent(SDL_Event *Event)
{
    int ShouldQuit = 0;

    switch(Event->type)
    {
        case SDL_QUIT:
        {
            printf("SDL_QUIT\n");
            ShouldQuit = 1;
        } break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            SDL_Keycode KeyCode = Event->key.keysym.sym;
            int WasDown = 0;
            if (Event->key.state == SDL_RELEASED)
            {
                WasDown = 1;
            }
            else if (Event->key.repeat != 0)
            {
                WasDown = 1;
            }

            if(KeyCode == SDLK_w)
            {
                printf("W\n");
            }

        } break;
    }

    return(ShouldQuit);
}

void
Update(void *Argument)
{
    game_state *State = (game_state *)Argument;

    SDL_Event Event;
    while (SDL_PollEvent(&Event)) 
    {
        HandleEvent(&Event);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    // Draw here

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


    // NOTE(Oskar): Setup OGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GLContext glc = SDL_GL_CreateContext(State.Window);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // NOTE(Oskar): Schedule the main loop handler
    emscripten_set_main_loop_arg(Update, &State, -1, 1);

    return 0;
}