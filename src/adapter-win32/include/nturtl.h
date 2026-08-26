#ifndef OPENNT_SUPPORT_NTURTL_H
#define OPENNT_SUPPORT_NTURTL_H
#include <ntrtl.h>
ULONG NTAPI RtlIsDosDeviceName_U(PWSTR DosFileName);
BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(PCWSTR DosFileName, PUNICODE_STRING NtFileName, PWSTR *FilePart, PVOID RelativeName);
#endif
