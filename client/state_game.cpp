#define CARD_WIDTH 50.0f
#define CARD_HEIGHT 100.0f

struct game_state
{
    game Game;
};

STN_INTERNAL void
StateGameInit(game_state *State)
{
    State->Game = {};
}

STN_INTERNAL void
StateGameCleanup(game_state *State)
{
}

STN_INTERNAL void
DrawCard(renderer *Renderer, texture *Texture, card_type Type, vector4 Destination)
{
    vector4 Source = CardTextureOffset[Type];
    PushTexture(Renderer, Texture, Source, Destination);
}

STN_INTERNAL void
DrawCardAngle(renderer *Renderer, texture *Texture, card_type Type, vector4 Destination, f32 Angle)
{
    vector2 Center = { CARD_WIDTH / 2.0f, CARD_HEIGHT / 2.0f };
    vector4 Source = CardTextureOffset[Type];

    PushTextureAngle(Renderer, Texture, Source, Destination, Center, Angle);
}

STN_INTERNAL void
ProcessGameEvents(game_state *State)
{
    for (u32 Index = 0; Index < GlobalState->MessageCount; ++Index)
    {
        message Message = GlobalState->Messages[Index];

        switch (Message.Type)
        {
            case MESSAGE_TYPE_GAME_INIT:
            case MESSAGE_TYPE_GAME_UPDATE:
            {
                State->Game = Message.GameInit;
            } break;

            case MESSAGE_TYPE_GAME_END_TURN:
            {
                State->Game.YourTurn = Message.MyTurn;
            };

            default:
            {
                // NOTE(Oskar): Ignore for now..
            }
        }
    }

    GlobalState->MessageCount = 0;
}

