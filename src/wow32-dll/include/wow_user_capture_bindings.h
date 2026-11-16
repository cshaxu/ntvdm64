#ifndef WOW_USER_CAPTURE_BINDINGS_H
#define WOW_USER_CAPTURE_BINDINGS_H
#include "nt.h"
#include <string.h>

/* Original usercli.h IN_STRING declaration, Copyright Microsoft Corporation.
 * ADAPTER-WOW-034: finite USER capture ABI; this is not a native TEB layout. */
typedef struct _IN_STRING {
    UNICODE_STRING strCapture;
    PUNICODE_STRING pstr;
    BOOL fAllocated;
} IN_STRING, *PIN_STRING;

/* Original user.h/usercli.h large-capture declarations (Microsoft). */
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "compat/include/opennt_user_multipass.h"
#undef WOW_ORIGINAL_LARGE_STRING_LAYOUT
typedef struct _LARGE_IN_STRING {
    LARGE_UNICODE_STRING strCapture;
    PLARGE_UNICODE_STRING pstr;
    BOOL fAllocated;
} LARGE_IN_STRING, *PLARGE_IN_STRING;
BOOL WINAPI RtlCaptureLargeAnsiString(PLARGE_IN_STRING, LPCSTR, UINT, BOOL);

/* Original ntpsapi.h constant. Reuse existing opennt_support_current_teb's
 * scratch descriptor; never read or overwrite the modern Windows TEB. */
#ifndef STATIC_UNICODE_BUFFER_LENGTH
#define STATIC_UNICODE_BUFFER_LENGTH 261
#endif
#ifndef MAXUSHORT
#define MAXUSHORT 0xffff
#endif
#define RtlAllocateHeap HeapAlloc
#define RtlFreeHeap HeapFree
#define RIPMSG0(level, text) ((void)0)
BOOL WINAPI RtlCaptureAnsiString(PIN_STRING, LPCSTR, BOOL);

/* The original body passes signed cbDst to the unsigned NTDLL byte-count
 * pointer. Preserve that x86 representation and the source body, not a cast. */
#pragma warning(disable:4057)
#endif
