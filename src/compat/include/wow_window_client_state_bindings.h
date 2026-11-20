#ifndef WOW_WINDOW_CLIENT_STATE_BINDINGS_H
#define WOW_WINDOW_CLIENT_STATE_BINDINGS_H
#include <windows.h>
struct tagWND;

/* ADAPTER-WOW-063. Caller owns the existing task/borrow scope. State belongs
 * to its enrolled HWND binding, not another registry or the guest projection.
 * Creation consumes captured provenance; validation never invents it. */
BOOL WINAPI wow_window_client_create_state(HWND, DWORD, DWORD, int);
BOOL WINAPI wow_window_client_ensure_state(HWND);
struct tagWND *WINAPI wow_window_client_validate_hwnd(HWND);
/* General original HWND validation, including native resource projections.
 * These do not grant control-private access. NoRip omits the invalid-handle
 * error report; adapter allocation/domain failures remain diagnosable. */
struct tagWND *WINAPI wow_window_client_validate_window(HWND);
struct tagWND *WINAPI wow_window_client_validate_window_no_rip(HWND);
struct tagWND *WINAPI wow_window_client_desktop(void);
/* Original inheritance algorithm and its exact-generation scalar input.
 * Requires the existing borrow scope/graph. Invariant failures are fatal,
 * never disguised as a legitimate zero (no locally assigned help ID). */
DWORD GetContextHelpId(struct tagWND *);
DWORD WINAPI wow_window_client_context_help(struct tagWND *);
#if defined(WOW_ORIGINAL_DEFAULT_WINDOW) || defined(WOW_ORIGINAL_EDIT_CLIENT) || defined(WOW_ORIGINAL_DIALOG_CLIENT)
#define _GetDesktopWindow wow_window_client_desktop
#endif
/* Local recovered-state domain only; rejects a copied/sampled WND as well
 * as an HWND not owned by this process. An exact already-retained allocation
 * keeps its provenance after retirement; this is not HWND validity checking
 * and does not model server privileges. */
BOOL WINAPI wow_window_client_owns_state(struct tagWND *);
/* Original xxxSetWindowLong owns private control and dialog word policy. */
LONG WINAPI wow_window_client_set_window_long(HWND, int, LONG, BOOL);
/* Public extra-word access to already recovered state. TRUE includes original
 * zero/error results; FALSE preserves last-error and means native ownership.
 * The setter also routes GWL_STYLE through the original ListBox guard.
 * Other negative indices keep their existing logical/native owners. */
BOOL WINAPI wow_window_client_try_get_long(HWND, int, BOOL, LONG *);
BOOL WINAPI wow_window_client_try_set_long(HWND, int, LONG, BOOL, LONG *);
/* Refresh public geometry/style/module only, preserving original private
 * flags, FNID, control data and the complete class-extra extent. */
BOOL WINAPI wow_window_client_refresh(HWND);
/* Original-entry graph preparation under the existing USER domain. Reuses
 * its observation transaction, retains exact generations and restores the
 * entry lock state. Requires the caller's active borrow scope. */
BOOL WINAPI wow_window_client_prepare_graph(HWND);
/* Captures native stored Unicode name without sending WM_GETTEXT. Success
 * transfers a process-heap buffer; failure leaves all outputs untouched. */
BOOL WINAPI wow_window_client_capture_name(HWND, WCHAR **, ULONG *, ULONG *);
/* Original control caller supplies a valid PWND in its borrow scope. The
 * call boundary retains that exact state before entering native dispatch,
 * rejects replacement/copied state and snapshots HWND before the callout. Modern
 * USER is the admitted sole message transport/conversion/Hook owner. */
LONG WINAPI wow_window_client_send_message(struct tagWND *, UINT, WPARAM,
    LPARAM, BOOL);
#endif
