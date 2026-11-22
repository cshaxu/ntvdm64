#ifndef COMPAT_WOW_HARD_ERROR_H
#define COMPAT_WOW_HARD_ERROR_H

#include <windows.h>

typedef LONG (WINAPI wow_hard_error_fn)(LONG, ULONG, ULONG, ULONG_PTR *, ULONG, ULONG *);
BOOL __cdecl wow_hard_error_bind(wow_hard_error_fn *provider);
wow_hard_error_fn wow_raise_hard_error;

#endif
