#ifndef WOW_HOOK_CALLBACK_H
#define WOW_HOOK_CALLBACK_H
#include <windows.h>
typedef struct wow_hook_context {
    INT code;
    LONG wp, lp;
    INT index;
    BOOL dde, who;
} wow_hook_context;
typedef struct wow_hook_callbacks {
    void (WINAPI *capture)(wow_hook_context *);
    void (WINAPI *restore)(const wow_hook_context *);
    BOOL (WINAPI *type)(INT, INT *);
    LONG (WINAPI *invoke)(INT, LONG, LONG, INT);
} wow_hook_callbacks;
/* Bind an immutable same-image provider before original DLL attach. */
BOOL WINAPI wow_hook_bind(const wow_hook_callbacks *provider);
#endif
