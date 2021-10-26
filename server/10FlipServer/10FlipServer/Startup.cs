using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Net;
using System.Net.WebSockets;
using System.Text;
using System.Threading.Tasks;
using _10FlipServer.Enums;
using _10FlipServer.Models;
using _10FlipServer.Services;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.HttpsPolicy;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Newtonsoft.Json;

namespace _10FlipServer
{
    public class Startup
    {

        private Dictionary<string, Game> games = new Dictionary<string, Game>();
        private GameplayService gamePlayService = new GameplayService();

        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        public void ConfigureServices(IServiceCollection services)
        {
            services.AddMvc().SetCompatibilityVersion(CompatibilityVersion.Latest).AddMvcOptions(options =>
            {
                options.EnableEndpointRouting = false;
            });
        }

        public void Configure(IApplicationBuilder app, IHostingEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }
            else
            {
                app.UseHsts();
            }

            app.UseHttpsRedirection();
            app.UseMvc();

            var wsOptions = new WebSocketOptions { KeepAliveInterval = TimeSpan.FromSeconds(120) };
            app.UseWebSockets(wsOptions);
            app.Use(async (context, next) =>
            {
                if (context.WebSockets.IsWebSocketRequest)
                {
                    using (WebSocket webSocket = await context.WebSockets.AcceptWebSocketAsync())
                    {
                        if (context.Request.Path == "/game")
                        {
                            await GetMessage(context, webSocket);
                        }   
                    }
                }
                else
                {
                    context.Response.StatusCode = (int)HttpStatusCode.BadRequest;
                }
            });
        }

