#include "opennt_host_event_state.h"
#include "softpc.new/host/inc/nt_event.h"

BOOL stdoutRedirected;

/* True source subset of OpenNT nt_msscs.c:1067. */
void nt_std_handle_notification(BOOL fIsRedirection)
{
    /* DIVERGENCE(HOST-DIV-029): the original store feeds optional X86GFX
     * mouse-buffer switching. Keep the source-visible global and record the
     * reached non-graphical state; display ownership remains deferred. */
    stdoutRedirected = fIsRedirection;
    opennt_host_event_set_stdout_redirected(fIsRedirection);
}
