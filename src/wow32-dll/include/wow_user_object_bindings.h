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
BOOL WINAPI wow_user_window_cancel_native_adoption(wow_user_object_table *, HWND,
    struct wow_cleanup_window *, ULONG);
HWND WINAPI wow_user_window_full_handle(wow_user_object_table *, WORD);
/* A public native USER return value can re-enter WOW only when it names a
 * currently published window in this worker.  The existing table is the sole
 * identity authority: do not truncate an unrelated native HWND into a guest
 * alias and do not allocate a proxy alias for it. */
HWND WINAPI wow_user_window_guest_handle(wow_user_object_table *, HWND);
/* Original message-conversion contract, including dead/non-window fallback.
 * Not a replacement for the strict lookup above at ownership boundaries. */
DWORD WINAPI wow_user_full_handle(wow_user_object_table *, WORD);
/* Original WOW conversion projection. Short forms consume the existing
 * index/uniqueness representation, never a new native sample. */
HWND WINAPI wow_user_window_native_handle(HWND);
/* Public modern USER boundary only. A short guest carrier resolves to a live
 * owned table entry or to the already committed shared-window observation;
 * no caller may reuse this as a general original conversion helper. */
HWND WINAPI wow_user_window_public_handle(HWND);
/* The reverse public-USER boundary first consults an owned object, then the
 * already committed original shared-window observation.  Both require exact
 * full native identity; neither may truncate or allocate a new alias. */
HWND WINAPI wow_user_window_public_guest_handle(HWND);
/* Call-local public-USER relationship sample. No WW, task association or
 * native ownership is created. Identities remain native until publication;
 * the desktop must be translated by the existing guest-view owner. */
typedef struct wow_user_window_links {
    HWND window, next, parent, child, owner;
} wow_user_window_links;
BOOL WINAPI wow_user_window_sample_links(HWND, wow_user_window_links *);
/* Prepare all query nodes and publish one validated relation closure. Does
 * not acquire task ownership. Dead observed generations can be withdrawn
 * only if no unchanged published source still points to their backing;
 * managed objects remain exclusively with their terminal owner. */
BOOL WINAPI wow_user_window_project_graph(const HWND *, ULONG);
/* Existing execution owner calls this before guest callbacks and after
 * native mutations. Includes live unlinked creation/terminal windows from
 * the managed table; does not acquire execution or native USER ownership. */
BOOL WINAPI wow_user_window_refresh_graph(HWND);
/* Typed callback payload identities, sampled in the same graph transaction.
 * Caller excludes insertion-order constants and non-window union members. */
BOOL WINAPI wow_user_window_refresh_payload(HWND, const HWND *, ULONG);
/* Native SDK message operands only; original WOW retains all conversion.
 * Caller supplies four slots. No window queries or guest allocation occur. */
ULONG WINAPI wow_user_message_window_roots(UINT, WPARAM, LPARAM, HWND [4]);
BOOL WINAPI wow_user_window_refresh_message(HWND, UINT, WPARAM, LPARAM);
ULONG WINAPI wow_user_queue_window_roots(const MSG *, HWND [4]);
ULONG WINAPI wow_user_message_return_roots(UINT, WPARAM, LPARAM, LRESULT, HWND [4]);
#endif
