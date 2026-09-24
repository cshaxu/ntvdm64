#ifndef WOW_USER_OBJECT_BINDINGS_H
#define WOW_USER_OBJECT_BINDINGS_H
#include <windows.h>
#include "opennt-host/windows/core/ntuser/inc/user.h"
struct wow_task_order_thread;
struct wow_cleanup_window;
typedef struct wow_cleanup_handle {
    PVOID phead, pOwner;
    BYTE bType, bFlags;
    WORD wUniq; /* Original HANDLEENTRY field; copied from native HIWORD. */
} wow_cleanup_handle;
/* ADAPTER-WOW-048: native object companion view indexed by the existing WOW
 * 16-bit handle alias. No handle generation or native object allocation.
 * The USER owner supplies zeroed storage and serializes publication, cleanup
 * and retirement. It retains this domain until every entry is retired. Full
 * native identity and a still-borrowed phead prevent stale removal from
 * clearing a replacement. Expected objects must remain borrowed during retire.
 * This is not an NT4 binary layout or a guest-published shared handle table. */
typedef struct wow_user_object_table {
    wow_cleanup_handle entries[0x10000];
    /* Native identity is worker-local lookup state only. The paired guest
     * HANDLEENTRY carries a separately allocated original-layout WND, never
     * this value or any other host pointer. */
    HWND windows[0x10000];
    ULONG last_handle;
} wow_user_object_table;
BOOL WINAPI wow_user_window_publish(wow_user_object_table *, HWND,
    struct wow_cleanup_window *, struct wow_task_order_thread *);
BOOL WINAPI wow_user_window_retire(wow_user_object_table *, HWND,
    struct wow_cleanup_window *, ULONG *retained_backing);
HWND WINAPI wow_user_window_full_handle(wow_user_object_table *, WORD);
#endif
