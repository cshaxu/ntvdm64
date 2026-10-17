#ifndef WOW_USER_REGISTERED_CALLBACKS_H
#define WOW_USER_REGISTERED_CALLBACKS_H
#include <windows.h>

struct tagAPFNWOWHANDLERSIN;
/* Borrow the existing UserRegisterWowHandlers input, never a second table.
 * Original consumers must run after registration and in their existing WOW
 * execution scope. The view is process-local and must not enter guest state. */
const struct tagAPFNWOWHANDLERSIN *WINAPI wow_user_registered_callbacks(void);
#endif
