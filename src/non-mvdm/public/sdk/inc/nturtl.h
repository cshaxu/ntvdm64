/* DIVERGENCE OPENNT-HOST-007: source-identified true subset of original
 * public/sdk/inc/nturtl.h.  Only MVDM-reached declaration shapes are retained;
 * no path, process, or filesystem behavior is implemented in this mirror. */
#line 28
#ifndef _NTURTL_
#define _NTURTL_
#include <ntrtl.h>
#line 257
ULONG NTAPI RtlIsDosDeviceName_U(PWSTR DosFileName);
#line 294
BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(PCWSTR DosFileName, PUNICODE_STRING NtFileName, PWSTR *FilePart, PVOID RelativeName);
#line 1103
#endif
