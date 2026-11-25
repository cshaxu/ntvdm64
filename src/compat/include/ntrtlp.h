#ifndef OPENNT_HOST_NTOS_RTL_NTRTLP_H
#define OPENNT_HOST_NTOS_RTL_NTRTLP_H

/* The selected source body is original.  This header supplies only the
 * established standalone declaration boundary for its finite user-mode
 * dependencies; it supplies no environment algorithm. */
#include <opennt_support_types.h>

/* `winternl.h` exposes a reduced modern PEB layout.  The original body
 * reaches only these adapter-private historical fields, so bind its source
 * spelling to the established surrogate rather than casting the host PEB. */
#define PPEB POPENNT_SUPPORT_PEB
/* error.c needs the original per-thread LastStatusValue carrier. */
#define PTEB POPENNT_SUPPORT_TEB

#ifndef STATUS_NOT_FOUND
#define STATUS_NOT_FOUND ((NTSTATUS)0xC0000225L)
#endif

/* XP ntrtl.h declarations used by the original environment owner. */
typedef struct _RTL_UNICODE_STRING_BUFFER *PRTL_UNICODE_STRING_BUFFER;
#ifndef RTL_CONSTANT_STRING
#define RTL_CONSTANT_STRING(s) { sizeof(s)-sizeof((s)[0]), sizeof(s), (PWSTR)(s) }
#endif
#define RTL_FIND_CHAR_IN_UNICODE_STRING_START_AT_END 0x00000001
NTSYSAPI NTSTATUS NTAPI RtlFindCharInUnicodeString(ULONG, PCUNICODE_STRING, PCUNICODE_STRING, USHORT *);
VOID NTAPI RtlSetLastWin32Error(LONG);
#endif
