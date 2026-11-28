#ifndef WOW_USER_CAPTURE_BINDINGS_H
#define WOW_USER_CAPTURE_BINDINGS_H
#include <windows.h>
#include <string.h>

struct _IN_STRING;
struct _LARGE_IN_STRING;

BOOL WINAPI RtlCaptureLargeAnsiString(struct _LARGE_IN_STRING *, LPCSTR, BOOL);

/* Original ntpsapi.h constant. Reuse existing opennt_support_current_teb's
 * scratch descriptor; never read or overwrite the modern Windows TEB. */
#ifndef STATIC_UNICODE_BUFFER_LENGTH
#define STATIC_UNICODE_BUFFER_LENGTH 261
#endif
#ifndef MAXUSHORT
#define MAXUSHORT 0xffff
#endif
/* XP w32wow64.h: x86 host, not BUILD_WOW6432. */
#define KPWSTR_TO_PWSTR(p) (p)
#ifndef KPVOID_TO_PVOID
#define KPVOID_TO_PVOID(p) (p)
#endif
/* XP user.h:467, pointer versus integer resource in the selected host ABI. */
#ifndef IS_PTR
#define IS_PTR(p) ((((ULONG_PTR)(p)) & ~MAXUSHORT) != 0)
#endif
#define RtlAllocateHeap HeapAlloc
#define RtlFreeHeap HeapFree
#define RIPMSG0(level, text) ((void)0)
BOOL WINAPI RtlCaptureAnsiString(struct _IN_STRING *, LPCSTR, BOOL);

/* The original body passes signed cbDst to the unsigned NTDLL byte-count
 * pointer. Preserve that x86 representation and the source body, not a cast. */
#pragma warning(disable:4057)
#endif
