using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace _10FlipServer.Models
{
    public class PlayerAction
    {
        public static string[] TypeStrings = { "lobby", "create", "connect", "start", "place", "endturn", "pickup" };

        public PlayerActionType Type;
        public string[] Arguments;

        public PlayerAction()
        {
            Arguments = new string[2];
        }

        public static PlayerAction ParsePlayerAction(string value)
        {
            PlayerAction action = new PlayerAction();

            string[] parts = value.Split(":");

            string typeStr = parts[0];
            action.Type = (PlayerActionType) Array.IndexOf(TypeStrings, typeStr);
            if (action.Type == PlayerActionType.Error)
            {
                return null;
            }

            if (parts.Length == 2)
            {
                action.Arguments = parts[1].Split(",");
            }

            return action;
        }
    }

    public enum PlayerActionType
    {
        Error = -1,
        Lobby = 0,
        Create = 1,
        Connect = 2,
        Start = 3,
        Place = 4,
        EndTurn = 5,
        PickUp = 6
    }
}
