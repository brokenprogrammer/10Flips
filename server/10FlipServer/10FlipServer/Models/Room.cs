using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace _10FlipServer.Models
{
    public class Room
    {
        public string Name;
        public List<User> Users;
        public int MaxUsers;
        public int UserCount
        {
            get
            {
                return Users.Count;
            }
        }

        public Room(string name)
        {
            this.Name = name;
            Users = new List<User>();
            MaxUsers = 4;
        }
    }
}
