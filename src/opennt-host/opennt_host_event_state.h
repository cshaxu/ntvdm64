#ifndef OPENNT_HOST_EVENT_STATE_H
#define OPENNT_HOST_EVENT_STATE_H

#include <stdint.h>
#include <windows.h>

/* Host-private observation for the reached non-graphical portion of
 * softpc.new/host/src/nt_msscs.c and nt_event.c.  This record is not a BOP,
 * guest, VDM, machine or native-handle ABI. */
typedef struct opennt_host_event_state {
    uint32_t stdout_redirected;
    uint32_t std_handle_notification_count;
    uint32_t event_blocked;
    uint32_t event_block_reason;
    uint32_t console_exit_requested;
    uint32_t console_initialized;
} opennt_host_event_state;

void opennt_host_event_reset(void);
void opennt_host_event_snapshot(opennt_host_event_state *state);
void opennt_host_event_set_stdout_redirected(BOOL redirected);
void opennt_host_event_note_block(ULONG reason);
void opennt_host_event_note_resume(void);
void opennt_host_event_note_exit_request(void);
void opennt_host_event_note_console_initialized(void);

#endif
