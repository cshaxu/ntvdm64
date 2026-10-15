#ifndef WOW_WINDOW_WORDS_BINDING_H
#define WOW_WINDOW_WORDS_BINDING_H
#include <windows.h>
#include "wow_window_dispatch_bindings.h"
#include "wow_class_words_binding.h"

struct _WW;
struct wow_user_object_table;
typedef struct wow_window_words_binding wow_window_words_binding;
/* Unpublished storage owns one reference only: no HWND property, class
 * association or cleanup membership. A scope may later enroll this identity. */
wow_window_words_binding *WINAPI wow_window_words_allocate(void);

/* Native storage only. The caller initializes the complete original WW,
 * publishes before the first callback, and detaches on creation rollback or
 * terminal destruction. Acquisitions survive detach; raw words do not survive
 * release. This does not replace native window lifetime or guest projection. */
BOOL WINAPI wow_window_words_attach(HWND, const struct _WW *);
/* Success transfers one already acquired class reference to this window;
 * failure leaves it with the caller. The caller selected the actual class
 * before creation, as original createw.c does; this API does not resolve it. */
BOOL WINAPI wow_window_words_attach_class(HWND, const struct _WW *,
    wow_class_words_binding *);
BOOL WINAPI wow_window_words_attach_target(HWND, const struct _WW *,
    wow_class_words_binding *, const wow_window_dispatch_target *);
LRESULT CALLBACK wow_window_native_proc(HWND, UINT, WPARAM, LPARAM);
/* Shared normalized native CPD entry for WND/CLS cleanup fields. A window
 * snapshots the logical class target into its own CPD storage before calling
 * this entry; this is never an externally visible procedure identity. */
LRESULT CALLBACK wow_window_call_native_previous(HWND, UINT, WPARAM, LPARAM);
/* Storage boundary for an already validated procedure, not full USER
 * SetWindowLong policy (CPD/ANSI conversion and zombie checks belong above).
 * Encoded WOW addresses remain opaque; exchange does not alter WW words.
 * Public CallProc identities are kept separately from the cleanup procedure
 * and forwarded through CallWindowProcA. Reads/exchanges return the original
 * identity, never the forwarding address; FFFF is not a WOW procedure tag. */
BOOL WINAPI wow_window_procedure_read(HWND, DWORD *);
BOOL WINAPI wow_window_class_procedure_read(HWND, DWORD *);
BOOL WINAPI wow_window_class_procedure_exchange(HWND, DWORD, DWORD *);
BOOL WINAPI wow_window_procedure_exchange(HWND, DWORD, DWORD *);
wow_window_words_binding *WINAPI wow_window_words_acquire(HWND);
struct _WW *WINAPI wow_window_words_value(wow_window_words_binding *);
/* Refresh only native WND fields aliased by WW, preserving original WOW
 * classification/procedure words. Caller retains the matching binding. */
BOOL WINAPI wow_window_words_refresh(HWND, wow_window_words_binding *);
BOOL WINAPI wow_window_words_exchange(HWND, unsigned, DWORD, DWORD *);
/* Live WND cleanup fields, not a copy. Caller retains this window borrow and
 * serializes membership/cleanup against dispatch; no thread is synthesized.
 * The procedure is the very field read by wow_window_native_proc. */
wow_cleanup_window *WINAPI wow_window_words_cleanup_value(wow_window_words_binding *);
BOOL WINAPI wow_window_words_publish_owner(HWND, wow_window_words_binding *,
    struct wow_task_order_thread *);
/* Same borrowed-window lifetime as WW; no extra release or copied WC. */
DWORD *WINAPI wow_window_class_words_value(wow_window_words_binding *);
void WINAPI wow_window_words_release(wow_window_words_binding *);
void WINAPI wow_window_words_detach(HWND);
/* Rollback only its own publication; expected must remain borrowed through
 * this call. A replaced property is untouched, even on the same HWND. */
BOOL WINAPI wow_window_words_detach_if(HWND, wow_window_words_binding *);
/* Terminal/veto paths consume the native property even if copied retirement
 * must wait for the original execution context. The existing object-table
 * reference retains storage; collection retries only such pending owners. */
BOOL WINAPI wow_window_words_terminal(HWND, wow_window_words_binding *);
BOOL WINAPI wow_window_words_collect_retired(struct wow_user_object_table *);
/* Tagged procedures require the registered WOW callback; native procedures
 * do not. FALSE means no dispatch occurred; result is untouched. Exceptions propagate after
 * releasing the borrowed WW. The original owner performs actual dispatch. */
BOOL WINAPI wow_window_dispatch_bound(HWND, UINT, WPARAM, LPARAM, DWORD,
    wow_window_callback, LONG *);
#endif
