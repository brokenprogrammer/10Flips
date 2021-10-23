STN_INTERNAL void
StateInit(state_type Type, memory_arena *StateArena)
{
    switch (Type)
    {
        case STATE_TYPE_MENU:
        {
            StateMenuInit(static_cast<menu_state *>(MemoryArenaAllocate(StateArena, sizeof(menu_state))));
        } break;

        case STATE_TYPE_LOBBY:
        {
            StateLobbyInit(static_cast<lobby_state *>(MemoryArenaAllocate(StateArena, sizeof(lobby_state))));
        } break;

        case STATE_TYPE_GAME:
        {
            StateGameInit(static_cast<game_state *>(MemoryArenaAllocate(StateArena, sizeof(game_state))));
        } break;
    }
}

STN_INTERNAL void
StateCleanUp(state_type Type, memory_arena *StateArena)
{
    switch (Type)
    {
        case STATE_TYPE_MENU:
        {
            StateMenuCleanup(static_cast<menu_state *>(StateArena->Memory));
        } break;

        case STATE_TYPE_LOBBY:
        {
            StateLobbyCleanup(static_cast<lobby_state *>(StateArena->Memory));
        } break;

        case STATE_TYPE_GAME:
        {
            StateGameCleanup(static_cast<game_state *>(StateArena->Memory));
        } break;
    }
    MemoryArenaClear(StateArena);
}

STN_INTERNAL state_type
StateUpdate(state_type Type, memory_arena *StateArena)
{
    state_type NextState = STATE_TYPE_INVALID;

    switch (Type)
    {
        case STATE_TYPE_MENU:
        {
            NextState = StateMenuUpdate(static_cast<menu_state *>(StateArena->Memory));
        } break;

        case STATE_TYPE_LOBBY:
        {
            NextState = StateLobbyUpdate(static_cast<lobby_state *>(StateArena->Memory));
        } break;

        case STATE_TYPE_GAME:
        {
            NextState = StateGameUpdate(static_cast<game_state *>(StateArena->Memory));
        } break;
    }

    return NextState;
}