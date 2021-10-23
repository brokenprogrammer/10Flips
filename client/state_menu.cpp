struct menu_game
{
    char Id[65];
    char Name[65];
};

struct menu_state
{
    bool IsCreatingGame;

    menu_game Games[25];
    u32 NumberOfGames;
};

STN_INTERNAL void
StateMenuInit(menu_state *State)
{
    State->IsCreatingGame = false;
    State->NumberOfGames = 0;
}

STN_INTERNAL void
StateMenuCleanup(menu_state *State)
{
}

STN_INTERNAL void
ProcessMenuEvents(menu_state *State)
{
    for (u32 Index = 0; Index < GlobalState->MessageCount; ++Index)
    {
        message Message = GlobalState->Messages[Index];

        switch (Message.Type)
        {
            case MESSAGE_TYPE_GET_GAMES:
            {
                for (u32 GameIndex = 0; GameIndex < Message.NumberOfGames; ++GameIndex)
                {
                    menu_game *Game = &State->Games[State->NumberOfGames++];
                    CopyCStringToFixedSizeBuffer(Game->Id, StringLength(Message.Games[GameIndex].Id) + 1, Message.Games[GameIndex].Id);
                    CopyCStringToFixedSizeBuffer(Game->Name, StringLength(Message.Games[GameIndex].Name) + 1, Message.Games[GameIndex].Name);
                }
            } break;

            default:
            {
                // NOTE(Oskar): Ignore for now..
            }
        }
    }

    GlobalState->MessageCount = 0;
}

STN_INTERNAL state_type
StateMenuUpdate(menu_state *State)
{
    state_type NextState = STATE_TYPE_INVALID;

    ProcessMenuEvents(State);

    bool Open = true;

    ImGuiWindowFlags WindowFlags = 0;
    WindowFlags |= ImGuiWindowFlags_NoTitleBar;
    WindowFlags |= ImGuiWindowFlags_NoCollapse;

    ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(Center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Another Window", &Open, WindowFlags); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Select a game to join or create a new game!");
    
    const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
    static int item_current_idx = 0; // Here we store our selection data as an index.
    if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 25 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for (int n = 0; n < State->NumberOfGames; n++)
        {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(State->Games[n].Name, is_selected))
                item_current_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    ImGui::BeginGroup();
    if (ImGui::Button("Join"))
    {
        char Buffer[80];
        sprintf(Buffer, "connect:%s", State->Games[item_current_idx].Id);
        emscripten_websocket_send_utf8_text(GlobalState->WebSocket, Buffer);
        NextState = STATE_TYPE_LOBBY;
    }
    ImGui::SameLine();
    if (ImGui::Button("Create"))
    {
        State->IsCreatingGame = true;
    }
    ImGui::EndGroup();
    ImGui::End();

    if (State->IsCreatingGame)
    {
        ImGuiWindowFlags WindowFlags = 0;
        WindowFlags |= ImGuiWindowFlags_NoCollapse;
        
        ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(Center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiCond_FirstUseEver);
        ImGui::Begin("Create Game", &State->IsCreatingGame, WindowFlags);
        static char buf1[64] = ""; 
        ImGui::InputText("Name", buf1, 64);
        if (ImGui::Button("Create"))
        {
            char Buffer[80];
            sprintf(Buffer, "create:%s", buf1);
            emscripten_websocket_send_utf8_text(GlobalState->WebSocket, Buffer);

            NextState = STATE_TYPE_LOBBY;
        }
        ImGui::End();
    }

    return NextState;
}