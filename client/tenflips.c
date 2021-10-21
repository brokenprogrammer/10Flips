#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten.h>

typedef struct
{
    SDL_Window *Window;
    SDL_Renderer *Renderer;

    SDL_Texture *CardsTexture;
    SDL_Rect CardsTextureSize;

} GameState;

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
    GameState *State = Argument;

    SDL_Event Event;
    while (SDL_PollEvent(&Event)) 
    {
        HandleEvent(&Event);
    }


    SDL_RenderClear(State->Renderer);
    SDL_RenderCopy(State->Renderer, State->CardsTexture, NULL, &State->CardsTextureSize);
    SDL_RenderPresent(State->Renderer);
}

int 
main()
{
    GameState State = {};

    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(512, 512, 0, &State.Window, &State.Renderer);
    SDL_SetRenderDrawColor(State.Renderer, 255, 255, 255, 255);

    {
        SDL_Surface *Image = IMG_Load("assets/8BitDeckAssets.png");
        if (!Image)
        {
            printf("Failed to load image: %s\n", IMG_GetError());
            return 0;
        }

        State.CardsTexture = SDL_CreateTextureFromSurface(State.Renderer, Image);
        State.CardsTextureSize.w = Image->w;
        State.CardsTextureSize.h = Image->h;

        SDL_FreeSurface (Image);
    }


    // NOTE(Oskar): Schedule the main loop handler
    emscripten_set_main_loop_arg(Update, &State, -1, 1);

    return 0;
}