STN_INTERNAL state_type
StateGameUpdate(game_state *State)
{
    state_type NextState = STATE_TYPE_INVALID;
    ImGuiIO& io = ImGui::GetIO();

    ProcessGameEvents(State);

    BeginFrame(&GlobalState->Renderer);

    // NOTE(Oskar): End Turn Button
    {
        vector4 Source = Vector4Init(0, 0, GlobalState->EndTurn.Width, GlobalState->EndTurn.Height);
        vector4 Destination = Vector4Init(0, 0, 250, 100);

        PushTexture(&GlobalState->Renderer, &GlobalState->EndTurn, 
                    Source, Destination);
        if (io.MouseClicked[0])
        {
            vector2 ClickPosition = Vector2Init(io.MouseClickedPos[0].x, io.MouseClickedPos[0].y);
            if (Vector4RectangleHasPoint(Destination, ClickPosition))
            {
                char Buffer[80];
                sprintf(Buffer, "endturn");
                emscripten_websocket_send_utf8_text(GlobalState->WebSocket, Buffer);
            }
        }
    }

    // NOTE(Oskar): Draw TableCard
    {
        vector4 Destination = Vector4Init(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, CARD_WIDTH, CARD_HEIGHT);
        DrawCard(&GlobalState->Renderer, &GlobalState->Cards, State->Game.TableCard, Destination);

        if (io.MouseClicked[0])
        {
            vector2 ClickPosition = Vector2Init(io.MouseClickedPos[0].x, io.MouseClickedPos[0].y);
            if (Vector4RectangleHasPoint(Destination, ClickPosition))
            {
                char Buffer[80];
                sprintf(Buffer, "pickup");
                emscripten_websocket_send_utf8_text(GlobalState->WebSocket, Buffer);
            }
        }
    }

    // NOTE(Oskar): Draw player
    {
        f32 CardStartX = (WINDOW_WIDTH / 2.0) - (State->Game.NumberOfCards / 2) * CARD_WIDTH;
        for (u32 Index = 0; Index < State->Game.NumberOfCards; ++Index)
        {
            vector4 Destination = Vector4Init(CardStartX, WINDOW_HEIGHT - CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
            DrawCard(&GlobalState->Renderer, &GlobalState->Cards, State->Game.Hand[Index], Destination);

            if (io.MouseClicked[0])
            {
                vector2 ClickPosition = Vector2Init(io.MouseClickedPos[0].x, io.MouseClickedPos[0].y);
                if (Vector4RectangleHasPoint(Destination, ClickPosition))
                {
                    char Buffer[80];
                    sprintf(Buffer, "place:%d", State->Game.Hand[Index]);
                    emscripten_websocket_send_utf8_text(GlobalState->WebSocket, Buffer);

                    printf("You clicked on your cards!\n");
                }
            }

            CardStartX += CARD_WIDTH;
        }

        CardStartX = (WINDOW_WIDTH / 2.0) - (State->Game.NumberOfCards / 2) * CARD_WIDTH;
        for (u32 Index = 0; Index < 3; ++Index)
        {
            vector4 Destination = Vector4Init(CardStartX, WINDOW_HEIGHT - CARD_HEIGHT * 2, CARD_WIDTH, CARD_HEIGHT);
            
            // if (State->Game.Player.BottomCards[Index] != CARD_TYPE_NULL)
            // {
                DrawCard(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination);
            // }
            if (State->Game.TopCards[Index] != CARD_TYPE_NULL)
            {
                DrawCard(&GlobalState->Renderer, &GlobalState->Cards, State->Game.TopCards[Index], Destination);
            }

            if (io.MouseClicked[0])
            {
                vector2 ClickPosition = Vector2Init(io.MouseClickedPos[0].x, io.MouseClickedPos[0].y);
                if (Vector4RectangleHasPoint(Destination, ClickPosition))
                {
                    card_type ClickedCard = CARD_TYPE_NULL;
                    if (State->Game.TopCards[Index] == CARD_TYPE_NULL)
                    {
                        // TODO(Oskar): Handle bottom cards
                        // if (State->Game.Player.BottomCards[Index] != CARD_TYPE_NULL)
                        // {
                        //     ClickedCard = State->Game.Player.BottomCards[Index];
                        // }   
                    }
                    else
                    {
                        ClickedCard = State->Game.TopCards[Index];
                    }
                    char Buffer[80];
                    sprintf(Buffer, "place:%d", ClickedCard);
                    emscripten_websocket_send_utf8_text(GlobalState->WebSocket, Buffer);

                    printf("You clicked on your top or bottom cards!\n");
                }
            }

            CardStartX += CARD_WIDTH;
        }
    }

    for (u32 OpponentIndex = 0; OpponentIndex < State->Game.OpponentCount; ++OpponentIndex)
    {

        // NOTE(Oskar): Top player
        if (OpponentIndex == 0)
        {
            f32 CardStartX = (WINDOW_WIDTH / 2.0) - (State->Game.Opponents[OpponentIndex].NumberOfCards / 2) * CARD_WIDTH;

            // NOTE(Oskar): Render hand
            for (u32 Index = 0; Index < State->Game.Opponents[OpponentIndex].NumberOfCards; ++Index)
            {
                vector4 Destination = Vector4Init(CardStartX,  0, CARD_WIDTH, CARD_HEIGHT);
                DrawCard(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination);

                CardStartX += CARD_WIDTH;
            }

            CardStartX = (WINDOW_WIDTH / 2.0) - (3 / 2) * CARD_WIDTH;
            // NOTE(Oskar): Render top and bottom cards
            for (u32 Index = 0; Index < 3; ++Index)
            {
                vector4 Destination = Vector4Init(CardStartX, CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
                
                // if (State->Game.BottomCards[Index] != CARD_TYPE_NULL)
                // {
                    DrawCard(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination);
                // }
                if (State->Game.Opponents[OpponentIndex].TopCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCard(&GlobalState->Renderer, &GlobalState->Cards, State->Game.Opponents[OpponentIndex].TopCards[Index], Destination);
                }

                CardStartX += CARD_WIDTH;
            }
        }
        else if (OpponentIndex == 1) // Left Player
        {
            f32 CardStartX = CARD_WIDTH;
            f32 CardStartY = (WINDOW_HEIGHT / 2.0f) - (State->Game.Opponents[OpponentIndex].NumberOfCards / 2) * CARD_WIDTH;

            // NOTE(Oskar): Render hand
            for (u32 Index = 0; Index < State->Game.Opponents[OpponentIndex].NumberOfCards; ++Index)
            {
                vector4 Destination = Vector4Init(CardStartX, CardStartY, CARD_WIDTH, CARD_HEIGHT);
                DrawCardAngle(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination, 1.57079633f);

                CardStartY += CARD_WIDTH;
            }

            CardStartX = CardStartY = (WINDOW_HEIGHT / 2.0f) - (3 / 2) * CARD_WIDTH;
            // NOTE(Oskar): Render top and bottom cards
            for (u32 Index = 0; Index < 3; ++Index)
            {
                vector4 Destination = Vector4Init(CARD_WIDTH + CARD_HEIGHT, CardStartY, CARD_WIDTH, CARD_HEIGHT);
                
                // if (State->Game.Player.BottomCards[Index] != CARD_TYPE_NULL)
                // {
                    DrawCardAngle(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination, 1.57079633f);
                // }
                if (State->Game.Opponents[OpponentIndex].TopCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCardAngle(&GlobalState->Renderer, &GlobalState->Cards, State->Game.Opponents[OpponentIndex].TopCards[Index], Destination, 1.57079633f);
                }

                CardStartY += CARD_WIDTH;
            }
        }
        else if (OpponentIndex == 2) // Right player
        {
            f32 CardStartX = WINDOW_WIDTH - CARD_HEIGHT;
            f32 CardStartY = (WINDOW_HEIGHT / 2.0f) - (State->Game.Opponents[OpponentIndex].NumberOfCards / 2) * CARD_WIDTH;

            // NOTE(Oskar): Render hand
            for (u32 Index = 0; Index < State->Game.Opponents[OpponentIndex].NumberOfCards; ++Index)
            {
                vector4 Destination = Vector4Init(CardStartX, CardStartY, CARD_WIDTH, CARD_HEIGHT);
                DrawCardAngle(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination, 1.57079633f);

                CardStartY += CARD_WIDTH;
            }

            CardStartY = (WINDOW_HEIGHT / 2.0f) - (3 / 2) * CARD_WIDTH;
            // NOTE(Oskar): Render top and bottom cards
            for (u32 Index = 0; Index < 3; ++Index)
            {
                vector4 Destination = Vector4Init(CardStartX - CARD_HEIGHT, CardStartY, CARD_WIDTH, CARD_HEIGHT);
                
                // if (State->Game.Player.BottomCards[Index] != CARD_TYPE_NULL)
                // {
                    DrawCardAngle(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination, 1.57079633f);
                // }
                if (State->Game.Opponents[OpponentIndex].TopCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCardAngle(&GlobalState->Renderer, &GlobalState->Cards, State->Game.Opponents[OpponentIndex].TopCards[Index], Destination, 1.57079633f);
                }

                CardStartY += CARD_WIDTH;
            }
        }
    }

    EndFrame(&GlobalState->Renderer);
    return NextState;
}