using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Net;
using System.Net.WebSockets;
using System.Text;
using System.Threading.Tasks;
using _10FlipServer.Models;
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
                        List<dynamic> returnGames = new List<dynamic>();
                        foreach (var game in games)
                        {
                            dynamic o = new ExpandoObject();
                            o.name = game.Value.Name;
                            o.id = game.Key;
                            returnGames.Add(o);
                        }
                        string response = JsonConvert.SerializeObject(returnGames);
                        await SendToWebSocket(response, webSocket, result);
                    }
                    else if (msg.Contains("create:"))
                    {
                        string[] parts = msg.Split("create:");
                        if (parts.Length == 2)
                        {
                            string name = parts[1];
                            string response = await CreateNewGame(name, webSocket);
                            await SendToWebSocket(response, webSocket, result);
                        } else
                        {
                            await SendToWebSocket("Failed", webSocket, result);
                        }
                    }
                    else if (msg.Contains("connect:"))
                    {
                        bool isConnected = games.Any(g => g.Value.Users.Any(u => u.Socket == webSocket));
                        if (!isConnected)
                        {
                            string[] parts = msg.Split("connect:");
                            if (parts.Length == 2)
                            {
                                string token = parts[1];
                                string name = await ConnectToGame(token, webSocket);
                                if (name != null)
                                {
                                    var game = games.GetValueOrDefault(token);
                                    dynamic o = new ExpandoObject();
                                    o.message = name + " connected.";
                                    o.users = game.Users;
                                    string response = JsonConvert.SerializeObject(o);
                                    foreach (User user in game.Users)
                                    {
                                        SendToWebSocket(response, user.Socket, result);
                                    }
                                } else
                                {
                                    await SendToWebSocket("Failed", webSocket, result);
                                }
                            }
                        }
                    }
                    else if (msg.Contains("start:"))
                    {
                        string[] parts = msg.Split("start:");
                        if (parts.Length == 2)
                        {
                            string[] tokens = parts[1].Split(",");
                            if (tokens.Length == 2)
                            {
                                await StartGame(tokens[0], tokens[1], result);
                            }
                        }
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

        private async Task<string> CreateNewGame(string name, WebSocket socket)
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

                return "{ \"id\":\"" + gameToken + "\", \"adminToken\":\"" + adminToken + "\" }";
            }
            return null;
        }

        private async Task StartGame(string gameToken, string adminToken, WebSocketReceiveResult result)
        {
            Game game = games.GetValueOrDefault(gameToken);
            if (!game.Started && game.AdminToken == adminToken)
            {
                game.Started = true;
                string response = "Game started";
                foreach (User user in game.Users)
                {
                    SendToWebSocket(response, user.Socket, result);
                }
            }
        }
    }
}
