#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten.h>

typedef struct
{
    SDL_Renderer *Renderer;
    SDL_Texture *CardsTexture;
    SDL_Rect CardsTextureSize;

} GameState;

void
Update(void *Argument)
{
    GameState *State = Argument;

    SDL_RenderClear(State->Renderer);
    SDL_RenderCopy(State->Renderer, State->CardsTexture, NULL, &State->CardsTextureSize);
    SDL_RenderPresent(State->Renderer);
}

int 
main()
{
    SDL_Window *Window;
    GameState State = {};

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    SDL_CreateWindowAndRenderer(600, 400, 0, &Window, &State.Renderer);
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