using _10FlipServer.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace _10FlipServer.Models
{
    public class Message
    {
        public MessageType Type { get; set; }
        public dynamic Data { get; set; }
    }
}
