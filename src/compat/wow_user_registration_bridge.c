#include <windows.h>
#include <winternl.h>
#include "wow_registration_provider.h"
#include "compat/include/wow_user_registration_state.h"
#include "compat/include/wow_task_start_notification.h"
#include "compat/include/wow_startup_report.h"
#include "compat/include/wow_user_runtime.h"
#include "compat/include/wow_page_domain.h"


/* Immutable image-lifetime functions, bound before original DLL attach. */
static const wow_registration_provider *provider;
const wow_registration_provider *WINAPI wow_registration_provider_current(void)
{
    return provider;
}

BOOL WINAPI wow_registration_bind(const wow_registration_provider *value)
{
    PVOID previous;
    if (!value || !value->free_dde_data || !value->class_words || !value->exchange ||
            !value->callbacks || !value->set_callbacks || !value->edit_callbacks_ready ||
            !value->capture_lifecycle || !value->window_callback) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&provider,
        (PVOID)value, NULL);
    if (previous && previous != value) {
        SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE;
    }
    return TRUE;
}

/* ADAPTER-WOW-056: UserRegisterWowHandlers is one original all-or-nothing
 * ABI transaction.  Modern USER32 exposes its historical name but rejects
 * registration, so the provider owns this narrow transport seam.  All 21
 * WOW-to-USER callbacks are retained verbatim for original owners introduced
 * by later packets.  Every 20 USER-to-WOW output slots is non-null: a slot
 * with a selected source-backed carrier is wired immediately; an otherwise
 * unimplemented slot fails explicitly and is labelled below.  No placeholder
 * reports successful work or invents USER policy. */
/* The registration seam is normally silent.  An opt-in host-only witness is
 * retained because the immutable USER16 workload has no textual way to show
 * whether it reached the all-or-nothing registration call. */
static DWORD WINAPI report_start(ULONG request)
{
    return NtvdmReportWowStartup(request, 0, FALSE);
}

static DWORD notify_task_started(DWORD task_id, DWORD shared_id, const char *trace)
{
    DWORD error = wow_user_task_lifecycle_notify_start(wow_user_registered_lifecycle(),
        task_id, shared_id, report_start);
    wow_user_registration_trace(trace, error);
    SetLastError(error);
    /* Both identities already belong to this committed shared WOW task. */
    if (!error && shared_id && shared_id != (DWORD)-1)
        (void)NtvdmReportWowStartup(shared_id, (USHORT)task_id, TRUE);
    return error;
}

BOOL WINAPI registered_init_task(UINT version, DWORD compat_flags, DWORD user_flags,
    LPCSTR module_name, LPCSTR base_name,
    DWORD task_id, DWORD hotkey, DWORD shared_id, DWORD x, DWORD y,
    DWORD width, DWORD height)
{
    BOOL result;
    DWORD init_error;
    task_id = LOWORD(task_id); /* USER lifecycle stores the Win16 handle. */
    if (!wow_user_ensure_registration_lifecycle()) return FALSE;
    if (wow_user_runtime_current()->thread) {
        /* wkman.c retries pfnInitTask after every failed post-InitTask
         * handoff.  Resume the one incomplete copied-view transaction before
         * its original launch notification retry; do not run xxxInitTask or
         * create another task carrier. */
        if (!wow_user_task_lifecycle_publish_initial_view(wow_user_registered_lifecycle())) {
            DWORD error = GetLastError();
            wow_user_registration_trace("InitTaskInitialPublicationRetry", error);
            SetLastError(error);
            return FALSE;
        }
        return notify_task_started(task_id, shared_id, "InitTaskNotificationRetry") == ERROR_SUCCESS;
    }
    wow_user_registration_trace("InitTaskId", task_id);
    wow_user_registration_trace("InitTaskSharedId", shared_id);
    result = wow_user_ensure_registration_lifecycle() && wow_user_task_lifecycle_init(wow_user_registered_lifecycle(),
        version, base_name ? base_name : module_name, task_id, hotkey, shared_id, x, y, width, height,
        SW_SHOW);
    init_error=GetLastError();
    if (result && !wow_user_task_lifecycle_publish_initial_view(wow_user_registered_lifecycle())) {
        result = FALSE;
        init_error = GetLastError();
        wow_user_registration_trace("InitTaskInitialPublication", init_error);
    }
    wow_user_registration_trace("InitTask", result);
    if(!result)wow_user_registration_trace("InitTaskError",init_error);
    SetLastError(init_error);
    if (result && ((shared_id && shared_id!=(DWORD)-1) ||
        (shared_id==0 && wow_current_separate_launch()))) {
        DWORD error = notify_task_started(task_id, shared_id, "InitTaskNotification");
        if (error) {
            /* Original W32Thread owns bounded retry and W32DestroyTask.
             * A retry resumes notification, not the committed InitTask. */
            SetLastError(error);
            return FALSE;
        }
    }
    return result;
}

DWORD WINAPI UserRegisterWowHandlers(struct tagAPFNWOWHANDLERSIN *input,
    struct tagAPFNWOWHANDLERSOUT *output)
{
    DWORD shared;
    if (!input || !output) {
        SetLastError(ERROR_INVALID_PARAMETER);
        wow_user_registration_trace("UserRegisterWowHandlers", 0);
        return 0;
    }

    if (!provider) {
        SetLastError(ERROR_INVALID_STATE);
        return 0;
    }
    shared = (DWORD)mvdm_softpc_wow_page_domain_guest_shared_info();
    if (!shared || !mvdm_softpc_wow_page_domain_client_desktop_ready()) {
        SetLastError(ERROR_INVALID_STATE);
        wow_user_registration_trace("UserRegisterWowHandlers", 0);
        return 0;
    }

    provider->exchange(input, output);
    wow_user_registration_trace("UserRegisterWowHandlers", shared);
    return shared;
}
