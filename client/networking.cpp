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
        //printf("Failed to parse JSON!\n");
    }

    if (MJSONResult < 1 || Tokens[0].Type != MJSON_OBJECT) 
    {
        //printf("Object expected\n");
        return EM_FALSE;
    }

    // TODO(Oskar): We dont really want to do this loop.
    // for (u32 JsonIndex = 1; JsonIndex < MJSONResult; ++JsonIndex)
    // {
    u32 JsonIndex = 1;
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
                        CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                        if (StringsAreEqual(Buffer, "name"))
                        {
                            JsonIndex++; // name

                            CopyCStringToFixedSizeBuffer(Game->Name, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
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
            } break;

            case MESSAGE_TYPE_CREATE_GAME:
            {
                JsonIndex++;
                JsonIndex++; // Data
                JsonIndex++; // {
                
                CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
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
            } break;

            case MESSAGE_TYPE_CONECT_TO_GAME:
            {
                JsonIndex++; // Data
                JsonIndex++; // {

                CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                Message.PlayerCount = GetFirstI32FromCString(Buffer);

            } break;

            case MESSAGE_TYPE_START_GAME:
            {
            } break;

            case MESSAGE_TYPE_LEAVE_GAME:
            {
            } break;

            case MESSAGE_TYPE_GAME_INIT:
            case MESSAGE_TYPE_GAME_UPDATE:
            {
                JsonIndex++; // Data
                JsonIndex++; // {
                JsonIndex++;
                
                printf("START OF REQUEST: %s\n", DataString + Tokens[JsonIndex].Start);

                CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                if (StringsAreEqual(Buffer, "opponentCount"))
                {
                    JsonIndex++; // opponentCount

                    CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                    i32 Count = GetFirstI32FromCString(Buffer);

                    Message.GameInit.OpponentCount = (u32)Count;
                    
                    JsonIndex++; 
                }

                JsonIndex++; // opponents

                u32 TotalOpponentsArraySize = Tokens[JsonIndex].Size;

                JsonIndex++; // [
                // JsonIndex++; // {
                for (u32 OpponentIndex = 0; OpponentIndex < TotalOpponentsArraySize; ++OpponentIndex)
                {
                    if (OpponentIndex == 0)
                    {
                        JsonIndex++; // {
                    }
                    
                    CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                    if (StringsAreEqual(Buffer, "bottomCardCount"))
                    {
                        JsonIndex++; // handCount

                        CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                        i32 BottomCardCount = GetFirstI32FromCString(Buffer);
                        Message.GameInit.Opponents[OpponentIndex].NumberOfBottomCards = BottomCardCount;

                        JsonIndex++;
                    }

                    CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                    if (StringsAreEqual(Buffer, "handCount"))
                    {
                        JsonIndex++; // handCount

                        CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                        i32 HandCount = GetFirstI32FromCString(Buffer);
                        Message.GameInit.Opponents[OpponentIndex].NumberOfCards = HandCount;

                        JsonIndex++;
                    }


                    CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                    if (StringsAreEqual(Buffer, "topCards"))
                    {
                        JsonIndex++; // topCards

                        u32 TotalTopCardsArraySize = Tokens[JsonIndex].Size;
                        JsonIndex++; // [
                        JsonIndex++; // {
                        for (u32 ArrayIndex = 0; ArrayIndex < TotalTopCardsArraySize; ++ArrayIndex)
                        {
                            CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                            if (StringsAreEqual(Buffer, "Type"))
                            {
                                JsonIndex++; // Type

                                CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                                i32 CardType = GetFirstI32FromCString(Buffer);
                                Message.GameInit.Opponents[OpponentIndex].TopCards[ArrayIndex] = (card_type)CardType;

                                JsonIndex++; // Number
                                JsonIndex++; // Value
                                JsonIndex++; // Number
                                JsonIndex++; // Suit
                                JsonIndex++; // Number
                            }
                            JsonIndex++;
                        }
                    }
                    
                    // JsonIndex++; // }
                }

                u32 TotalTopCardsArraySize = Tokens[JsonIndex].Size;
                JsonIndex++; // [
                JsonIndex++; // {
                for (u32 TopCard = 0; TopCard < TotalTopCardsArraySize; ++TopCard)
                {
                    CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                    if (StringsAreEqual(Buffer, "Type"))
                    {
                        JsonIndex++; // Type

                        CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                        i32 CardType = GetFirstI32FromCString(Buffer);
                        Message.GameInit.TopCards[TopCard] = (card_type)CardType;

                        JsonIndex++; // Number
                        JsonIndex++; // Value
                        JsonIndex++; // Number
                        JsonIndex++; // Suit
                        JsonIndex++; // Number
                    }
                    JsonIndex++;
                }
                
                u32 TotalHandCards = Tokens[JsonIndex].Size;
                Message.GameInit.NumberOfCards = TotalHandCards;
                
                JsonIndex++;
                JsonIndex++;
                for (u32 HandIndex = 0; HandIndex < TotalHandCards; ++HandIndex)
                {
                    JsonIndex++; // Type

                    CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                    i32 CardType = GetFirstI32FromCString(Buffer);
                    Message.GameInit.Hand[HandIndex] = (card_type)CardType;
                    
                    JsonIndex++; // HandValue
                    JsonIndex++; // Value
                    JsonIndex++; // Number
                    JsonIndex++; // Suit
                    JsonIndex++; // Number
                    JsonIndex++; // {
                }

                CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                i32 YourTurn = GetFirstI32FromCString(Buffer);

                if (YourTurn)
                {
                    Message.GameInit.YourTurn = true;    
                }
                else
                {
                    Message.GameInit.YourTurn = false;
                }

                JsonIndex++; // YourTurn
                JsonIndex++; // tableCard

                CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                i32 TableCardType = GetFirstI32FromCString(Buffer);
                Message.GameInit.TableCard = (card_type)TableCardType;

                printf("End OF REQUEST: %s\n", DataString + Tokens[JsonIndex].Start);

                JsonIndex++; // TableCardType
                JsonIndex++; // numberOfBottomCards

                CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                i32 NumberOfBottomCards = GetFirstI32FromCString(Buffer);
                Message.GameInit.NumberOfBottomCards = (u32)NumberOfBottomCards;

                printf("Bottom Cards: %d\n", NumberOfBottomCards);

            } break;

            case MESSAGE_TYPE_GAME_END_TURN:
            {
                JsonIndex++; // Data
                JsonIndex++; // {

                CopyCStringToFixedSizeBuffer(Buffer, (Tokens[JsonIndex].End - Tokens[JsonIndex].Start) + 1, DataString + Tokens[JsonIndex].Start);
                i32 MyTurn = GetFirstI32FromCString(Buffer);
                if (MyTurn)
                {
                    Message.MyTurn = true;    
                }
                else
                {
                    Message.MyTurn = false;
                }
            } break;

            case MESSAGE_TYPE_GAME_WIN:
            {
                printf("Got GAME_WIN\n");
            } break;

            case MESSAGE_TYPE_GAME_LOOSE:
            {
                printf("Got GAME_LOOSE\n");
            } break;
        }
    }
    // }

    GlobalState->Messages[GlobalState->MessageCount++] = Message;
    return EM_TRUE;
}