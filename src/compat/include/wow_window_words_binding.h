#ifndef WOW_WINDOW_WORDS_BINDING_H
#define WOW_WINDOW_WORDS_BINDING_H
#include <windows.h>
#include "wow_window_dispatch_bindings.h"
#include "wow_window_procedure_entry.h"
#include "wow_class_words_binding.h"

struct _WW;
struct tagWND;
struct wow_user_object_table;
typedef struct wow_window_words_binding wow_window_words_binding;
/* Trusted resource-only WM_DESTROY owner, not an application procedure.
 * Registration/consumption is same-GUI-thread and exact-generation scoped. */
typedef UINT (WINAPI *wow_window_resource_destroy)(HWND,UINT,WPARAM,LPARAM);
BOOL WINAPI wow_window_words_bind_resource_destroy(HWND,wow_window_resource_destroy);
/* Ordinary owner clears with expected!=NULL/invoke=FALSE. The terminal
 * edge invokes with expected=NULL only under early task-cleanup permission. */
BOOL WINAPI wow_window_words_consume_resource_destroy(HWND,
    wow_window_resource_destroy expected,BOOL invoke);
/* Unpublished storage owns one reference only: no HWND property, class
 * association or cleanup membership. A scope may later enroll this identity. */
wow_window_words_binding *WINAPI wow_window_words_allocate(void);
/* Existing page-domain generation owns the shared observed storage. Caller
 * holds its backing borrow and receives an additional host storage reference.
 * No property/task enrollment or native window ownership is introduced. */
wow_window_words_binding *WINAPI wow_window_words_acquire_generation(ULONG);
/* Caller already owns a host reference. Return a transient generation pin,
 * or zero for property-only storage; release nonzero pins in its scope. */
BOOL WINAPI wow_window_words_pin_generation(wow_window_words_binding *, ULONG *);

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
/* Read actual native WND selection, decoding only our immutable guest entry.
 * The caller retains an enrolled view; no shadow-current target is selected. */
BOOL WINAPI wow_window_procedure_read(HWND, DWORD *);
BOOL WINAPI wow_window_class_target_read(HWND, wow_window_dispatch_target *);
BOOL WINAPI wow_window_class_procedure_exchange(HWND, DWORD, BOOL guest,
    DWORD *, BOOL *previous_guest);
/* Immutable native entry dispatch. The supplied typed target is saved by
 * the caller, independent of this HWND's current native procedure. */
BOOL WINAPI wow_window_dispatch_immutable(HWND, UINT, WPARAM, LPARAM,
    const wow_window_dispatch_target *, LONG *);
/* First guest subclass of an observed same-process native window. Caller
 * has already distinguished native CPD from encoded guest identity. */
BOOL WINAPI wow_window_procedure_adopt_native(HWND, DWORD, DWORD *);
void WINAPI wow_window_note_native_procedure(HWND, WNDPROC, BOOL);
/* Per-task-thread post-dispatch terminal boundary, independent of which
 * native subclass owns the real WND slot. Installed before task creation. */
LRESULT CALLBACK wow_window_terminal_observer(int, WPARAM, LPARAM);
/* Caller holds USER domain lock, retains storage, and has marked the owning
 * thread INCLEANUP. Prepare every window before destroying any window. */
BOOL WINAPI wow_window_words_prepare_exit(HWND, wow_window_words_binding *);
wow_window_words_binding *WINAPI wow_window_words_acquire(HWND);
struct _WW *WINAPI wow_window_words_value(wow_window_words_binding *);
/* Immutable owning native thread identity for an enrolled/retained binding.
 * Zero means unpublished/unknown, never an invented THREADINFO address. */
DWORD WINAPI wow_window_words_thread_id(wow_window_words_binding *);
/* Copy existing view-owner metadata into unpublished sampled storage.
 * This creates no HWND property, task owner or thread registration. */
BOOL WINAPI wow_window_words_set_observed_thread_id(wow_window_words_binding *, DWORD);
/* Actual creation API provenance, not the current procedure/class encoding.
 * Unknown remains unknown. Publication is once-only (same value idempotent).
 * Both operations require an existing retained window reference. */
BOOL WINAPI wow_window_words_set_creation_ansi(wow_window_words_binding *, BOOL);
BOOL WINAPI wow_window_words_creation_ansi(wow_window_words_binding *, BOOL *);
typedef struct wow_window_creation_facts {
    DWORD module_version;
    DWORD compatibility_flags;
    int class_extra_bytes;
} wow_window_creation_facts;
/* Captured before native creation, retained with the existing HWND owner.
 * Unknown is not replaced with the current task's values during subclassing. */
