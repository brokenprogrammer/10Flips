struct menu_state
{
    bool IsCreatingGame;
};

STN_INTERNAL void
StateMenuInit(menu_state *State)
{
    State->IsCreatingGame = false;
}

STN_INTERNAL void
StateMenuCleanup(menu_state *State)
{
}

STN_INTERNAL state_type
StateMenuUpdate(menu_state *State)
{
    state_type NextState = STATE_TYPE_INVALID;

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
            // TODO(Oskar): Create game
            // State->IsInMenu = false;
            // State->IsCreatingGame = false;
            // State->HasCreatedGame = true;
            // State->IsInLobby = true;
            NextState = STATE_TYPE_LOBBY;
        }
        ImGui::End();
    }

    return NextState;
}