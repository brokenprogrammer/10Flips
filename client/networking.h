enum message_type
{
    MESSAGE_TYPE_GET_GAMES = 0,
    MESSAGE_TYPE_CREATE_GAME = 1,
    MESSAGE_TYPE_CONECT_TO_GAME = 2,
    MESSAGE_TYPE_START_GAME = 3,
    MESSAGE_TYPE_LEAVE_GAME = 4,
    MESSAGE_TYPE_GAME_INIT = 5,
    MESSAGE_TYPE_GAME_UPDATE = 6,
    MESSAGE_TYPE_GAME_END_TURN = 7,
    MESSAGE_TYPE_GAME_WIN = 8,
    MESSAGE_TYPE_GAME_LOOSE = 9,
};

struct game_message
{
    char Id[64];
    char Name[64];
};

struct message
{
    message_type Type;

    union
    {
        struct // get_games
        {
            game_message Games[25];
            u32 NumberOfGames;
        };

        struct // create_game
        {
            char Id[64];
            char AdminToken[64];
        };

        struct // connect_to_game
        {
            i32 PlayerCount;
        };

        game GameInit;

        struct // end_turn
        {
            b32 MyTurn;
        };
    };
};