/* DIVERGENCE OPENNT-HOST-007: source-identified true subset of original
 * public/sdk/inc/nturtl.h.  Only MVDM-reached declaration shapes are retained;
 * no path, process, or filesystem behavior is implemented in this mirror. */
#ifndef OPENNT_HOST_NTURTL_SUBSET_H
#define OPENNT_HOST_NTURTL_SUBSET_H
#include <ntrtl.h>
ULONG NTAPI RtlIsDosDeviceName_U(PWSTR DosFileName);
BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(PCWSTR DosFileName, PUNICODE_STRING NtFileName, PWSTR *FilePart, PVOID RelativeName);
#endif
