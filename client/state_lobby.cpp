struct lobby_state
{
    bool HasCreatedGame;

    // TODO(Oskar): Get players info and stuff.
};

STN_INTERNAL void
StateLobbyInit(lobby_state *State)
{
    // TODO(Oskar): Get this from server.
    State->HasCreatedGame = true;
}

STN_INTERNAL void
StateLobbyCleanup(lobby_state *State)
{
}

STN_INTERNAL state_type
StateLobbyUpdate(lobby_state *State)
{
    state_type NextState = STATE_TYPE_INVALID;

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
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
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
        // TODO(Oskar): Leave lobby
        NextState = STATE_TYPE_MENU;
    }
    ImGui::SameLine();
    if (State->HasCreatedGame)
    {
        if (ImGui::Button("Start Game"))
        {
            // TODO(Oskar): Join game
            NextState = STATE_TYPE_GAME;
        }
    }
    ImGui::EndGroup();
    ImGui::End();

    return NextState;
}