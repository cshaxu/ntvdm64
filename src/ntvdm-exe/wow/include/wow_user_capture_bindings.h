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

/* Original ntpsapi.h constant. Reuse existing opennt_support_current_teb's
 * scratch descriptor; never read or overwrite the modern Windows TEB. */
#ifndef STATIC_UNICODE_BUFFER_LENGTH
#define STATIC_UNICODE_BUFFER_LENGTH 261
#endif
#ifndef MAXUSHORT
#define MAXUSHORT 0xffff
#endif
#define RtlProcessHeap() GetProcessHeap()
#define RtlAllocateHeap HeapAlloc
#define RtlFreeHeap HeapFree
#define RIPMSG0(level, text) ((void)0)
NTSYSAPI NTSTATUS NTAPI RtlMultiByteToUnicodeN(PWCH, ULONG, PULONG, PCCH, ULONG);
BOOL WINAPI RtlCaptureAnsiString(PIN_STRING, LPCSTR, BOOL);

/* The original body passes signed cbDst to the unsigned NTDLL byte-count
 * pointer. Preserve that x86 representation and the source body, not a cast. */
#pragma warning(disable:4057)
#endif
