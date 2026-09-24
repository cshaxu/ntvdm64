#include "wow_user_message_bridge.h"
#include "wow_task_order_bindings.h"

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
    (void)PeekMessageW(&ignored, NULL, 0, 0, PM_NOREMOVE);
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
    (void)PeekMessageW(&ignored, NULL, 0, 0, PM_NOREMOVE);
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
    (void)PeekMessageW(&message, NULL, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
    return wow_user_message_bridge_sync(bridge, thread);
}

void WINAPI wow_user_message_bridge_dispose(wow_user_message_bridge *bridge)
{
    if (!bridge) return;
    bridge->owner_thread_id = 0;
    bridge->initialized = FALSE;
}
