struct player
{
    
};

struct opponent
{
    u32 NumberOfCards; // NOTE(Oskar): Init to 3
    card_type TopCards[3];
};

struct game
{
    opponent Opponents[3];
    u32 OpponentCount;

    card_type TopCard; // NOTE(Oskar): Init to null

    // NOTE(Oskar): Player
    b32 YourTurn;
    card_type Hand[52];
    u32 NumberOfCards; // NOTE(Oskar): Init to 3
    card_type TopCards[3];

    card_type TableCard;
};