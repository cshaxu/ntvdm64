/*
 * Source-shaped unavailable boundary for the original NT4 VDM debugger event
 * product.  The ordinary DOS profile neither registers nor services a private
 * debugger transport; an actual debug event must therefore terminate only the
 * bound VDM session, never silently succeed and never terminate the host app.
 */

#include <windows.h>

#include "mvdm_softpc_termination.h"
#include "session/session.h"

static void mvdm_debugger_product_unavailable(const char *origin)
{
    session *owner = session_thread_current();

    if (owner != NULL)
        session_record_mechanical_resume_status(owner,
            SESSION_MECHANICAL_STATUS_BACKEND_UNAVAILABLE);
    mvdm_softpc_set_termination_origin(origin);
    (void)mvdm_softpc_terminate_current_session(0u,
        (uint32_t)ERROR_CALL_NOT_IMPLEMENTED);
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}

/* DIVERGENCE(ADAPTER-MVDM-DEBUGGER-002): original `dbg.c` dispatches each of
 * these forms over private NT4 VDM debugger/CSR transport.  That product is
 * not present in the declared ordinary non-debug profile.  Preserve every
 * reached external name and parameter shape; a real debugger operation is a
 * typed session stop, not a no-op event, synthetic debugger reply or process
 * termination. */
void ModuleLoad(char *module_name, char *path_name, unsigned short segment,
    unsigned long length)
{
    (void)module_name;
    (void)path_name;
    (void)segment;
    (void)length;
    mvdm_debugger_product_unavailable("debugger:ModuleLoad");
}

void ModuleSegmentMove(char *module_name, char *path_name,
    unsigned short old_selector, unsigned short new_selector)
{
    (void)module_name;
    (void)path_name;
    (void)old_selector;
    (void)new_selector;
    mvdm_debugger_product_unavailable("debugger:ModuleSegmentMove");
}

void ModuleFree(char *module_name, char *path_name)
{
    (void)module_name;
    (void)path_name;
    mvdm_debugger_product_unavailable("debugger:ModuleFree");
}

void DBGDispatch(void)
{
    mvdm_debugger_product_unavailable("debugger:DBGDispatch");
}

unsigned long DbgPrompt(char *prompt, char *response,
    unsigned long maximum_response_length)
{
    (void)prompt;
    (void)response;
    (void)maximum_response_length;
    mvdm_debugger_product_unavailable("debugger:DbgPrompt");
    return 0u;
}
