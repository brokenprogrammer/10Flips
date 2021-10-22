#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GLES3/gl3.h>
#include <emscripten.h>

#define STN_NO_SSE
#define STN_USE_MATH
#define STN_USE_STRING
#include "stn.h"

#include "shader.cpp"

#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 512

struct texture
{
    GLuint Id;
    u32 Width;
    u32 Height;

    b32 IsLoaded;
};

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

    // NOTE(Oskar): Rendering
    GLuint TextureShader;

    u32 PushedJobs;

    GLuint TextureVAO;
    GLuint TextureInstanceBuffer;
    u32 TextureInstanceDataAllocPosition;
    // NOTE(Oskar): Max textures * number of inputs
    GLubyte TextureInstanceData[32768 *  (sizeof(f32) * 16)];

    texture Cards;
};

void 
PushTexture(game_state *State, texture *Texture, 
    vector4 Source, vector4 Destination)
{
    vector3 p1 = { Destination.X, Destination.Y };
    vector3 p2 = { Destination.X, Destination.Y + Destination.Height };
    vector3 p3 = { Destination.X + Destination.Width, Destination.Y };
    vector3 p4 = { Destination.X + Destination.Width, Destination.Y + Destination.Height };

    GLubyte *Data = (State->TextureInstanceData + State->TextureInstanceDataAllocPosition);
    ((f32 *)Data)[0]  = Source.X;
    ((f32 *)Data)[1]  = Source.Y;
    ((f32 *)Data)[2]  = Source.Width;
    ((f32 *)Data)[3]  = Source.Height;
    ((f32 *)Data)[4]  = p1.X;
    ((f32 *)Data)[5]  = p1.Y;
    ((f32 *)Data)[6]  = p1.Z;
    ((f32 *)Data)[7]  = p2.X;
    ((f32 *)Data)[8]  = p2.Y;
    ((f32 *)Data)[9]  = p2.Z;
    ((f32 *)Data)[10] = p3.X;
    ((f32 *)Data)[11] = p3.Y;
    ((f32 *)Data)[12] = p3.Z;
    ((f32 *)Data)[13] = p4.X;
    ((f32 *)Data)[14] = p4.Y;
    ((f32 *)Data)[15] = p4.Z;

    State->TextureInstanceDataAllocPosition += (sizeof(f32) * 16);
    State->PushedJobs++;
}

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

STN_INTERNAL void
BeginFrame(game_state *State)
{
    State->PushedJobs = 0;
    State->TextureInstanceDataAllocPosition = 0;
}

STN_INTERNAL void
EndFrame(game_state *State)
{
    matrix4 ViewProjection  = Matrix4Orthographic(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1.0f, 1.0f);

    for (u32 Index = 0; Index < State->PushedJobs; ++Index)
    {
        {
            glBindBuffer(GL_ARRAY_BUFFER, State->TextureInstanceBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (sizeof(f32) * 16), 
                            State->TextureInstanceData + (Index * (sizeof(f32) * 16)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        glBindVertexArray(State->TextureVAO);
        glUseProgram(State->TextureShader);
        {
            glUniformMatrix4fv(glGetUniformLocation(State->TextureShader, "view_projection"), 1, GL_FALSE,
                               &ViewProjection.Elements[0][0]);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, State->Cards.Id);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glUniform1i(glGetUniformLocation(State->TextureShader, "tex"), 0);
            glUniform2f(glGetUniformLocation(State->TextureShader, "tex_resolution"),
                        (f32)State->Cards.Width,
                        (f32)State->Cards.Height);
            
            GLint First = 0;
            GLsizei Count = 4;
            GLsizei InstanceCount = (sizeof(f32) * 16) / (sizeof(f32) * 16);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, First, Count, InstanceCount);
        }
        glBindVertexArray(0);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
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
    BeginFrame(State);
    vector4 Source = Vector4Init(0, 0, 35, 47);
    vector4 Destination = Vector4Init(0.0f, 0.0f, 100.0f, 150.0f);
    PushTexture(State, &State->Cards, Source, Destination);

    Source = Vector4Init(35, 0, 35, 47);
    Destination = Vector4Init(105.0f, 0.0f, 100.0f, 150.0f);
    PushTexture(State, &State->Cards, Source, Destination);
    EndFrame(State);

    SDL_GL_SwapWindow(State->Window);
}

void
InitGLStuff(game_state *State)
{
    glGenVertexArrays(1, &State->TextureVAO);
    glBindVertexArray(State->TextureVAO);
    {
        glGenBuffers(1, &State->TextureInstanceBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, State->TextureInstanceBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(State->TextureInstanceData), 0, GL_DYNAMIC_DRAW);
        

        i32 Stride = (sizeof(f32) * 16);
        u32 Offset = 0;
        {
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, Stride,
                          (void *)(sizeof(f32)*0));
            glVertexAttribDivisor(0, 1);

            Offset += 4;
        }
        {
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Stride,
                          (void *)(sizeof(f32)*Offset));
            glVertexAttribDivisor(1, 1);

            Offset += 3;
        }
        {
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, Stride,
                          (void *)(sizeof(f32)*Offset));
            glVertexAttribDivisor(2, 1);

            Offset += 3;
        }
        {
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, Stride,
                          (void *)(sizeof(f32)*Offset));
            glVertexAttribDivisor(3, 1);

            Offset += 3;
        }
        {
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, Stride,
                          (void *)(sizeof(f32)*Offset));
            glVertexAttribDivisor(4, 1);

            Offset += 3;
        }
    }
    glBindVertexArray(0);
}

texture
LoadTexture(game_state *State)
{
    texture Texture = {};
    Texture.IsLoaded = false;

    SDL_Surface *Image = IMG_Load("assets/8BitDeckAssets.png");

    if(!Image)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
        return (Texture);
    }
    
    int bitsPerPixel = Image->format->BitsPerPixel;
    printf("Image dimensions %dx%d, %d bits per pixel\n", Image->w, Image->h, bitsPerPixel);

    Texture.Width = Image->w;
    Texture.Height = Image->h;

    GLint format = -1;
    if (bitsPerPixel == 24)
        format = GL_RGB;
    else if (bitsPerPixel == 32)
        format = GL_RGBA;

    if (format != -1)
    {
        glGenTextures(1, &Texture.Id);
        glBindTexture(GL_TEXTURE_2D, Texture.Id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, Texture.Width, Texture.Height, 0, format,
                     GL_UNSIGNED_BYTE, Image->pixels);

        glBindTexture(GL_TEXTURE_2D, 0);
        Texture.IsLoaded = true;

        printf("Texture successfully loaded!\n");
    }

    SDL_FreeSurface(Image);

    return (Texture);
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

    State.TextureShader = CreateShader();
    InitGLStuff(&State);
    texture Texture = LoadTexture(&State);
    State.Cards = Texture;

    printf("Cards stuff: %d, %d\n", State.Cards.Width, State.Cards.Height);
    printf("Starting the game!\n");

    // NOTE(Oskar): Schedule the main loop handler
    emscripten_set_main_loop_arg(Update, &State, -1, 1);

    return 0;
}