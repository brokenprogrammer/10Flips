using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace _10FlipServer.Models
{
    public class Game
    {
        public string Name;
        public List<User> Users;
        public Deck Deck;
        public Stack<Card> PlacedCards;
        public int CurrentUser;
        public string AdminToken;
        public bool Started;
        public int MaxUsers;
        public int UserCount
        {
            get
            {
                return Users.Count;
            }
        }

        public Game(string name)
        {
            this.Name = name;
            Users = new List<User>();
            MaxUsers = 4;
            Started = false;
        }
    }
}
