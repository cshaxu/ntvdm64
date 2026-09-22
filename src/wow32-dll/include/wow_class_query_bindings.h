#ifndef WOW_CLASS_QUERY_BINDINGS_H
#define WOW_CLASS_QUERY_BINDINGS_H
#include "wow_user_capture_bindings.h"
#include "wow_user_borrow_scope.h"

LONG WINAPI GetClassWOWWords(HINSTANCE, LPCSTR);

/* ADAPTER-WOW-035: the original query consumes only GCL_WOWWORDS. Its PCLS
 * is a scope-retained native WC view, not an NT4 CLS or a guest pointer.
 * Original classc.c::_GetClassData returns pcls->adwWOW for this one index;
 * map that fixed expression to the existing two-word storage. */
#ifdef WOW_ORIGINAL_CLASS_QUERY
typedef DWORD *PCLS;
static const struct { ULONG ulClientDelta; } wow_class_query_client = {0};
#define GetClientInfo() (&wow_class_query_client)
#define NtUserGetWOWClass wow_user_borrow_named_class
#define GCL_WOWWORDS (-27)
#define _GetClassData(pcls, pwnd, index, ansi) ((DWORD)(pcls))
#define RIPERR0(code, level, text) SetLastError(code)
#endif
#endif
