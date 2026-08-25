#ifndef APP_OPENNT_RTL_TYPES_H
#define APP_OPENNT_RTL_TYPES_H

/* Exact host-private OpenNT counted-string ABI.  These records may contain
 * native pointers and are never guest-visible or asynchronous payloads. */

#include <windows.h>

typedef LONG NTSTATUS;

typedef struct _ANSI_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} ANSI_STRING, *PANSI_STRING;
typedef ANSI_STRING OEM_STRING, *POEM_STRING;
typedef ANSI_STRING STRING, *PSTRING;

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef const OEM_STRING *PCOEM_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

#endif
