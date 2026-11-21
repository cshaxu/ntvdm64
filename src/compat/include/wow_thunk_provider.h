#ifndef WOW_THUNK_PROVIDER_H
#define WOW_THUNK_PROVIDER_H
#include <windows.h>
struct _TD;
struct _VDMFRAME;
typedef ULONG (WINAPI *wow_thunk_procedure)(struct _VDMFRAME *);
typedef struct wow_thunk_mode { BOOL who_called, thunk_dde; } wow_thunk_mode;
typedef struct wow_thunk_frame { WORD task, ax, args; DWORD call, return_ip; } wow_thunk_frame;
typedef struct wow_thunk_provider {
    INT (WINAPI *exception)(DWORD, PEXCEPTION_POINTERS, struct _TD *, DWORD, BOOL);
    VOID (WINAPI *capture)(wow_thunk_mode *);
    VOID (WINAPI *restore)(const wow_thunk_mode *);
    VOID (WINAPI *frame)(struct _VDMFRAME *, wow_thunk_frame *);
} wow_thunk_provider;
BOOL WINAPI wow_thunk_bind(const wow_thunk_provider *);
#endif
