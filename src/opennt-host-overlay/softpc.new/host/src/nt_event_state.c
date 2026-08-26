#include "opennt_host_event_state.h"

static opennt_host_event_state state;

void opennt_host_overlay_event_reset(void)
{
    ZeroMemory(&state, sizeof(state));
}

void opennt_host_overlay_event_snapshot(opennt_host_event_state *snapshot)
{
    if (snapshot != NULL) *snapshot = state;
}

void opennt_host_overlay_event_set_stdout_redirected(BOOL redirected)
{
    state.stdout_redirected = redirected ? 1u : 0u;
    ++state.std_handle_notification_count;
}

void opennt_host_overlay_event_note_block(ULONG reason)
{
    state.event_blocked = 1u;
    state.event_block_reason = reason;
}

void opennt_host_overlay_event_note_resume(void)
{
    state.event_blocked = 0u;
}

void opennt_host_overlay_event_note_exit_request(void)
{
    state.console_exit_requested = 1u;
}
