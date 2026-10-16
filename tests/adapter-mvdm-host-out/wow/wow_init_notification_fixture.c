/* The original declaration relies on the provider's /Gz. Annotate that one
 * declaration while this host fixture uses cdecl worker/session headers. */
#define UserRegisterWowHandlers __stdcall UserRegisterWowHandlers
#include "wow_user_task_lifecycle.h"
#undef UserRegisterWowHandlers
#include "ntvdm-exe/session/session.h"
#include <stdio.h>

/* Actual registration and lifecycle code; only guest publication and the
 * broker RPC are seams. This fixture does not claim to execute guest code. */
DWORD WINAPI UserRegisterWowHandlers(APFNWOWHANDLERSIN, APFNWOWHANDLERSOUT);
/* No boot task runs in this fixture. Direct launcher requests do not inspect
 * this original startup identity, so do not link W32Thread as a substitute. */
struct _TD *gptdShell;
static PFNWOWHANDLERSOUT output;
static unsigned errors, notifications, styles;
static unsigned fail_count;
static BOOL raise_notification, retire_notification;
static BOOL reject_style;
void WINAPI wow_fixture_startup_td_begin(WORD);
void WINAPI wow_fixture_startup_td_end(void);
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, \
    "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

unsigned long __cdecl mvdm_softpc_wow_page_domain_guest_shared_info(void)
{ return 0x1000; }
int __cdecl mvdm_softpc_wow_page_domain_client_desktop_ready(void)
{ return 1; }
DWORD WINAPI OpenNtBaseClientWowStyle(ULONG request, DWORD *style)
{
    CHECK(request == 0x5678);
    ++styles;
    if (reject_style) return ERROR_NOT_ENOUGH_MEMORY;
    *style = WINDOW_STYLE_OFF;
    return ERROR_SUCCESS;
}
static BOOL initialize_task(void)
{
    return output.pfnInitTask(0x0400, "APP.EXE", 0x1234, 0, 0x5678,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT,
        (DWORD)CW_USEDEFAULT, (DWORD)CW_USEDEFAULT, SW_HIDE);
}
DWORD WINAPI OpenNtBaseClientWowStarted(ULONG request)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    CHECK(request == 0x5678 && binding && !binding->exclusive_held);
    ++notifications;
    CHECK(!initialize_task() && GetLastError() == ERROR_INVALID_STATE);
    if (raise_notification) RaiseException(0xe0447002u, 0, 0, NULL);
    if (retire_notification) CHECK(wow_user_runtime_unbind(binding));
    return notifications <= fail_count ? ERROR_RETRY : ERROR_SUCCESS;
}
static VOID WINAPI end_task(void) { }

int __cdecl main(void)
{
    session owner;
    wow_user_runtime runtime = WOW_USER_RUNTIME_INITIALIZER;
    wow_user_runtime_thread binding = {0};
    PFNWOWHANDLERSIN input = {0};
    wow_user_task_lifecycle *lifecycle;
    HANDLE event = CreateEventW(NULL, FALSE, FALSE, NULL);
    void *committed;
    unsigned i;
    BOOL caught = FALSE;
    session_initialize(&owner, 44702);
    CHECK(session_activate(&owner));
    CHECK(session_thread_bind_owned(&owner, SESSION_THREAD_BINDING_ORIGINAL_WORKER));
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    CHECK(UserRegisterWowHandlers(&input, &output) == 0x1000);
    CHECK(output.pfnRegisterUserHungAppHandlers(end_task, event));
    lifecycle = runtime.lifecycle;

    /* The actual registered cleanup must accept exactly the still-linked,
     * never-committed original task after all initialization attempts fail. */
    wow_fixture_startup_td_begin(0x1234);
    reject_style = TRUE;
    for (i = 0; i < 7; ++i)
        CHECK(!initialize_task() && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    CHECK(!binding.thread && !lifecycle->threads && styles == 7 && !notifications);
    CHECK(output.pfnWOWCleanup((HANDLE)0x1234, 0x1234, NULL, 0));
    CHECK(!binding.thread && !lifecycle->threads);
    wow_fixture_startup_td_end();
    reject_style = FALSE; styles = 0;
    fprintf(stderr,"WOW_INIT_PRECOMMIT_CLEANUP errors=%u retries=7 registered_cleanup=1\n",errors);

    fail_count = 2;
    CHECK(!initialize_task() && GetLastError() == ERROR_RETRY);
    committed = binding.thread;
    CHECK(committed && styles == 1 && notifications == 1);
    CHECK(!initialize_task() && GetLastError() == ERROR_RETRY);
    CHECK(initialize_task());
    CHECK(binding.thread == committed && styles == 1 && notifications == 3);
    CHECK(!initialize_task() && GetLastError() == ERROR_INVALID_STATE);
    CHECK(notifications == 3);
    CHECK(wow_user_runtime_unbind(&binding));

    /* Original W32Thread owns the seven-attempt bound; verify its callback
     * can be attempted seven times without reinitialization, then retired. */
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    notifications = styles = 0; fail_count = 7;
    for (i = 0; i < 7; ++i)
        CHECK(!initialize_task() && GetLastError() == ERROR_RETRY);
    CHECK(styles == 1 && notifications == 7);
    CHECK(output.pfnWOWCleanup((HANDLE)0x1234, 0x1234, NULL, 0));
    CHECK(binding.thread != NULL); /* Committed case retains normal retirement. */
    CHECK(wow_user_runtime_unbind(&binding));
    CHECK(lifecycle->threads == NULL);

    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    notifications = styles = fail_count = 0; raise_notification = TRUE;
    __try { (void)initialize_task(); }
    __except (GetExceptionCode() == 0xe0447002u ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && binding.thread && !binding.exclusive_held);
    raise_notification = FALSE;
    CHECK(initialize_task());
    CHECK(styles == 1 && notifications == 2);
    CHECK(wow_user_runtime_unbind(&binding));

    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    retire_notification = TRUE;
    CHECK(!initialize_task() && GetLastError() == ERROR_OPERATION_ABORTED);
    CHECK(!wow_user_runtime_current() && !lifecycle->threads);
    retire_notification = FALSE;
    CHECK(wow_user_runtime_bind(&binding, &runtime, NULL, NULL));
    CHECK(initialize_task());
    CHECK(wow_user_runtime_unbind(&binding));
    wow_user_task_lifecycle_dispose(lifecycle);
    CHECK(session_thread_unbind(&owner));
    CHECK(session_dispose(&owner));
    CloseHandle(event);
    fprintf(stderr, "WOW_INIT_NOTIFICATION errors=%u registration/retry/exhaustion/exception/retirement checked\n", errors);
    return errors ? 1 : 0;
}
