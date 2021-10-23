STN_INTERNAL EM_BOOL 
WebSocketOnOpen(int EventType, const EmscriptenWebSocketOpenEvent *WebsocketEvent, void *UserData) 
{
    EMSCRIPTEN_RESULT result;
    result = emscripten_websocket_send_utf8_text(WebsocketEvent->socket, "lobby");
    if (result) {
        printf("Failed to emscripten_websocket_send_utf8_text(): %d\n", result);
    }

    return EM_TRUE;
}

STN_INTERNAL EM_BOOL 
WebSocketOnError(int EventType, const EmscriptenWebSocketErrorEvent *WebsocketEvent, void *UserData) 
{
    puts("onerror");

    return EM_TRUE;
}

STN_INTERNAL EM_BOOL 
WebSocketOnClose(int EventType, const EmscriptenWebSocketCloseEvent *WebsocketEvent, void *UserData) 
{
    puts("onclose");

    return EM_TRUE;
}

STN_INTERNAL EM_BOOL 
WebSocketOnMessage(int EventType, const EmscriptenWebSocketMessageEvent *WebsocketEvent, void *UserData)
{
    char Buffer[1024];
    message Message = {};

    mjson_parser Parser;
    mjson_token Tokens[512];
    mjson_init(&Parser);

    char *DataString = reinterpret_cast<char*>(const_cast<uint8_t*>(WebsocketEvent->data));
    u32 Len = StringLength(DataString);
    int MJSONResult = mjson_parse(&Parser, DataString, Len, Tokens, 512);
    if(MJSONResult < 0)
    {
        printf("Failed to parse JSON!\n");
    }

    if (MJSONResult < 1 || Tokens[0].Type != MJSON_OBJECT) 
    {
        printf("Object expected\n");
        return EM_FALSE;
    }

    for (u32 JsonIndex = 1; JsonIndex < MJSONResult; ++JsonIndex)
    {
        CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
        if (StringsAreEqual(Buffer, "Type"))
        {
            JsonIndex++;
            CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
            i32 Type = GetFirstI32FromCString(Buffer);

            message_type MessageType = (message_type)Type;
            Message.Type = MessageType;

            switch(MessageType)
            {
                case MESSAGE_TYPE_GET_GAMES:
                {
                    JsonIndex++;
                    JsonIndex++; // "Data"
                    
                    if (Tokens[JsonIndex].Type == MJSON_ARRAY)
                    {
                        u32 TotalArraySize = Tokens[JsonIndex].Size;
                        JsonIndex++;
                        for (u32 ArrayIndex = 0; ArrayIndex < TotalArraySize; ++ArrayIndex)
                        {
                            JsonIndex++; // {
                            game_message *Game = &Message.Games[Message.NumberOfGames++];
                            printf("Inside ArrayLoop: %s\n", DataString + Tokens[JsonIndex].Start);
                            CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                            if (StringsAreEqual(Buffer, "name"))
                            {
                                JsonIndex++; // name

                                CopyCStringToFixedSizeBuffer(Game->Name, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                                printf("Inside Name: %s\n", DataString + Tokens[JsonIndex].Start);
                                printf("Copied to Game.Name: %s\n", Game->Name);

                                JsonIndex++; 
                            }
                            
                            CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                            if (StringsAreEqual(Buffer, "id"))
                            {
                                JsonIndex++; // id

                                CopyCStringToFixedSizeBuffer(Game->Id, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);

                                JsonIndex++;
                            }

                            // JsonIndex++; // }
                        }
                    }
                    printf("1: %s\n", Message.Games[0].Id);
                    printf("2: %s\n", Message.Games[0].Name);
                } break;

                case MESSAGE_TYPE_CREATE_GAME:
                {
                    printf("Create Event Start Token: %s\n", DataString + Tokens[JsonIndex].Start);
                    JsonIndex++;
                    JsonIndex++; // Data
                    JsonIndex++; // {
                    
                    printf("Create event\n");
                    printf("%s\n", DataString + Tokens[JsonIndex].Start);

                    CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                    printf("Buffer: : %s\n", Buffer);
                    if (StringsAreEqual(Buffer, "id"))
                    {
                        JsonIndex++; // id
                        CopyCStringToFixedSizeBuffer(Message.Id, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);

                        JsonIndex++;
                    }
                    
                    CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                    if (StringsAreEqual(Buffer, "adminToken"))
                    {
                        JsonIndex++; // name

                        CopyCStringToFixedSizeBuffer(Message.AdminToken, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);

                        JsonIndex++; 
                    }
                    
                    JsonIndex++; // }

                    printf("1: %s\n", Message.Id);
                    printf("2: %s\n", Message.AdminToken);
                } break;

                case MESSAGE_TYPE_CONECT_TO_GAME:
                {
                    printf("CONNECT TO GAME: %s\n", DataString + Tokens[JsonIndex].Start);
                    JsonIndex++; // Data
                    JsonIndex++; // {
                    printf("CONNECT TO GAME2: %s\n", DataString + Tokens[JsonIndex].Start);

                    CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                    Message.PlayerCount = GetFirstI32FromCString(Buffer);

                } break;

                case MESSAGE_TYPE_START_GAME:
                {

                } break;

                case MESSAGE_TYPE_LEAVE_GAME:
                {

                } break;
            }
        }
    }

    GlobalState->Messages[GlobalState->MessageCount++] = Message;
    return EM_TRUE;
}