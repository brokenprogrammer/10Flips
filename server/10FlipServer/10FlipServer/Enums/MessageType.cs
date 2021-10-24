using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace _10FlipServer.Enums
{
    public enum MessageType
    {
        GET_GAMES = 0,
        CREATE_GAME = 1,
        CONECT_TO_GAME = 2,
        START_GAME = 3,
        LEAVE_GAME = 4,
        GAME_INIT = 5,
        GAME_UPDATE = 6,
        GAME_END_TURN = 7,
        GAME_WIN = 8,
        GAME_LOOSE = 9,
    }
}
