#define CARD_WIDTH 50.0f
#define CARD_HEIGHT 100.0f

struct player
{
    char *Name;
    card_type Hand[52];
    u32 NumberOfCards;

    card_type TopCards[3];
    card_type BottomCards[3];
};

struct opponent
{
    char *Name;

    u32 NumberOfCards;

    card_type TopCards[3];
    card_type BottomCards[3];
};

struct game
{
    char *Name;
    opponent Opponents[3];
    u32 OpponentCount;

    card_type TopCard;

    player Player;
};

struct game_state
{
    game Game;
};

STN_INTERNAL void
StateGameInit(game_state *State)
{
    State->Game = {};
    State->Game.Player = {};
    State->Game.Player.Hand[0] = FIVE_OF_CLUBS;
    State->Game.Player.Hand[1] = FOUR_OF_HEARTS;
    State->Game.Player.Hand[2] = SIX_OF_DIAMONDS;
    State->Game.Player.NumberOfCards = 3;
    State->Game.Player.TopCards[0] = KING_OF_CLUBS;
    State->Game.Player.TopCards[1] = CARD_TYPE_NULL;
    State->Game.Player.TopCards[2] = JACK_OF_DIAMONDS;
    State->Game.Player.BottomCards[0] = TWO_OF_SPADES;
    State->Game.Player.BottomCards[1] = NINE_OF_SPADES;
    State->Game.Player.BottomCards[2] = TEN_OF_CLUBS;

    State->Game.OpponentCount = 3;

    opponent One = { "", 4, {FIVE_OF_CLUBS, FIVE_OF_CLUBS, FIVE_OF_CLUBS}, {FIVE_OF_CLUBS, FIVE_OF_CLUBS ,FIVE_OF_CLUBS}};
    opponent Two = { "", 4, {FIVE_OF_CLUBS, FIVE_OF_CLUBS, FIVE_OF_CLUBS}, {FIVE_OF_CLUBS, FIVE_OF_CLUBS ,FIVE_OF_CLUBS}};
    opponent Three = { "", 10, {THREE_OF_DIAMONDS, TEN_OF_DIAMONDS, ACE_OF_DIAMONDS}, {FIVE_OF_SPADES, SEVEN_OF_SPADES ,JACK_OF_CLUBS}};
    State->Game.Opponents[0] = One;
    State->Game.Opponents[1] = Two;
    State->Game.Opponents[2] = Three;
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

STN_INTERNAL state_type
StateGameUpdate(game_state *State)
{
    state_type NextState = STATE_TYPE_INVALID;
    ImGuiIO& io = ImGui::GetIO();

    BeginFrame(&GlobalState->Renderer);

    // NOTE(Oskar): Draw player
    {
        f32 CardStartX = (WINDOW_WIDTH / 2.0) - (State->Game.Player.NumberOfCards / 2) * CARD_WIDTH;
        for (u32 Index = 0; Index < State->Game.Player.NumberOfCards; ++Index)
        {
            vector4 Destination = Vector4Init(CardStartX, WINDOW_HEIGHT - CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
            DrawCard(&GlobalState->Renderer, &GlobalState->Cards, State->Game.Player.Hand[Index], Destination);

            if (io.MouseClicked[0])
            {
                vector2 ClickPosition = Vector2Init(io.MouseClickedPos[0].x, io.MouseClickedPos[0].y);
                if (Vector4RectangleHasPoint(Destination, ClickPosition))
                {
                    printf("You clicked on your cards!\n");
                }
            }

            CardStartX += CARD_WIDTH;
        }

        CardStartX = (WINDOW_WIDTH / 2.0) - (State->Game.Player.NumberOfCards / 2) * CARD_WIDTH;
        for (u32 Index = 0; Index < 3; ++Index)
        {
            vector4 Destination = Vector4Init(CardStartX, WINDOW_HEIGHT - CARD_HEIGHT * 2, CARD_WIDTH, CARD_HEIGHT);
            
            if (State->Game.Player.BottomCards[Index] != CARD_TYPE_NULL)
            {
                DrawCard(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination);
            }
            if (State->Game.Player.TopCards[Index] != CARD_TYPE_NULL)
            {
                DrawCard(&GlobalState->Renderer, &GlobalState->Cards, State->Game.Player.TopCards[Index], Destination);
            }

            if (io.MouseClicked[0])
            {
                vector2 ClickPosition = Vector2Init(io.MouseClickedPos[0].x, io.MouseClickedPos[0].y);
                if (Vector4RectangleHasPoint(Destination, ClickPosition))
                {
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
                
                if (State->Game.Player.BottomCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCard(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination);
                }
                if (State->Game.Player.TopCards[Index] != CARD_TYPE_NULL)
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
                
                if (State->Game.Player.BottomCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCardAngle(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination, 1.57079633f);
                }
                if (State->Game.Player.TopCards[Index] != CARD_TYPE_NULL)
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
                
                if (State->Game.Player.BottomCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCardAngle(&GlobalState->Renderer, &GlobalState->Cards, CARD_TYPE_FLIPPED, Destination, 1.57079633f);
                }
                if (State->Game.Player.TopCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCardAngle(&GlobalState->Renderer, &GlobalState->Cards, State->Game.Opponents[OpponentIndex].TopCards[Index], Destination, 1.57079633f);
                }

                CardStartY += CARD_WIDTH;
            }
        }
    }

    EndFrame(&GlobalState->Renderer, &GlobalState->Cards);
    return NextState;
}