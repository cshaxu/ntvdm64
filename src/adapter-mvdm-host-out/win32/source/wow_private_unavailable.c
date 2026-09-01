/*
 * Source-shaped unavailable boundary for private NT4 WOW presentation and
 * hard-error services.  Public modern Win32 has no equivalent compound
 * Console-Server/CSRSS contract.  The ordinary non-WOW profile must not
 * manufacture a cursor, dialog response, or remote hard-error completion.
 */

#include <windows.h>

#include "nt.h"
#include "mvdm_softpc_termination.h"
#include "session/session.h"

static void mvdm_wow_private_product_unavailable(void)
{
    session *owner = session_thread_current();

    if (owner != NULL)
        session_record_mechanical_resume_status(owner,
            SESSION_MECHANICAL_STATUS_BACKEND_UNAVAILABLE);
    mvdm_softpc_set_termination_origin("wow:private-hard-error");
    (void)mvdm_softpc_terminate_current_session(0u,
        (uint32_t)ERROR_CALL_NOT_IMPLEMENTED);
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}

/* DIVERGENCE(ADAPTER-WIN32-039): original USER client `ShowStartGlass`
 * invokes the private `NtUserCallOneParam(SFI__SHOWSTARTGLASS)` compound
 * server action.  That void call has no result or MVDM-visible state and is
 * presentation-only.  The declared non-GUI profile therefore preserves its
 * name, parameter and caller ordering while intentionally provides no
 * presentation.  It must not be generalized into a USER/GDI provider.
 * Hard-error handling below remains an unavailable, controlled-stop path. */
void ShowStartGlass(DWORD timeout)
{
    (void)timeout;
}

/* DIVERGENCE(ADAPTER-WIN32-040): original `nt_error.c` forwards this exact
 * NT4 hard-error ABI to CSRSS.  Public Win32 does not provide the same
 * response/broker contract.  Preserve the parameters and failure direction;
 * never invent a dialog response or terminate the hosting process. */
NTSTATUS NtRaiseHardError(NTSTATUS error_status,
    ULONG number_of_parameters, ULONG unicode_string_parameter_mask,
    PULONG_PTR parameters, ULONG valid_response_options, PULONG response)
{
    (void)error_status;
    (void)number_of_parameters;
    (void)unicode_string_parameter_mask;
    (void)parameters;
    (void)valid_response_options;
    if (response != NULL) *response = 0;
    mvdm_wow_private_product_unavailable();
    return (NTSTATUS)0xC0000002L; /* STATUS_NOT_IMPLEMENTED */
}
