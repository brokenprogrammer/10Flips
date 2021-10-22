#define CARD_WIDTH 50.0f
#define CARD_HEIGHT 100.0f

static player TestPlayer = {};
static u32 OpponentCount = 3;

static opponent One = { "", 4, {FIVE_OF_CLUBS, FIVE_OF_CLUBS, FIVE_OF_CLUBS}, {FIVE_OF_CLUBS, FIVE_OF_CLUBS ,FIVE_OF_CLUBS}};
static opponent Two = { "", 4, {FIVE_OF_CLUBS, FIVE_OF_CLUBS, FIVE_OF_CLUBS}, {FIVE_OF_CLUBS, FIVE_OF_CLUBS ,FIVE_OF_CLUBS}};
static opponent Three = { "", 10, {THREE_OF_DIAMONDS, TEN_OF_DIAMONDS, ACE_OF_DIAMONDS}, {FIVE_OF_SPADES, SEVEN_OF_SPADES ,JACK_OF_CLUBS}};


static opponent Opponents[3] = { One, Two, Three};

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

// TODO(Oskar): I was tired when I wrote this mess.. Clean up sometime.
STN_INTERNAL void
UpdateAndRenderGame(game_state *State, ImGuiIO& io)
{
    TestPlayer.Hand[0] = FIVE_OF_CLUBS;
    TestPlayer.Hand[1] = FOUR_OF_HEARTS;
    TestPlayer.Hand[2] = SIX_OF_DIAMONDS;
    TestPlayer.NumberOfCards = 3;

    TestPlayer.TopCards[0] = KING_OF_CLUBS;
    TestPlayer.TopCards[1] = CARD_TYPE_NULL;
    TestPlayer.TopCards[2] = JACK_OF_DIAMONDS;

    TestPlayer.BottomCards[0] = TWO_OF_SPADES;
    TestPlayer.BottomCards[1] = NINE_OF_SPADES;
    TestPlayer.BottomCards[2] = TEN_OF_CLUBS;
    

    // NOTE(Oskar): Draw player
    {
        f32 CardStartX = (WINDOW_WIDTH / 2.0) - (TestPlayer.NumberOfCards / 2) * CARD_WIDTH;
        for (u32 Index = 0; Index < TestPlayer.NumberOfCards; ++Index)
        {
            vector4 Destination = Vector4Init(CardStartX, WINDOW_HEIGHT - CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
            DrawCard(&State->Renderer, &State->Cards, TestPlayer.Hand[Index], Destination);

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

        CardStartX = (WINDOW_WIDTH / 2.0) - (TestPlayer.NumberOfCards / 2) * CARD_WIDTH;
        for (u32 Index = 0; Index < 3; ++Index)
        {
            vector4 Destination = Vector4Init(CardStartX, WINDOW_HEIGHT - CARD_HEIGHT * 2, CARD_WIDTH, CARD_HEIGHT);
            
            if (TestPlayer.BottomCards[Index] != CARD_TYPE_NULL)
            {
                DrawCard(&State->Renderer, &State->Cards, CARD_TYPE_FLIPPED, Destination);
            }
            if (TestPlayer.TopCards[Index] != CARD_TYPE_NULL)
            {
                DrawCard(&State->Renderer, &State->Cards, TestPlayer.TopCards[Index], Destination);
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

    for (u32 OpponentIndex = 0; OpponentIndex < OpponentCount; ++OpponentIndex)
    {

        // NOTE(Oskar): Top player
        if (OpponentIndex == 0)
        {
            f32 CardStartX = (WINDOW_WIDTH / 2.0) - (Opponents[OpponentIndex].NumberOfCards / 2) * CARD_WIDTH;

            // NOTE(Oskar): Render hand
            for (u32 Index = 0; Index < Opponents[OpponentIndex].NumberOfCards; ++Index)
            {
                vector4 Destination = Vector4Init(CardStartX,  0, CARD_WIDTH, CARD_HEIGHT);
                DrawCard(&State->Renderer, &State->Cards, CARD_TYPE_FLIPPED, Destination);

                CardStartX += CARD_WIDTH;
            }

            CardStartX = (WINDOW_WIDTH / 2.0) - (3 / 2) * CARD_WIDTH;
            // NOTE(Oskar): Render top and bottom cards
            for (u32 Index = 0; Index < 3; ++Index)
            {
                vector4 Destination = Vector4Init(CardStartX, CARD_HEIGHT, CARD_WIDTH, CARD_HEIGHT);
                
                if (TestPlayer.BottomCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCard(&State->Renderer, &State->Cards, CARD_TYPE_FLIPPED, Destination);
                }
                if (TestPlayer.TopCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCard(&State->Renderer, &State->Cards, Opponents[OpponentIndex].TopCards[Index], Destination);
                }

                CardStartX += CARD_WIDTH;
            }
        }
        else if (OpponentIndex == 1) // Left Player
        {
            f32 CardStartX = 0;
            f32 CardStartY = (WINDOW_HEIGHT / 2.0f) - (Opponents[OpponentIndex].NumberOfCards / 2) * CARD_WIDTH;

            // NOTE(Oskar): Render hand
            for (u32 Index = 0; Index < Opponents[OpponentIndex].NumberOfCards; ++Index)
            {
                vector4 Destination = Vector4Init(CardStartX, CardStartY, CARD_WIDTH, CARD_HEIGHT);
                DrawCardAngle(&State->Renderer, &State->Cards, CARD_TYPE_FLIPPED, Destination, 1.57079633f);

                CardStartY += CARD_WIDTH;
            }

            CardStartX = CardStartY = (WINDOW_HEIGHT / 2.0f) - (3 / 2) * CARD_WIDTH;
            // NOTE(Oskar): Render top and bottom cards
            for (u32 Index = 0; Index < 3; ++Index)
            {
                vector4 Destination = Vector4Init(CARD_HEIGHT, CardStartY, CARD_WIDTH, CARD_HEIGHT);
                
                if (TestPlayer.BottomCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCardAngle(&State->Renderer, &State->Cards, CARD_TYPE_FLIPPED, Destination, 1.57079633f);
                }
                if (TestPlayer.TopCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCardAngle(&State->Renderer, &State->Cards, Opponents[OpponentIndex].TopCards[Index], Destination, 1.57079633f);
                }

                CardStartY += CARD_WIDTH;
            }
        }
        else if (OpponentIndex == 2) // Right player
        {
            f32 CardStartX = WINDOW_WIDTH - CARD_HEIGHT;;
            f32 CardStartY = (WINDOW_HEIGHT / 2.0f) - (Opponents[OpponentIndex].NumberOfCards / 2) * CARD_WIDTH;

            // NOTE(Oskar): Render hand
            for (u32 Index = 0; Index < Opponents[OpponentIndex].NumberOfCards; ++Index)
            {
                vector4 Destination = Vector4Init(CardStartX, CardStartY, CARD_WIDTH, CARD_HEIGHT);
                DrawCardAngle(&State->Renderer, &State->Cards, CARD_TYPE_FLIPPED, Destination, 1.57079633f);

                CardStartY += CARD_WIDTH;
            }

            CardStartY = (WINDOW_HEIGHT / 2.0f) - (3 / 2) * CARD_WIDTH;
            // NOTE(Oskar): Render top and bottom cards
            for (u32 Index = 0; Index < 3; ++Index)
            {
                vector4 Destination = Vector4Init(CardStartX - CARD_HEIGHT, CardStartY, CARD_WIDTH, CARD_HEIGHT);
                
                if (TestPlayer.BottomCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCardAngle(&State->Renderer, &State->Cards, CARD_TYPE_FLIPPED, Destination, 1.57079633f);
                }
                if (TestPlayer.TopCards[Index] != CARD_TYPE_NULL)
                {
                    DrawCardAngle(&State->Renderer, &State->Cards, Opponents[OpponentIndex].TopCards[Index], Destination, 1.57079633f);
                }

                CardStartY += CARD_WIDTH;
            }
        }
    }

    EndFrame(&State->Renderer, &State->Cards);
}