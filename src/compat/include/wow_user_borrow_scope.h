#ifndef WOW_USER_BORROW_SCOPE_H
#define WOW_USER_BORROW_SCOPE_H
#include "wow_window_words_binding.h"

struct wow_user_borrow;
typedef struct wow_user_borrow_scope {
    struct wow_user_borrow_scope *previous;
    struct wow_user_borrow *borrows;
    wow_class_lookup_context *classes; /* Inherited; owner may bind after enter. */
    BOOL callback_published;
    ULONG saved_callback[2]; /* Saved pair only; page-domain owns current identity. */
} wow_user_borrow_scope;

/* ADAPTER-WOW-032: retain private WW/WC aliases for a synchronous native
 * WOW consumer scope. Enter/leave on the same thread, strictly nested, with
 * leave in finally. No alias survives its scope; no guest address is stored.
 * Class lookup still requires the caller's class-publication synchronization. */
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *);
/* Publish an actual window entry once in this scope; leave restores the
 * existing page-domain callback pair before releasing any retained objects.
 * Ordinary temporary borrow scopes do not change callback identity. */
BOOL WINAPI wow_user_borrow_set_callback(HWND);
/* A valid leave always consumes the scope. FALSE can also report deferred
 * physical reclamation failure; retry collection, never leave that scope twice. */
BOOL WINAPI wow_user_borrow_leave(wow_user_borrow_scope *);
/* Production finally: require a matching pop, but allow the existing
 * collector to retain deferred physical frees. Preserve callback last-error. */
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *);
/* Borrowed from the current nested scopes, not a transferred reference.
 * Enrollment must retain it before exposing a native property. */
wow_window_words_binding *WINAPI wow_user_borrow_observed_binding(HWND);
/* Existing enrolled identity retained by the current scope, without creating
 * sampled storage or another HWND map. NULL outside a live borrow scope. */
wow_window_words_binding *WINAPI wow_user_borrow_enrolled_window(HWND);
/* Match an exact retained host WND, including an already retired borrow.
 * A newly acquired HWND binding must own that same allocation. */
wow_window_words_binding *WINAPI wow_user_borrow_client_state(HWND, struct tagWND *);
struct _WW *WINAPI wow_user_borrow_window(HWND);
/* General original WND identity, not permission to use private control words.
 * Callback pair first, then current committed handle generation, including
 * original short-handle uniqueness forms. Requires USER execution ownership.
 * Retains the complete reachable graph; never samples native HWND state. */
struct tagWND *WINAPI wow_user_borrow_window_state(HWND);
/* Retain the exact committed relationship target in this scope, including
 * historical generations. Success with NULL means no edge. The returned
 * binding is borrowed, not caller-owned; no enrollment or native refresh.
 * Relation ordering is next/parent/child/owner (0..3). */
BOOL WINAPI wow_user_borrow_window_relation(wow_window_words_binding *, ULONG,
    wow_window_words_binding **);
/* Prepare and publish the full reachable host WND graph from committed exact
 * generations. All visited nodes remain retained until the outermost active
 * scope leaves, including targets introduced by a nested refresh.
 * Caller owns USER execution and supplies the observed desktop identity.
 * NULL root refreshes all suspended original roots in one transaction;
 * without an active scope/original state that form is a successful no-op. */
BOOL WINAPI wow_user_borrow_client_graph(wow_window_words_binding *, HWND);
BOOL WINAPI wow_user_borrow_window_exchange(HWND, unsigned, DWORD, DWORD *);
DWORD *WINAPI wow_user_borrow_window_class(HWND);
DWORD *WINAPI wow_user_borrow_class(wow_class_lookup_context *, ATOM, HANDLE);
DWORD *WINAPI wow_user_borrow_named_class(HINSTANCE, const UNICODE_STRING *);
#endif
