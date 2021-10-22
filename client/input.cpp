STN_INTERNAL int 
HandleEvent(SDL_Event *Event, input *Input)
{
    int ShouldQuit = 0;

    switch(Event->type)
    {
        case SDL_QUIT:
        {
            printf("SDL_QUIT\n");
            ShouldQuit = 1;
        } break;

        case SDL_MOUSEMOTION: 
        {
            SDL_MouseMotionEvent *m = (SDL_MouseMotionEvent*)Event;
            Input->MouseX = m->x;
            Input->MouseY = m->y;
            break;
        }

        case SDL_MOUSEBUTTONDOWN: 
        {
            SDL_MouseButtonEvent *m = (SDL_MouseButtonEvent*)Event;
            if (m->button == SDL_BUTTON_LEFT)
            {
                if (Input->LeftMouseButtonDown == false)
                {
                    Input->LeftMouseButtonPressed = true;
                }
                else
                {
                    Input->LeftMouseButtonPressed = false;
                }

                Input->LeftMouseButtonDown = true;
            }
            else if (m->button == SDL_BUTTON_RIGHT)
            {
                if (Input->RightMouseButtonDown == false)
                {
                    Input->RightMouseButtonPressed = true;
                }
                else
                {
                    Input->RightMouseButtonPressed = false;
                }

                Input->RightMouseButtonDown = true;
            }
        } break;

        case SDL_MOUSEBUTTONUP: 
        {
            SDL_MouseButtonEvent *m = (SDL_MouseButtonEvent*)Event;
            if (m->button == SDL_BUTTON_LEFT)
            {
                Input->LeftMouseButtonDown = false;
                Input->LeftMouseButtonPressed = false;
            }
            else if (m->button == SDL_BUTTON_RIGHT)
            {
                Input->RightMouseButtonDown = false;
                Input->RightMouseButtonPressed = false;
            }
        } break;

        case SDL_FINGERMOTION:
        {
            if (Input->FingerDown)
            {
                SDL_TouchFingerEvent *m = (SDL_TouchFingerEvent*)Event;

                if (m->fingerId == Input->FingerDownId)
                {
                    Input->FingerX = m->x;
                    Input->FingerY = m->y;
                }
            }
        } break;

        case SDL_FINGERDOWN:
        {
            // Finger already down means multiple fingers, which is handled by multigesture event
            if (Input->FingerDown)
            {
                Input->FingerDown = false;
            }
            else
            {
                SDL_TouchFingerEvent *m = (SDL_TouchFingerEvent*)Event;

                Input->FingerDown = true;
                Input->FingerX = m->x;
                Input->FingerY = m->y;
                Input->FingerDownId = m->fingerId;
            }
        } break; 

        case SDL_FINGERUP:
        {
            Input->FingerDown = false;
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
ResetInput(input *Input)
{
    if (Input->LeftMouseButtonPressed && Input->LeftMouseButtonDown == true)
    {
        Input->LeftMouseButtonPressed = false;
    }

    if (Input->RightMouseButtonPressed && Input->RightMouseButtonDown == true)
    {
        Input->RightMouseButtonPressed = false;
    }
}