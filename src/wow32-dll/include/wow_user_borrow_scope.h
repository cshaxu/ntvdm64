#ifndef WOW_USER_BORROW_SCOPE_H
#define WOW_USER_BORROW_SCOPE_H
#include "wow_window_words_binding.h"

struct wow_user_borrow;
typedef struct wow_user_borrow_scope {
    struct wow_user_borrow_scope *previous;
    struct wow_user_borrow *borrows;
    wow_class_lookup_context *classes; /* Inherited; owner may bind after enter. */
} wow_user_borrow_scope;

/* ADAPTER-WOW-032: retain private WW/WC aliases for a synchronous native
 * WOW consumer scope. Enter/leave on the same thread, strictly nested, with
 * leave in finally. No alias survives its scope; no guest address is stored.
 * Class lookup still requires the caller's class-publication synchronization. */
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *);
BOOL WINAPI wow_user_borrow_leave(wow_user_borrow_scope *);
struct _WW *WINAPI wow_user_borrow_window(HWND);
DWORD *WINAPI wow_user_borrow_window_class(HWND);
DWORD *WINAPI wow_user_borrow_class(wow_class_lookup_context *, ATOM, HANDLE);
DWORD *WINAPI wow_user_borrow_named_class(HINSTANCE, const UNICODE_STRING *);
#endif
