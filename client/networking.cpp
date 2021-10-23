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
    puts("onmessage");
    if (WebsocketEvent->isText) {
        // For only ascii chars.
        printf("message: %s\n", WebsocketEvent->data);
    }

    EMSCRIPTEN_RESULT result;
    result = emscripten_websocket_close(WebsocketEvent->socket, 1000, "no reason");
    if (result) {
        printf("Failed to emscripten_websocket_close(): %d\n", result);
    }
    return EM_TRUE;
}