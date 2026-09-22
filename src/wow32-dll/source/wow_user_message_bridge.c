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
    if (bridge->quit_received) {
        SetLastError(ERROR_CANCELLED);
        return FALSE;
    }
    /* Public USER32 has one queue/dispatch operation for synchronous sends
     * and posted messages. Do not invent a second selector/SMS state merely
     * to retain the NT4 server's internal split. */
    while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            bridge->quit_code = (DWORD)message.wParam;
            bridge->quit_received = TRUE;
            thread->pcti->fsWakeBits &= (WORD)~QS_SENDMESSAGE;
            SetLastError(ERROR_CANCELLED);
            return FALSE;
        }
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return wow_user_message_bridge_sync(bridge, thread);
}

void WINAPI wow_user_message_bridge_dispose(wow_user_message_bridge *bridge)
{
    if (!bridge) return;
    bridge->owner_thread_id = 0;
    bridge->quit_code = 0;
    bridge->initialized = FALSE;
    bridge->quit_received = FALSE;
}
