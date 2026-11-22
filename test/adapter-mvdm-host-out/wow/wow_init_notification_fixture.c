#include "wow_class_client_fixture_provider.h"
/* The original declaration relies on the provider's /Gz. Annotate that one
 * declaration while this host fixture uses cdecl worker/session headers. */
#define UserRegisterWowHandlers __stdcall UserRegisterWowHandlers
#include "wow_shared_user_abi.h"
#include "wow_user_task_lifecycle.h"
#undef UserRegisterWowHandlers
#include "ntvdm-exe/session/session.h"
#include <stdio.h>
#include "wow_standard_class_provider.h"
#include "compat/include/wow_button_lifetime.h"
extern PVOID WINAPI wow_original_button_data(struct tagWND *);
extern VOID WINAPI FreeLookasideEntry(PLOOKASIDE, PVOID);
#include "wow_task_profile_bindings.h"
#include "wow_cleanup_bindings.h"
extern UINT WINAPI wow_original_task_profile(UINT, LPCWSTR, LPCWSTR, LPWSTR, DWORD);
extern const wow_cleanup_provider wow_original_cleanup_provider;
extern INT WINAPI GetStdClassNumber(LPSTR);
extern LPCSTR WINAPI wow_original_class_name(UINT);
extern ATOM WINAPI wow_original_class_atom(UINT);
extern BOOL WINAPI wow_original_class_capture(LPCSTR, PUNICODE_STRING);
extern void WINAPI wow_fixture_startup_bind(void);
extern BOOL __cdecl ntvdm_dpmi_initialize(void);
static const wow_standard_class_provider class_provider = {
    GetStdClassNumber, wow_original_class_name, wow_original_class_atom, wow_original_class_capture };

/* Actual registration and lifecycle code; only guest publication and the
 * broker RPC are seams. This fixture does not claim to execute guest code. */
DWORD WINAPI UserRegisterWowHandlers(APFNWOWHANDLERSIN, APFNWOWHANDLERSOUT);
/* No boot task runs in this fixture. Direct launcher requests do not inspect
 * this original startup identity, so do not link W32Thread as a substitute. */
struct _TD *gptdShell;
static PFNWOWHANDLERSOUT output;
static unsigned errors, notifications, ticket_binds;
static unsigned fail_count;
static BOOL raise_notification, retire_notification;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, \
    "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

unsigned long __cdecl mvdm_softpc_wow_page_domain_guest_shared_info(void)
{ return 0x1000; }
int __cdecl mvdm_softpc_wow_page_domain_client_desktop_ready(void)
{ return 1; }
static BOOL initialize_task(void)
{
    return output.pfnInitTask(0x0400, "APP.EXE", 0x1234, 0, 0x5678,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, SW_HIDE);
}
DWORD WINAPI wow_fixture_NtvdmReportWowStartup(ULONG request, USHORT task, BOOL bind)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    CHECK(request == 0x5678 && binding && !binding->exclusive_held);
    if (bind) {
        CHECK(task == 0x1234 && notifications > fail_count);
        ++ticket_binds;
        return ERROR_SUCCESS;
    }
    CHECK(!task);
    ++notifications;
    CHECK(!initialize_task() && GetLastError() == ERROR_INVALID_STATE);
    if (raise_notification) RaiseException(0xe0447002u, 0, 0, NULL);
    if (retire_notification) CHECK(wow_user_task_lifecycle_unbind(binding));
    return notifications <= fail_count ? ERROR_RETRY : ERROR_SUCCESS;
}
static VOID WINAPI end_task(void) { }

int __cdecl main(void)
{
    if (!wow_class_client_bind(&wow_fixture_class_client_provider)) return 3;
    session owner;
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    PFNWOWHANDLERSIN input = {0};
    wow_user_task_lifecycle *lifecycle;
    HANDLE event = CreateEventW(NULL, FALSE, FALSE, NULL);
    void *committed;
    unsigned i;
    BOOL caught = FALSE;
    CHECK(ntvdm_dpmi_initialize());
    CHECK(wow_profile_bind(wow_original_task_profile, FastGetProfileIntW));
    CHECK(wow_button_bind(wow_original_button_data, FreeLookasideEntry));
    CHECK(wow_cleanup_bind(&wow_original_cleanup_provider));
    CHECK(wow_standard_class_bind(&class_provider));
    wow_fixture_startup_bind();
    session_initialize(&owner, 44702);
    CHECK(session_activate(&owner));
    CHECK(session_thread_bind_owned(&owner, SESSION_THREAD_BINDING_ORIGINAL_WORKER));
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    CHECK(UserRegisterWowHandlers(&input, &output) == 0x1000);
    CHECK(output.pfnRegisterUserHungAppHandlers(end_task, event));
    lifecycle = runtime.lifecycle;

    fail_count = 2;
    CHECK(!initialize_task() && GetLastError() == ERROR_RETRY);
    committed = binding.thread;
    CHECK(committed && notifications == 1 && ticket_binds == 0);
    CHECK(!initialize_task() && GetLastError() == ERROR_RETRY);
    CHECK(ticket_binds == 0);
    CHECK(initialize_task());
    CHECK(binding.thread == committed && notifications == 3);
    CHECK(!initialize_task() && GetLastError() == ERROR_INVALID_STATE);
    CHECK(notifications == 3 && ticket_binds == 1);
    CHECK(wow_user_task_lifecycle_unbind(&binding));

    /* Original W32Thread owns the seven-attempt bound; verify its callback
     * can be attempted seven times without reinitialization, then retired. */
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    notifications = 0; fail_count = 7;
    for (i = 0; i < 7; ++i)
        CHECK(!initialize_task() && GetLastError() == ERROR_RETRY);
    CHECK(notifications == 7 && ticket_binds == 1);
    CHECK(output.pfnWOWCleanup((HANDLE)0x1234, 0x1234, NULL, 0));
    CHECK(binding.thread != NULL); /* Committed case retains normal retirement. */
    CHECK(wow_user_task_lifecycle_unbind(&binding));
    CHECK(lifecycle->threads == NULL);

    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    notifications = fail_count = 0; raise_notification = TRUE;
    __try { (void)initialize_task(); }
    __except (GetExceptionCode() == 0xe0447002u ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && binding.thread && !binding.exclusive_held);
    raise_notification = FALSE;
    CHECK(initialize_task());
    CHECK(notifications == 2 && ticket_binds == 2);
    CHECK(wow_user_task_lifecycle_unbind(&binding));

    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    retire_notification = TRUE;
    CHECK(!initialize_task() && GetLastError() == ERROR_OPERATION_ABORTED);
    CHECK(!wow_user_runtime_current() && !lifecycle->threads);
    CHECK(ticket_binds == 2);
    retire_notification = FALSE;
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    CHECK(initialize_task());
    CHECK(ticket_binds == 3);
    CHECK(wow_user_task_lifecycle_unbind(&binding));
    wow_user_task_lifecycle_dispose(lifecycle);
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    CloseHandle(event);
    fprintf(stderr, "WOW_INIT_NOTIFICATION errors=%u registration/retry/exhaustion/exception/retirement checked\n", errors);
    return errors ? 1 : 0;
}
