#ifndef WOW_USER_THUNK_SCOPE_H
#define WOW_USER_THUNK_SCOPE_H
#include "wow_thunk_provider.h"
/* Opaque pointers preserve selected original x86 /Gz ABI without layouts. */
ULONG WINAPI wow_user_invoke_thunk(wow_thunk_procedure, struct _VDMFRAME *);
/* ADAPTER-WOW-099: original policy, known original API return identity. */
INT WINAPI wow_user_dispatch_exception(DWORD, PEXCEPTION_POINTERS, struct _TD *,
    DWORD, BOOL);
/* Original service contract; the worker remains presentation owner. */
ULONG __cdecl wow_user_sys_error_box(LPSTR, LPSTR, USHORT, USHORT, USHORT);
#endif
