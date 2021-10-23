using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.WebSockets;
using System.Threading.Tasks;

namespace _10FlipServer.Models
{
    public class User
    {
        public string Name;
        public WebSocket Socket;
        public List<Card> Hand;
        public Card[] BottomCards;
        public Card[] TopCards;

        public User(string name)
        {
            this.Name = name;
        }
    }
}
