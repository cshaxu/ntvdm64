#ifndef OPENNT_ABI_NTRTL_WOW32_H
#define OPENNT_ABI_NTRTL_WOW32_H

/* Selected source-used declarations from OpenNT public/sdk/inc/ntrtl.h
 * (SHA-256 2FC698D4E078ABF7B18722F3E84712AB580BF22CF3362BA3CE9FB9486B1A7E98). */
NTSYSAPI
NTSTATUS
NTAPI
RtlMultiByteToUnicodeN(
    PWSTR UnicodeString,
    ULONG MaxBytesInUnicodeString,
    PULONG BytesInUnicodeString,
    PCHAR MultiByteString,
    ULONG BytesInMultiByteString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteN(
    PCHAR MultiByteString,
    ULONG MaxBytesInMultiByteString,
    PULONG BytesInMultiByteString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAdjustPrivilege(
    ULONG Privilege,
    BOOLEAN Enable,
    BOOLEAN Client,
    PBOOLEAN WasEnabled
    );

NTSYSAPI
PIMAGE_NT_HEADERS
NTAPI
RtlImageNtHeader(PVOID Base);

#endif /* OPENNT_ABI_NTRTL_WOW32_H */
