#include "precomp.h"

#ifdef WOW_ORIGINAL_THUNK_BUILD
#if !defined(WOW_USER_PRIVATE_REDIRECT) || !defined(CallNextHookEx)
#error Original WOW thunk redirects are missing.
#endif
#else
#if defined(WOW_USER_PRIVATE_REDIRECT) || defined(CallNextHookEx)
#error Original WOW thunk redirects escaped into project-owned code.
#endif
#endif