        private async Task GetMessage(HttpContext context, WebSocket webSocket)
        {
            var buffer = new byte[1024 * 4];
            WebSocketReceiveResult result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), System.Threading.CancellationToken.None);
            if (result != null)
            {
                // Clear empty games
                var emptyGames = games.Where(g => g.Value.Users.All(u => u.Socket.State != WebSocketState.Open)).Select(k => k.Key).ToList();
                foreach (var k in emptyGames)
                {
                    games.Remove(k);
                }

                while (!result.CloseStatus.HasValue)
                {
                    string msg = Encoding.UTF8.GetString(new ArraySegment<byte>(buffer, 0, result.Count));

                    Console.WriteLine($"client says: {msg}");

                    PlayerAction action = PlayerAction.ParsePlayerAction(msg);

                    switch (action.Type)
                    {
                        case PlayerActionType.Lobby:
                            {
                                var message = GetGamesMessage();
                                await SendToWebSocket(JsonConvert.SerializeObject(message), webSocket, result);
                                break; 
                            } 
                        case PlayerActionType.Create:
                            {
                                var message  = await CreateNewGame(action.Arguments[0], webSocket);
                                await SendToWebSocket(JsonConvert.SerializeObject(message), webSocket, result);
                                break;   
                            }
                        case PlayerActionType.Connect:
                            {
                                bool isConnected = games.Any(g => g.Value.Users.Any(u => u.Socket == webSocket));
                                if (!isConnected)
                                {
                                    var message = new Message();
                                    message.Type = MessageType.CONECT_TO_GAME;
                                    
                                    if (action.Arguments.Length == 1)
                                    {
                                        string token = action.Arguments[0];
                                        string name = await ConnectToGame(token, webSocket);
                                        var game = games.GetValueOrDefault(token);
                                        message.Data = game.UserCount;
                                        foreach (User user in game.Users)
                                        {
                                            await SendToWebSocket(JsonConvert.SerializeObject(message), user.Socket, result);
                                        }
                                    }
                                    else
                                    {
                                        message = GetGamesMessage();
                                        await SendToWebSocket(JsonConvert.SerializeObject(message), webSocket, result);
                                    }
                                }
                                break;
                            }
                        case PlayerActionType.Start:
                            {
                                var message = new Message();
                                message.Type = MessageType.START_GAME;
                                if (action.Arguments.Length == 2)
                                {
                                    await StartGame(action.Arguments[0], action.Arguments[1], result);
                                }
                                message.Data = "";
                                await SendToWebSocket(JsonConvert.SerializeObject(message), webSocket, result);
                                break;
                            }
                        case PlayerActionType.Place:
                            {
                                if (action.Arguments.Length == 1)
                                {
                                    CardType card = (CardType)Int32.Parse(action.Arguments[0]);
                                    await PlaceCard(card, webSocket, result);
                                }
                                break;
                            }
                        case PlayerActionType.EndTurn:
                            {
                                Game game = games.Where(g => g.Value.Users.Any(u => u.Socket == webSocket)).FirstOrDefault().Value;
                                User endingUser = game.Users.Where(u => u.Socket == webSocket).FirstOrDefault();
                                int index = game.Users.IndexOf(endingUser);
                                if (index == game.CurrentUser)
                                {
                                    await EndTurn(webSocket, result);
                                }
                                break;
                            }
                        case PlayerActionType.PickUp:
                            {
                                Game game = games.Where(g => g.Value.Users.Any(u => u.Socket == webSocket)).FirstOrDefault().Value;
                                User placingUser = game.Users.Where(u => u.Socket == webSocket).FirstOrDefault();
                                int index = game.Users.IndexOf(placingUser);
                                if (index == game.CurrentUser)
                                {
                                    while (game.PlacedCards.Count > 0)
                                    {
                                        var card = game.PlacedCards.Pop();
                                        placingUser.Hand.Add(card);
                                    }
                                    await UpdateGame(game, result);
                                }
                                await EndTurn(webSocket, result);
                                break;
                            }
                        default:
                            break;
                    }

                    result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), System.Threading.CancellationToken.None);
                }
            }
            await webSocket.CloseAsync(result.CloseStatus.Value, result.CloseStatusDescription, System.Threading.CancellationToken.None);
        }

        private async Task SendToWebSocket(string msg, WebSocket webSocket, WebSocketReceiveResult result)
        {
            await webSocket.SendAsync(new ArraySegment<byte>(Encoding.UTF8.GetBytes(msg)), result.MessageType, result.EndOfMessage, System.Threading.CancellationToken.None);
        }

        private async Task EndTurn(WebSocket webSocket, WebSocketReceiveResult result)
        {
            Message message = new Message();
            message.Type = MessageType.GAME_END_TURN;
            Game game = games.Where(g => g.Value.Users.Any(u => u.Socket == webSocket)).FirstOrDefault().Value;
            game.CurrentUser = game.CurrentUser + 1 >= game.UserCount ? 0 : game.CurrentUser + 1;
            game.canPlaceMore = true;
            for (int i = 0; i < game.Users.Count; i++)
            {
                int myTurn = game.CurrentUser == i ? 1 : 0;
                message.Data = myTurn;
                await SendToWebSocket(JsonConvert.SerializeObject(message), game.Users.ElementAt(i).Socket, result);
            }
        }

        private async Task PlaceCard(CardType card, WebSocket webSocket, WebSocketReceiveResult result)
        {
            var message = new Message();
            message.Type = MessageType.GAME_UPDATE;

            Game game = games.Where(g => g.Value.Users.Any(u => u.Socket == webSocket)).FirstOrDefault().Value;
            User placingUser = game.Users.Where(u => u.Socket == webSocket).FirstOrDefault();
            if (game != null)
            {
                gamePlayService.HandlePlaceRequest(game, placingUser, card);
                await UpdateGame(game, result);
                if (placingUser.Hand.Count() == 0 && 
                    placingUser.TopCards.Count(c => c != null) == 0 && 
                    placingUser.BottomCards.Count(c => c != null) == 0)
                {
                    var winMessage = new Message();
                    winMessage.Type = MessageType.GAME_WIN;
                    winMessage.Data = 0;
                    await SendToWebSocket(JsonConvert.SerializeObject(winMessage), placingUser.Socket, result);

                    var otherUsers = game.Users.Where(u => u != placingUser).ToList();
                    foreach (var u in otherUsers)
                    {
                        var loseMessage = new Message();
                        loseMessage.Type = MessageType.GAME_LOOSE;
                        loseMessage.Data = 0;
                        await SendToWebSocket(JsonConvert.SerializeObject(loseMessage), u.Socket, result);
                    }
                }
            }
        }

        private async Task UpdateGame(Game game, WebSocketReceiveResult result)
        {
            Message message = new Message();
            message.Type = MessageType.GAME_UPDATE;
            foreach (User user in game.Users)
            {
                dynamic o = new ExpandoObject();
                o.opponentCount = game.UserCount - 1;

                o.opponents = new List<dynamic>();
                foreach (User opp in game.Users.Where(u => u != user))
                {
                    dynamic opponent = new ExpandoObject();
                    opponent.bottomCardCount = opp.BottomCards.Count(c => c != null);
                    opponent.handCount = opp.Hand.Count;
                    opponent.topCards = opp.TopCards;
                    o.opponents.Add(opponent);
                }

                o.topCards = user.TopCards;
                o.hand = user.Hand;
                o.yourTurn = user == game.Users.ElementAt(game.CurrentUser) ? 1 : 0;
                o.tableCard = game.PlacedCards.Count > 0 ? game.PlacedCards.Peek().Type : CardType.CARD_TYPE_NULL;
                o.numberOfBottomCards = user.BottomCards.Count(c => c != null);
                message.Data = o;
                await SendToWebSocket(JsonConvert.SerializeObject(message), user.Socket, result);
            }
        }

        private async Task<string> ConnectToGame(string token, WebSocket socket)
        {
            // TODO(Jesper): Save web socket connection.

            Game game = games.GetValueOrDefault(token);
            if (game != null && game.UserCount < game.MaxUsers)
            {
                User existing = game.Users.Where(u => u.Socket == socket).FirstOrDefault();
                if (existing == null)
                {
                    string name = "Player " + (game.UserCount + 1);
                    User user = new User(name);
                    user.Socket = socket;
                    game.Users.Add(user);
                    return name;
                }
            }
            return null;
        }

        private async Task<Message> CreateNewGame(string name, WebSocket socket)
        {
            if (name == null)
            {
                return null;
            }
            if (games.Count < 25)
            {
                var message = new Message();
                message.Type = MessageType.CREATE_GAME;

                string gameToken = Guid.NewGuid().ToString();
                string adminToken = Guid.NewGuid().ToString();

                Game game = new Game(name);
                game.AdminToken = adminToken;
                games.Add(gameToken, game);

                User user = new User("Player 1");
                user.Socket = socket;
                game.Users.Add(user);
                
                dynamic o = new ExpandoObject();
                o.id = gameToken;
                o.adminToken = adminToken;
                message.Data = o;
                return message;
            }
            return null;
        }

        private async Task StartGame(string gameToken, string adminToken, WebSocketReceiveResult result)
        {
            Game game = games.GetValueOrDefault(gameToken);
            if (!game.Started && game.AdminToken == adminToken)
            {
                User starter = gamePlayService.StartGame(game);
                var message = new Message();
                message.Type = MessageType.START_GAME;

                foreach (User user in game.Users)
                {
                    await SendToWebSocket(JsonConvert.SerializeObject(message), user.Socket, result);
                }

                var gameInit = new Message();
                message.Type = MessageType.GAME_INIT;
                foreach (User user in game.Users)
                {
                    dynamic o = new ExpandoObject();
                    o.opponentCount = game.UserCount - 1;

                    o.opponents = new List<dynamic>();
                    foreach (User opp in game.Users.Where(u => u != user))
                    {
                        dynamic opponent = new ExpandoObject();
                        opponent.handCount = opp.Hand.Count;
                        opponent.topCards = opp.TopCards;
                        o.opponents.Add(opponent);
                    }

                    o.topCards = user.TopCards;
                    o.hand = user.Hand;
                    o.yourTurn = user == starter ? 1 : 0;
                    o.tableCard = game.PlacedCards.Count > 0 ? game.PlacedCards.Peek().Type : CardType.CARD_TYPE_NULL;
                    message.Data = o;
                    await SendToWebSocket(JsonConvert.SerializeObject(message), user.Socket, result);
                }
            }
        }

        private Message GetGamesMessage()
        {
            var message = new Message();
            message.Type = MessageType.GET_GAMES;

            List<dynamic> returnGames = new List<dynamic>();
            foreach (var game in games.Where(g => !g.Value.Started))
            {
                dynamic o = new ExpandoObject();
                o.name = game.Value.Name;
                o.id = game.Key;
                returnGames.Add(o);
            }

            message.Data = returnGames;
            return message;
        }
    }
}
