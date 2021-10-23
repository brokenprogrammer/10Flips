﻿using System;
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
            services.AddMvc().SetCompatibilityVersion(CompatibilityVersion.Version_2_1);
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
                while (!result.CloseStatus.HasValue)
                {
                    string msg = Encoding.UTF8.GetString(new ArraySegment<byte>(buffer, 0, result.Count));

                    Console.WriteLine($"client says: {msg}");

                    if (msg == "lobby")
                    {
                        var message = GetGamesMessage();
                        await SendToWebSocket(JsonConvert.SerializeObject(message), webSocket, result);
                    }
                    else if (msg.Contains("create:"))
                    {
                        var message = new Message();
                        message.Type = MessageType.CREATE_GAME;

                        string[] parts = msg.Split("create:");
                        if (parts.Length == 2)
                        {
                            string name = parts[1];
                            message.Data = await CreateNewGame(name, webSocket); // "{gameToken},{adminToken}"
                        }

                        await SendToWebSocket(JsonConvert.SerializeObject(message), webSocket, result);
                    }
                    else if (msg.Contains("connect:"))
                    {
                        bool isConnected = games.Any(g => g.Value.Users.Any(u => u.Socket == webSocket));
                        if (!isConnected)
                        {
                            var message = new Message();
                            message.Type = MessageType.CONECT_TO_GAME;

                            string[] parts = msg.Split("connect:");
                            if (parts.Length == 2)
                            {
                                string token = parts[1];
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
                    }
                    else if (msg.Contains("start:"))
                    {
                        var message = new Message();
                        message.Type = MessageType.START_GAME;

                        string[] parts = msg.Split("start:");
                        if (parts.Length == 2)
                        {
                            string[] tokens = parts[1].Split(",");
                            if (tokens.Length == 2)
                            {
                                await StartGame(tokens[0], tokens[1], result);
                            }
                        }

                        message.Data = "";
                        await SendToWebSocket(JsonConvert.SerializeObject(message), webSocket, result);
                    }
                    else if (msg.Contains("place:"))
                    {
                        string[] parts = msg.Split("place:");
                        if (parts.Length == 2)
                        {
                            CardType card = (CardType) Int32.Parse(parts[1]);
                            await PlaceCard(card, webSocket, result);
                        }
                    }
                    else if (msg == "endturn")
                    {
                        await EndTurn(webSocket, result);
                    }
                    else if (msg == "pickup")
                    {
                        Game game = games.Where(g => g.Value.Users.Any(u => u.Socket == webSocket)).FirstOrDefault().Value;
                        game.CurrentUser = game.CurrentUser + 1 >= game.UserCount ? 0 : game.CurrentUser + 1;
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
                    opponent.topCards = opp.TopCards;
                    opponent.handCount = opp.Hand.Count;
                    o.opponents.Add(opponent);
                }

                o.topCards = user.TopCards;
                o.hand = user.Hand;
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

        private async Task<dynamic> CreateNewGame(string name, WebSocket socket)
        {
            if (games.Count < 25)
            {
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
                return o;
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
                        opponent.topCards = opp.TopCards;
                        o.opponents.Add(opponent);
                    }

                    o.topCards = user.TopCards;
                    o.hand = user.Hand;
                    o.yourTurn = user == starter ? 1 : 0;
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
