#ifndef BX_NTVDM_BOP_SHIM_OPENNT_RTL_COMPAT_H
#define BX_NTVDM_BOP_SHIM_OPENNT_RTL_COMPAT_H

/*
 * Canonical HOST-07 declaration surface for directly imported OpenNT bodies.
 *
 * Include this only after the consuming source's OpenNT-shaped STRING and
 * UNICODE_STRING types are visible.  The types deliberately remain native
 * host-private records: no member crosses a guest ABI.  The declarations keep
 * the spelling and caller-visible allocation/return contracts used by the
 * imported COMMAND source, while their bounded public-Win32 implementation
 * remains in the existing source-shaped command shims.
 */

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* The following layout checks document the x86/x64 host boundary.  The
 * counted-string records carry native pointers and are valid only inside a
 * synchronous host facade; they must never be copied into guest memory. */
_Static_assert(sizeof(USHORT) == 2u, "OpenNT counted-string length width");
_Static_assert(offsetof(ANSI_STRING, Length) == 0u,
    "OpenNT ANSI_STRING Length offset");
_Static_assert(offsetof(ANSI_STRING, MaximumLength) == sizeof(USHORT),
    "OpenNT ANSI_STRING MaximumLength offset");
_Static_assert(offsetof(UNICODE_STRING, Length) == 0u,
    "OpenNT UNICODE_STRING Length offset");
_Static_assert(offsetof(UNICODE_STRING, MaximumLength) == sizeof(USHORT),
    "OpenNT UNICODE_STRING MaximumLength offset");

void RtlInitAnsiString(PANSI_STRING destination, const CHAR *source);
void RtlInitUnicodeString(PUNICODE_STRING destination, const WCHAR *source);
NTSTATUS RtlCreateEnvironment(BOOLEAN clone_current, PVOID *environment);
NTSTATUS RtlSetEnvironmentVariable(PVOID *environment,
    const PUNICODE_STRING name, const PUNICODE_STRING value);
void RtlDestroyEnvironment(PVOID environment);
NTSTATUS RtlAnsiStringToUnicodeString(PUNICODE_STRING destination,
    const PANSI_STRING source, BOOL allocate_destination);
NTSTATUS RtlUnicodeStringToOemString(POEM_STRING destination,
    const PUNICODE_STRING source, BOOL allocate_destination);
void RtlFreeUnicodeString(PUNICODE_STRING string);
void RtlInitString(PSTRING destination, const CHAR *source);
NTSTATUS RtlOemStringToUnicodeString(PUNICODE_STRING destination,
    const POEM_STRING source, BOOL allocate_destination);
NTSTATUS RtlUnicodeStringToAnsiString(PANSI_STRING destination,
    const PUNICODE_STRING source, BOOL allocate_destination);
void RtlFreeAnsiString(PANSI_STRING string);
ULONG RtlNtStatusToDosError(NTSTATUS status);

#if defined(__cplusplus)
}
#endif

#endif
