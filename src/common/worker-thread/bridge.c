#include "common/worker-thread/initialize.h"
#include "common/worker-thread/session.h"
#include "compat/include/thread_lifecycle.h"

static void finish_owner(void)
{
    struct session *owner = session_thread_current();
    if (owner != NULL && !session_thread_finish(owner)) {
        EXCEPTION_RECORD record = {0};
        record.ExceptionCode = 0xc0000420;
        record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
        record.NumberParameters = 1;
        record.ExceptionInformation[0] = GetLastError();
        RaiseFailFastException(&record, NULL, 0);
    }
}

static int enter_owner(void *owner, const char *name)
{
    return session_thread_bind_owned_source((struct session *)owner,
        SESSION_THREAD_BINDING_ORIGINAL_WORKER, name);
}

static void capture_owner(win32_thread_hooks *hooks)
{
    hooks->owner = session_thread_current();
    hooks->enter = enter_owner;
    hooks->finish = finish_owner;
}

BOOL worker_thread_initialize(void)
{
    static const opennt_thread_lifecycle lifecycle = {capture_owner, finish_owner};
    return opennt_thread_install_lifecycle(&lifecycle);
}
