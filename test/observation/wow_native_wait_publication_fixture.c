/* Control-flow unit of generated, exact production publication helpers.
 * Lower-service stand-ins expose duplicate work and failure retirement only;
 * this is not a replacement for real scheduler/CCPU/guest acceptance. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct wow_task_order_thread wow_task_order_thread;
typedef struct { wow_task_order_thread *CSOwningThread; } model_process;
typedef struct { model_process *pwpi; } model_ppi;
struct wow_task_order_thread { model_ppi *ppi; };
typedef struct { BOOL wait_publication_pending; } wow_user_task_lifecycle_thread;
typedef struct { wow_user_task_lifecycle_thread *lifecycle; } model_runtime;
typedef struct { wow_task_order_thread *thread; model_runtime *runtime; }
    wow_user_runtime_thread;
typedef struct { unsigned unused; } wow_user_native_call;
static unsigned errors, seen, calls, fail_stage, throw_stage, fatal_calls;
static model_process fixture_process;
static model_ppi fixture_ppi = { &fixture_process };
static wow_task_order_thread fixture_thread = { &fixture_ppi };
static wow_user_task_lifecycle_thread fixture_task;
static model_runtime fixture_runtime = { &fixture_task };
static wow_user_runtime_thread fixture_binding = { &fixture_thread, &fixture_runtime };
#define CHECK(x) do { if (!(x)) { ++errors; \
    fprintf(stderr,"FAIL line=%d error=%lu\n",__LINE__,GetLastError()); } } while (0)
static wow_user_task_lifecycle_thread *current_task(
    wow_user_task_lifecycle_thread *owner) { return owner; }
static BOOL service(unsigned stage)
{
    seen |= 1u << stage; ++calls;
    SetLastError(0x44709300u + stage);
    if (throw_stage == stage)
        RaiseException(0xe0447393u, EXCEPTION_NONCONTINUABLE, 0, NULL);
    return fail_stage != stage;
}
static BOOL publish_call_windows(const wow_user_native_call *call)
{ (void)call; return service(1); }
static BOOL wow_user_window_refresh_graph(HWND window)
{ (void)window; return service(2); }
static BOOL wow_user_borrow_client_graph(void *owner, HWND window)
{ (void)owner; (void)window; return service(3); }
static BOOL wow_menu_client_refresh_all(model_ppi *owner)
{ CHECK(owner == &fixture_ppi); return service(4); }
static __declspec(noreturn) void wow_user_native_call_failed(
    const char *where, DWORD error)
{
    (void)where; (void)error; ++fatal_calls;
    /* Stand-in observes state at production fail-fast selection. It does
     * not claim real RaiseFailFastException is catchable/recoverable. */
    RaiseException(0xe0447394u, EXCEPTION_NONCONTINUABLE, 0, NULL);
    abort();
}
#include "native_wait_publication_body.h"
static void reset(BOOL pending, BOOL owned)
{
    seen = calls = fail_stage = throw_stage = fatal_calls = 0;
    fixture_task.wait_publication_pending = pending;
    fixture_process.CSOwningThread = owned ? &fixture_thread : NULL;
    SetLastError(0x447093ffu);
}
int main(void)
{
    wow_user_native_call call = {0};
    unsigned stage=0, after=0;
    BOOL caught=FALSE;
    CHECK(current_task(&fixture_task)==&fixture_task);
    reset(FALSE, TRUE);
    publish_scheduled_return(&fixture_binding);
    CHECK(calls == 0 && !fixture_task.wait_publication_pending);
    CHECK(GetLastError() == 0x447093ffu);
    reset(TRUE, FALSE);
    publish_scheduled_return(&fixture_binding);
    CHECK(calls == 0 && fixture_task.wait_publication_pending);
    CHECK(GetLastError() == 0x447093ffu);
    reset(TRUE, TRUE);
    publish_scheduled_return(&fixture_binding);
    CHECK(seen == 28u && calls == 3 && !fixture_task.wait_publication_pending);
    CHECK(GetLastError() == 0x447093ffu);
    after = calls;
    publish_scheduled_return(&fixture_binding);
    CHECK(calls == after && GetLastError() == 0x447093ffu);
    reset(TRUE, TRUE);
    CHECK(publish_guest_return(&fixture_binding, &call));
    CHECK(seen == 26u && calls == 3 && !fixture_task.wait_publication_pending);
    for (stage = 2; stage <= 4; ++stage) {
        reset(TRUE, TRUE); fail_stage = stage;
        CHECK(!publish_guest_return(&fixture_binding, NULL));
        CHECK(fixture_task.wait_publication_pending);
        CHECK(GetLastError() == 0x44709300u + stage);
        reset(TRUE, TRUE); throw_stage = stage; caught = FALSE;
        __try { (void)publish_guest_return(&fixture_binding, NULL); }
        __except(GetExceptionCode() == 0xe0447393u ?
                EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            caught = TRUE;
        }
        CHECK(caught && fixture_task.wait_publication_pending && !fatal_calls);
    }
    reset(TRUE, TRUE); fail_stage = 2; caught = FALSE;
    __try { publish_scheduled_return(&fixture_binding); }
    __except(GetExceptionCode() == 0xe0447394u ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught && fatal_calls == 1 && fixture_task.wait_publication_pending);
    printf("NATIVE_WAIT_PUBLICATION_BODY errors=%u no_debt=1 native_continuation=1 "
        "discharged_once=1 ordinary_return=1 failure_retained=1 "
        "exception_retained=1 last_error=1\n",errors);
    return errors != 0;
}
