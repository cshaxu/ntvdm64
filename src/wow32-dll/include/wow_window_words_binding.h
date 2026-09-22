#ifndef WOW_WINDOW_WORDS_BINDING_H
#define WOW_WINDOW_WORDS_BINDING_H
#include <windows.h>
#include "wow_window_dispatch_bindings.h"
#include "wow_class_words_binding.h"

struct _WW;
typedef struct wow_window_words_binding wow_window_words_binding;

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
/* Storage boundary for an already validated procedure, not full USER
 * SetWindowLong policy (CPD/ANSI conversion and zombie checks belong above).
 * Encoded WOW addresses remain opaque; exchange does not alter WW words.
 * CallProc handles must be resolved by that policy before storage/dispatch;
 * their FFFF tag is not a callable WOW procedure. */
BOOL WINAPI wow_window_procedure_read(HWND, DWORD *);
BOOL WINAPI wow_window_class_procedure_read(HWND, DWORD *);
BOOL WINAPI wow_window_class_procedure_exchange(HWND, DWORD, DWORD *);
BOOL WINAPI wow_window_procedure_exchange(HWND, DWORD, DWORD *);
wow_window_words_binding *WINAPI wow_window_words_acquire(HWND);
struct _WW *WINAPI wow_window_words_value(wow_window_words_binding *);
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
/* Tagged procedures require the registered WOW callback; native procedures
 * do not. FALSE means no dispatch occurred; result is untouched. Exceptions propagate after
 * releasing the borrowed WW. The original owner performs actual dispatch. */
BOOL WINAPI wow_window_dispatch_bound(HWND, UINT, WPARAM, LPARAM, DWORD,
    wow_window_callback, LONG *);
#endif
