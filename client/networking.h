enum message_type
{
    MESSAGE_TYPE_GET_GAMES = 0,
    MESSAGE_TYPE_CREATE_GAME = 1,
    MESSAGE_TYPE_CONECT_TO_GAME = 2,
    MESSAGE_TYPE_START_GAME = 3,
    MESSAGE_TYPE_LEAVE_GAME = 4,
};

struct game_message
{
    char *Id;
    char *Name;
};

struct message
{
    union
    {
        struct
        {
            game_message Games[25];
        };

        struct
        {
            u32 TEMP;
        };
    }
};