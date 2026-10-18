#include "wow_user_message_bridge.h"
#include "wow_task_order_bindings.h"
#include "wow_user_runtime.h"
#include "wow_user_task_lifecycle.h"

/* ADAPTER-WOW-050: ssend.c MAKECALL releases the USER data lock across a
 * client callback, independently of WOW task execution ownership. Native
 * PeekMessage may call the client before returning. Restore the data lock
 * even on unwind, before the caller touches its original queue view again. */
static BOOL native_peek(MSG *message, UINT flags, BOOL scheduler_wait)
{
    BOOL restored = TRUE;
    wow_user_native_call call;
    DWORD error = GetLastError();
    if (!wow_user_native_call_begin_inline(&call)) return FALSE;
    __try {
        SetLastError(error);
        (void)PeekMessageW(message, NULL, 0, 0, flags);
    } __finally {
        error = GetLastError();
        restored = scheduler_wait ? wow_user_native_call_end_wait(&call) :
            wow_user_native_call_end(&call);
        if (restored) SetLastError(error);
    }
    return restored;
}

static BOOL verify_owner(const wow_user_message_bridge *bridge,
    const wow_task_order_thread *thread)
{
    if (!bridge || !thread || !thread->pcti || !bridge->initialized ||
            bridge->owner_thread_id != GetCurrentThreadId()) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI wow_user_message_bridge_initialize(wow_user_message_bridge *bridge)
{
    MSG ignored;

    if (!bridge || bridge->initialized) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* PeekMessage creates this thread's real USER32 queue without consuming a
     * posted message.  This is the only queue state the bridge owns. */
    if (!native_peek(&ignored, PM_NOREMOVE, FALSE)) return FALSE;
    bridge->owner_thread_id = GetCurrentThreadId();
    bridge->initialized = TRUE;
    return TRUE;
}

BOOL WINAPI wow_user_message_bridge_sync(wow_user_message_bridge *bridge,
    wow_task_order_thread *thread)
{
    if (!verify_owner(bridge, thread)) return FALSE;
    /* ADAPTER-WOW-093: queue creation belongs to initialize, and receive
     * already dispatched sends. An extra unfiltered peek observes posted
     * messages/Hooks and repeats the entire native-return publication. */
    thread->pcti->fsWakeBits &= (WORD)~QS_SENDMESSAGE;
    return TRUE;
}

BOOL WINAPI wow_user_message_bridge_receive(wow_user_message_bridge *bridge,
    wow_task_order_thread *thread)
{
    MSG message;

    if (!verify_owner(bridge, thread)) return FALSE;
    /* userk.h::xxxReceiveMessages drains QS_SENDMESSAGE only. Public USER
     * dispatches pending nonqueued sends during PeekMessage; do not remove
     * posted messages or WM_QUIT from the original guest Get/PeekMessage loop.
     * In particular, there is no autonomous Translate/DispatchMessage loop. */
    if (!native_peek(&message, PM_NOREMOVE | PM_QS_SENDMESSAGE, FALSE)) return FALSE;
    return wow_user_message_bridge_sync(bridge, thread);
}

BOOL WINAPI wow_user_message_bridge_receive_wait(wow_user_message_bridge *bridge,
    wow_task_order_thread *thread)
{
    MSG message;
    if (!verify_owner(bridge, thread)) return FALSE;
    /* No posted removal, unfiltered second peek, native WOW yield or fake
     * receive identity. Actual callbacks retain their ordinary entry scope. */
    return native_peek(&message,
        PM_NOREMOVE | PM_QS_SENDMESSAGE | PM_NOYIELD, TRUE);
}

void WINAPI wow_user_message_bridge_dispose(wow_user_message_bridge *bridge)
{
    if (!bridge) return;
    bridge->owner_thread_id = 0;
    bridge->initialized = FALSE;
}
