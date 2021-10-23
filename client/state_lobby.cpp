struct lobby_state
{
    bool HasCreatedGame;
    char AdminToken[80];
    char GameId[80];

    // TODO(Oskar): Get players info and stuff.
    u32 NumberOfPlayers;
};

STN_INTERNAL void
StateLobbyInit(lobby_state *State)
{
    State->HasCreatedGame = false;
    State->NumberOfPlayers = 1;
}

STN_INTERNAL void
StateLobbyCleanup(lobby_state *State)
{
}

STN_INTERNAL b32
ProcessLobbyEvents(lobby_state *State)
{
    for (u32 Index = 0; Index < GlobalState->MessageCount; ++Index)
    {
        message Message = GlobalState->Messages[Index];

        switch (Message.Type)
        {
            case MESSAGE_TYPE_CREATE_GAME:
            {
                State->HasCreatedGame = true;
                CopyCStringToFixedSizeBuffer(State->AdminToken, StringLength(Message.AdminToken) + 1, Message.AdminToken);
                CopyCStringToFixedSizeBuffer(State->GameId, StringLength(Message.Id) + 1, Message.Id);
            } break;

            case MESSAGE_TYPE_CONECT_TO_GAME:
            {
                if (Message.PlayerCount < 0)
                {
                    return false; 
                }
                State->NumberOfPlayers = (u32)Message.PlayerCount;
            } break;

            default:
            {
                // NOTE(Oskar): Ignore for now..
            }
        }
    }

    GlobalState->MessageCount = 0;
    return true;
}

STN_INTERNAL state_type
StateLobbyUpdate(lobby_state *State)
{
    state_type NextState = STATE_TYPE_INVALID;

    if (!ProcessLobbyEvents(State))
    {
        NextState = STATE_TYPE_MENU;
    }

    bool Open = true;

    ImGuiWindowFlags WindowFlags = 0;
    WindowFlags |= ImGuiWindowFlags_NoTitleBar;
    WindowFlags |= ImGuiWindowFlags_NoCollapse;

    ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(Center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Lobby", &Open, WindowFlags); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    
    if (State->HasCreatedGame)
    {
        ImGui::Text("Wait for players and Start when ready!");

    }
    else
    {
        ImGui::Text("Wait for the host to start!");
    }

    const char* items[] = { "Player 1", "Player 2", "Player 3", "Player 4" };
    static int item_current_idx = 0;
    if (ImGui::BeginListBox("##playerlist", ImVec2(-FLT_MIN, 4 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for (int n = 0; n < State->NumberOfPlayers; n++)
        {
            const bool is_selected = (0 == n);
            if (ImGui::Selectable(items[n], is_selected))
                item_current_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    ImGui::BeginGroup();
    if (ImGui::Button("Back"))
    {
        // TODO(Oskar): Send Leave lobby message
        NextState = STATE_TYPE_MENU;
    }
    ImGui::SameLine();
    if (State->HasCreatedGame)
    {
        if (ImGui::Button("Start Game"))
        {
            // NOTE(Oskar): Start game!
            char Buffer[80];
            sprintf(Buffer, "start:%s,%s", State->GameId, State->AdminToken);
            emscripten_websocket_send_utf8_text(GlobalState->WebSocket, Buffer);

            NextState = STATE_TYPE_GAME;
        }
    }
    ImGui::EndGroup();
    ImGui::End();

    return NextState;
}