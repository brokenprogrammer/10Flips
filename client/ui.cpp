STN_INTERNAL void
UpdateAndRenderMenu(game_state *State)
{
    ImGuiWindowFlags WindowFlags = 0;
    WindowFlags |= ImGuiWindowFlags_NoTitleBar;
    WindowFlags |= ImGuiWindowFlags_NoCollapse;

    ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(Center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Another Window", &State->IsInMenu, WindowFlags); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Select a game to join or create a new game!");
    
    const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
    static int item_current_idx = 0; // Here we store our selection data as an index.
    if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 25 * ImGui::GetTextLineHeightWithSpacing())))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected))
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
        // TODO(Oskar): Join game
    }
    ImGui::SameLine();
    if (ImGui::Button("Create"))
    {
        State->IsCreatingGame = true;
    }
    ImGui::EndGroup();

    ImGui::End();
}

STN_INTERNAL void
UpdateAndRenderCreateGame(game_state *State)
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
        // TODO(Oskar): Create game
        State->IsInMenu = false;
        State->IsCreatingGame = false;
        State->HasCreatedGame = true;
        State->IsInLobby = true;
    }
    ImGui::End();
}

STN_INTERNAL void
UpdateAndRenderLobby(game_state *State)
{
    ImGuiWindowFlags WindowFlags = 0;
    WindowFlags |= ImGuiWindowFlags_NoTitleBar;
    WindowFlags |= ImGuiWindowFlags_NoCollapse;

    ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(Center, ImGuiCond_Once, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Lobby", &State->IsInLobby, WindowFlags); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    
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
    }
    ImGui::SameLine();
    if (State->HasCreatedGame)
    {
        if (ImGui::Button("Start Game"))
        {
            // TODO(Oskar): Join game
        }
    }
    ImGui::EndGroup();

    ImGui::End();
}