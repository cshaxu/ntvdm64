#ifndef WOW_IDLE_HOOK_BINDING_H
#define WOW_IDLE_HOOK_BINDING_H

#include <windows.h>

/* Borrow original TD registration state; no second Hook registry. */
BOOL WINAPI wow_user_idle_hook_registered(void);
void WINAPI wow_user_idle_hook_dispatch(void);
LRESULT CALLBACK WK32ForegroundIdleHook(int, WPARAM, LPARAM);
/* Borrowed native observer. Original TD owns intent, not observer lifetime. */
HHOOK WINAPI wow_user_idle_hook_for_current(void);

#endif
