#include "opennt_host_event_state.h"
#include "softpc.new/host/inc/nt_event.h"

/* True source subset of OpenNT nt_event.c global definitions. */
ULONG CntrlHandlerState = 0;
ULONG event_thread_blocked_reason = 0xFFFFFFFF;
BOOL fEventThreadBlock = FALSE;

/* DIVERGENCE(HOST-DIV-030): the source-derived observation record is private
 * overlay code; this mirror keeps only narrow calls into that private body. */
#include "opennt-host-overlay/softpc.new/host/src/nt_event_state.c"

void opennt_host_event_reset(void)
{
    CntrlHandlerState = 0;
    event_thread_blocked_reason = 0xFFFFFFFF;
    fEventThreadBlock = FALSE;
    stdoutRedirected = FALSE;
    opennt_host_overlay_event_reset();
}

void opennt_host_event_snapshot(opennt_host_event_state *snapshot)
{
    opennt_host_overlay_event_snapshot(snapshot);
}

void opennt_host_event_set_stdout_redirected(BOOL redirected)
{
    opennt_host_overlay_event_set_stdout_redirected(redirected);
}

void opennt_host_event_note_block(ULONG reason)
{
    opennt_host_overlay_event_note_block(reason);
}

void opennt_host_event_note_resume(void)
{
    opennt_host_overlay_event_note_resume();
}

void opennt_host_event_note_exit_request(void)
{
    opennt_host_overlay_event_note_exit_request();
}

void opennt_host_event_note_console_initialized(void)
{
    opennt_host_overlay_event_note_console_initialized();
}

/* DIVERGENCE(HOST-DIV-030): the original thread bootstrap also binds VDD,
 * display, BIOS-keyboard and console-window product facilities. The admitted
 * single-session subset records only that source initialization was requested. */
void nt_init_event_thread(void)
{
    opennt_host_event_note_console_initialized();
}

/* True source subset of OpenNT nt_event.c:1364. */
void nt_block_event_thread(ULONG BlockFlags)
{
    event_thread_blocked_reason = BlockFlags;
    /* DIVERGENCE(HOST-DIV-030): nt_init_event_thread, VDD hooks, display,
     * BIOS keyboard, sound, timer and device close paths require separate
     * product owners. Preserve the original reached state transitions. */
    fEventThreadBlock = TRUE;
    CntrlHandlerState |= CNTRL_VDMBLOCKED;
    opennt_host_event_note_block(BlockFlags);
}

/* True source subset of OpenNT nt_event.c:1501. */
void nt_resume_event_thread(void)
{
    /* DIVERGENCE(HOST-DIV-030): the original resumes historical console,
     * event, device and display services. Those owners are not admitted;
     * retain the state order needed by reached COMMAND callers. */
    CntrlHandlerState &= ~CNTRL_VDMBLOCKED;
    fEventThreadBlock = FALSE;
    opennt_host_event_note_resume();
}

/* True source subset of OpenNT nt_event.c:1879. */
void cmdPushExitInConsoleBuffer(void)
{
    if ((CntrlHandlerState & CNTRL_PUSHEXIT) != 0u) return;
    CntrlHandlerState |= CNTRL_PUSHEXIT;
    /* DIVERGENCE(HOST-DIV-030): host_applClose, ExitVDM, window messaging
     * and ExitThread belong to deferred console/VDD process-group lifecycle.
     * Record the original request without claiming it occurred. */
    opennt_host_event_note_exit_request();
}
