#include "wow_user_message_bridge.h"
#include "wow_task_order_bindings.h"
#include "wow_user_runtime.h"

/* ADAPTER-WOW-050: ssend.c MAKECALL releases the USER data lock across a
 * client callback, independently of WOW task execution ownership. Native
 * PeekMessage may call the client before returning. Restore the data lock
 * even on unwind, before the caller touches its original queue view again. */
static BOOL native_peek(MSG *message, UINT flags)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    BOOL held = binding && binding->exclusive_held;
    BOOL restored = TRUE;
    if (held && !wow_user_runtime_leave(binding)) return FALSE;
    __try {
        (void)PeekMessageW(message, NULL, 0, 0, flags);
    } __finally {
        if (held) restored = wow_user_runtime_enter(binding);
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
    if (!native_peek(&ignored, PM_NOREMOVE)) return FALSE;
    bridge->owner_thread_id = GetCurrentThreadId();
    bridge->initialized = TRUE;
    return TRUE;
}

BOOL WINAPI wow_user_message_bridge_sync(wow_user_message_bridge *bridge,
    wow_task_order_thread *thread)
{
    MSG ignored;

    if (!verify_owner(bridge, thread)) return FALSE;
    /* USER32 exposes no non-consuming "pending SendMessage" probe. A
     * non-removing peek is still useful to create/observe this thread's real
     * queue, but W2's receive entry below performs actual delivery. */
    if (!native_peek(&ignored, PM_NOREMOVE)) return FALSE;
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
    if (!native_peek(&message, PM_NOREMOVE | PM_QS_SENDMESSAGE)) return FALSE;
    return wow_user_message_bridge_sync(bridge, thread);
}

void WINAPI wow_user_message_bridge_dispose(wow_user_message_bridge *bridge)
{
    if (!bridge) return;
    bridge->owner_thread_id = 0;
    bridge->initialized = FALSE;
}
