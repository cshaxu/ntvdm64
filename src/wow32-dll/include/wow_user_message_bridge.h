#ifndef WOW_USER_MESSAGE_BRIDGE_H
#define WOW_USER_MESSAGE_BRIDGE_H

#include <windows.h>

struct wow_task_order_thread;

/* ADAPTER-WOW-050: finite public-USER queue consumer for the current USER
 * thread. Original userk.h::xxxReceiveMessages reaches private SMS/PWND
 * state, which is an explicit stopping boundary. Modern USER32 only makes a
 * cross-thread synchronous send reliably observable while its normal queue is
 * serviced. A non-removing, sent-message-only peek performs that delivery;
 * posted messages and WM_QUIT remain owned by the guest's message loop.
 * The bridge owns neither a parallel SMS list nor a second queue. */
typedef struct wow_user_message_bridge {
    DWORD owner_thread_id;
    BOOL initialized;
} wow_user_message_bridge;

BOOL WINAPI wow_user_message_bridge_initialize(wow_user_message_bridge *);
BOOL WINAPI wow_user_message_bridge_sync(wow_user_message_bridge *,
    struct wow_task_order_thread *);
BOOL WINAPI wow_user_message_bridge_receive(wow_user_message_bridge *,
    struct wow_task_order_thread *);
void WINAPI wow_user_message_bridge_dispose(wow_user_message_bridge *);

#endif