BOOL WINAPI wow_window_words_set_creation_facts(wow_window_words_binding *,
    const wow_window_creation_facts *);
BOOL WINAPI wow_window_words_creation_facts(wow_window_words_binding *,
    wow_window_creation_facts *);
/* ADAPTER-WOW-063: one original-layout WND/class-extra process-heap host
 * carrier owned by this retained window. Success transfers it; failure does
 * not. It is never native USER extra bytes or a guest-visible allocation.
 * Original controls own their private cleanup before last window release. */
BOOL WINAPI wow_window_words_attach_client_state(wow_window_words_binding *,
    struct tagWND *);
/* Borrowed until the caller releases its existing window reference. */
struct tagWND *WINAPI wow_window_words_client_state_value(wow_window_words_binding *);
/* Original behavior-state capability, distinct from a resource projection
 * and live membership. It survives retirement of this exact allocation. */
BOOL WINAPI wow_window_words_owns_client_state(wow_window_words_binding *,
    struct tagWND *);
/* Build a resource-only WND from its retained committed generation. Caller
 * holds the execution owner and supplies its observed desktop identity.
 * No native resampling, enrollment, or behavior-state capability is granted.
 * Relations are not published here. Later original-state promotion preserves
 * the allocation address; successful attach consumes its temporary input. */
BOOL WINAPI wow_window_words_prepare_resource_state(wow_window_words_binding *, HWND);
/* Call-local prepared graph, never a persistent registry. All bindings and
 * relationship targets must be retained in the active borrow scope. Indices
 * are next/parent/child/owner; MAXDWORD denotes an absent edge. */
typedef struct wow_window_client_graph_row {
    wow_window_words_binding *binding;
    struct tagWND *state;
    HWND window;
    ULONG target[4];
    WINDOWINFO info;
    DWORD control_id, module;
    ATOM class_atom;
} wow_window_client_graph_row;
/* Caller holds the existing USER execution owner. Validate all rows before
 * modifying any WND; publication has no native calls or allocation. */
BOOL WINAPI wow_window_words_publish_client_graph(ULONG,
    const wow_window_client_graph_row *);
/* Retained binding required. Protects only synchronous strName readers;
 * never call USER, refresh, application code or release the binding while
 * held. NULL acquires no lock. Original GETTEXT/LENGTH consumer wiring is
 * required before selecting those bodies; a window borrow alone is not enough. */
struct tagWND *WINAPI wow_window_words_lock_client_name(wow_window_words_binding *);
void WINAPI wow_window_words_unlock_client_name(wow_window_words_binding *);
/* CBT has identified a new window. Existing-window promotion must not
 * accidentally replay class-menu creation on a later sizing message. */
void WINAPI wow_window_words_begin_creation(wow_window_words_binding *, HMENU);
/* Refresh only native WND fields aliased by WW, preserving original WOW
 * classification/procedure words. Caller retains the matching binding. */
BOOL WINAPI wow_window_words_refresh(HWND, wow_window_words_binding *);
/* Same refresh transaction, optionally returning its WINDOWINFO and numeric
 * child ID. Outputs are written only after successful publication. Original
 * control carriers reuse this sample instead of a second query. */
BOOL WINAPI wow_window_words_refresh_info(HWND, wow_window_words_binding *,
    WINDOWINFO *, DWORD *);
/* Same-process, full-HWND observation after the existing graph transaction
 * validates/commits. Reuses its rectangles/styles/numeric child ID without
 * resampling them. Module identity also comes from that committed sample;
 * zero is a valid captured value. control_id is data, not a menu pointer.
 * An unenrolled native window is a no-op; no creation or guest write. */
BOOL WINAPI wow_window_words_refresh_observed(HWND, const WINDOWINFO *,
    DWORD control_id, DWORD module);
/* Prepare all enrolled local scalar/name observations before committing the
 * existing page-domain graph. Caller holds the existing USER execution owner.
 * This is transaction scratch, not a new registry or host relationship owner. */
struct mvdm_softpc_wow_window_observation;
BOOL WINAPI wow_window_words_project_observed(ULONG,
    const struct mvdm_softpc_wow_window_observation *);
BOOL WINAPI wow_window_words_exchange(HWND, unsigned, DWORD, DWORD *);
/* Retained cleanup fields. Before original cleanup, the owner projects the
 * actual native selection and typed guest identity; this is not a dispatch
 * authority. Caller retains its window borrow and serialized membership. */
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
