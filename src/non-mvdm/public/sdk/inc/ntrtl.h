/*++ BUILD Version: 0005    // Increment this if a change has global effects

Copyright (c) 1989-2000  Microsoft Corporation
Copyright (c) 2015  OpenNT Project

Module Name:

    ntrtl.h

Abstract:

    Include file for NT runtime routines that are callable by both
    kernel mode code in the executive and user mode code in various
    NT subsystems.

Author:

    Steve Wood (stevewo) 31-Mar-1989

Environment:

    These routines are statically linked in the caller's executable and
    are callable in either kernel mode or user mode.

Revision History:

    30-Apr-2015     stephanos   Merged the changes made in the NT 5 implementation

--*/

#line 31
#ifndef _NTRTL_
#define _NTRTL_

#include <nt.h>

#line 56
#ifdef __cplusplus
#line 57
#error Unsupported C++ inclusion of the NT4 RTL subset
#endif
#line 59

PVOID NTAPI RtlProcessHeap(VOID);
PVOID NTAPI RtlAllocateHeap(PVOID HeapHandle, ULONG Flags, SIZE_T Size);
BOOLEAN NTAPI RtlFreeHeap(PVOID HeapHandle, ULONG Flags, PVOID BaseAddress);
VOID NTAPI RtlInitString(PSTRING DestinationString, PCSZ SourceString);
VOID NTAPI RtlInitAnsiString(PANSI_STRING DestinationString, PCSZ SourceString);
VOID NTAPI RtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString);
NTSTATUS NTAPI RtlOemStringToUnicodeString(PUNICODE_STRING DestinationString, PCOEM_STRING SourceString, BOOLEAN AllocateDestinationString);
/* DIVERGENCE(OPENNT-HOST-011): selected SoftPC keyboard conversion reaches
 * the original fixed-buffer RTL declaration.  Keep its original public
 * source shape in this true subset; the modern API binding is adapter-owned. */
NTSTATUS NTAPI RtlOemToUnicodeN(PWSTR UnicodeString, ULONG MaxBytesInUnicodeString, PULONG BytesInUnicodeString, PCHAR OemString, ULONG BytesInOemString);
NTSTATUS NTAPI RtlUnicodeStringToOemString(POEM_STRING DestinationString, PCUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString);
/* DIVERGENCE(OPENNT-HOST-012): the selected byte-identical NetLib CopyStr
 * unit reaches these two original OEM conversion declarations.  The full
 * NT4 NTRTL header is outside the accepted host closure; retain the reached
 * signatures only.  The selected x86 NTDLL/import-library binding is verified
 * by the Redirector network package. */
NTSTATUS NTAPI RtlUnicodeToOemN(PCHAR OemString, ULONG MaxBytesInOemString, PULONG BytesInOemString, PWSTR UnicodeString, ULONG BytesInUnicodeString);
ULONG NTAPI RtlUnicodeStringToOemSize(PCUNICODE_STRING UnicodeString);
NTSTATUS NTAPI RtlAnsiStringToUnicodeString(PUNICODE_STRING DestinationString, PCANSI_STRING SourceString, BOOLEAN AllocateDestinationString);
NTSTATUS NTAPI RtlUnicodeStringToAnsiString(PANSI_STRING DestinationString, PCUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString);
VOID NTAPI RtlFreeUnicodeString(PUNICODE_STRING UnicodeString);
VOID NTAPI RtlFreeOemString(POEM_STRING OemString);
VOID NTAPI RtlFreeAnsiString(PANSI_STRING AnsiString);
BOOLEAN NTAPI RtlEqualUnicodeString(PCUNICODE_STRING String1, PCUNICODE_STRING String2, BOOLEAN CaseInSensitive);
ULONG NTAPI RtlOemStringToUnicodeSize(PCOEM_STRING String);
ULONG NTAPI RtlGetFullPathName_U(PCWSTR FileName, ULONG Size, PWSTR Buffer, PWSTR *ShortName);
ULONG NTAPI RtlGetCurrentDirectory_U(ULONG MaximumLength, PWSTR Buffer);
NTSTATUS NTAPI RtlSetCurrentDirectory_U(PCUNICODE_STRING PathName);
/* DIVERGENCE(OPENNT-HOST-027): modern NTDLL exports these historical names.
 * The selected original error.c implementation therefore lives in an
 * explicit private symbol domain.  All selected callers use that original
 * table; this header supplies no project-defined mapping policy. */
#define RtlNtStatusToDosError OpenNtRtlNtStatusToDosError
#define RtlNtStatusToDosErrorNoTeb OpenNtRtlNtStatusToDosErrorNoTeb
ULONG NTAPI RtlNtStatusToDosError(NTSTATUS Status);
ULONG NTAPI RtlNtStatusToDosErrorNoTeb(NTSTATUS Status);
VOID NTAPI RtlAcquirePebLock(VOID);
VOID NTAPI RtlReleasePebLock(VOID);
/* DIVERGENCE(OPENNT-HOST-010): original COMMAND environment code reaches
 * these NT4 RTL declarations, while the selected subset previously stopped
 * short of them.  Preserve only the original public function shapes; the
 * modern mutable-block implementation is adapter-owned. */
NTSTATUS NTAPI RtlCreateEnvironment(BOOLEAN CloneCurrentEnvironment, PVOID *Environment);
NTSTATUS NTAPI RtlDestroyEnvironment(PVOID Environment);
NTSTATUS NTAPI RtlSetEnvironmentVariable(PVOID *Environment, PCUNICODE_STRING Name, PCUNICODE_STRING Value);

/* DIVERGENCE OPENNT-HOST-004: `nt_bop.c` reaches the exact historical
 * ntrtl.h declaration for this diagnostic-only breakpoint.  Modern SDK
 * selection through nt.h does not expose it consistently, so retain only its
 * original declaration shape here; debugger behavior remains owned by the
 * debugger adapter family. */
VOID NTAPI DbgBreakPoint(VOID);

BOOLEAN NTAPI RtlCreateUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString);
LONG NTAPI RtlCompareUnicodeString(PCUNICODE_STRING String1, PCUNICODE_STRING String2, BOOLEAN CaseInSensitive);
NTSTATUS NTAPI RtlAppendUnicodeToString(PUNICODE_STRING Destination, PCWSTR Source);
NTSTATUS NTAPI RtlUpcaseUnicodeStringToCountedOemString(POEM_STRING DestinationString, PCUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString);

/* DIVERGENCE OPENNT-HOST-005: Original SoftPC CMOS code includes ntrtl.h
 * for this NT4 time-conversion declaration shape.  The selected modern SDK
 * route intentionally supplies only a small historical subset, so retain the
 * unchanged data layout and public-NTDLL declarations here rather than alter
 * the original CMOS source. */
#ifndef OPENNT_SUPPORT_TIME_FIELDS_DECLARED
#define OPENNT_SUPPORT_TIME_FIELDS_DECLARED
typedef SHORT CSHORT;
typedef struct _TIME_FIELDS {
    CSHORT Year;
    CSHORT Month;
    CSHORT Day;
    CSHORT Hour;
    CSHORT Minute;
    CSHORT Second;
    CSHORT Milliseconds;
    CSHORT Weekday;
} TIME_FIELDS, *PTIME_FIELDS;

VOID NTAPI RtlTimeToTimeFields(PLARGE_INTEGER Time, PTIME_FIELDS TimeFields);
BOOLEAN NTAPI RtlTimeFieldsToTime(PTIME_FIELDS TimeFields, PLARGE_INTEGER Time);
#endif

/* DIVERGENCE OPENNT-HOST-006: Original SoftPC timer and heartbeat code
 * reaches these NT4 RTL arithmetic, critical-section, and process-control
 * declaration forms.  The modern public SDK does not expose a coherent
 * source-compatible subset through the selected headers.  Retain their
 * original signatures only; NTDLL remains the implementation owner. */
LARGE_INTEGER NTAPI RtlExtendedLargeIntegerDivide(
    LARGE_INTEGER Dividend,
    ULONG Divisor,
    PULONG Remainder);
LARGE_INTEGER NTAPI RtlExtendedIntegerMultiply(
    LARGE_INTEGER Multiplicand,
    LONG Multiplier);
VOID NTAPI RtlFillMemoryUlong(
    PVOID Destination,
    SIZE_T Length,
    ULONG Pattern);
NTSTATUS NTAPI RtlEnterCriticalSection(PRTL_CRITICAL_SECTION CriticalSection);
NTSTATUS NTAPI RtlLeaveCriticalSection(PRTL_CRITICAL_SECTION CriticalSection);
NTSTATUS NTAPI RtlInitializeCriticalSection(PRTL_CRITICAL_SECTION CriticalSection);
NTSTATUS NTAPI NtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);
#ifndef NtCurrentProcess
#define NtCurrentProcess() ( (HANDLE) -1 )
#endif

#ifndef InitializeListHead
#define InitializeListHead(ListHead) ((ListHead)->Flink = (ListHead)->Blink = (ListHead))
#define IsListEmpty(ListHead) ((ListHead)->Flink == (ListHead))
#define RemoveEntryList(Entry) { PLIST_ENTRY _blink = (Entry)->Blink; PLIST_ENTRY _flink = (Entry)->Flink; _blink->Flink = _flink; _flink->Blink = _blink; }
#define InsertHeadList(ListHead,Entry) { PLIST_ENTRY _flink = (ListHead)->Flink; (Entry)->Flink = _flink; (Entry)->Blink = (ListHead); _flink->Blink = (Entry); (ListHead)->Flink = (Entry); }
#endif

/* DIVERGENCE(OPENNT-HOST-029): original header inline required by BaseSrv's
 * IsClientSystem; retain its body rather than generating a test-only copy. */
#line 4938
#if !defined(MIDL_PASS)

#line 4940
__inline LUID
NTAPI
RtlConvertLongToLuid(
    LONG Long
    )
{
    LUID TempLuid;
    LARGE_INTEGER TempLi;

    TempLi.QuadPart = Long;
    TempLuid.LowPart = TempLi.LowPart;
    TempLuid.HighPart = TempLi.HighPart;
    
    return TempLuid;
}
#line 4969
#endif
#line 4970

#line 34
#if defined (_MSC_VER)
#line 35
#line 35
#if ( _MSC_VER >= 800 )
#line 36
#line 37
#if _MSC_VER >= 1200
#line 38
#line 39
#endif
#line 40
#line 43
#endif
#line 44
#line 44
#if (_MSC_VER > 1020)
#line 45
#line 46
#endif
#line 47
#endif
#line 48
#line 67
#if DBG
#line 68
#error Excluded unsupported original NT RTL configuration
#line 86
#else
#line 87
#line 89
#endif
#line 90
#line 1032
#if defined(_M_MRX000) || defined(_M_ALPHA)
#line 1033
#error Excluded unsupported original NT RTL configuration
#line 1045
#else
#line 1046
#line 1055
#endif
#line 1056
#line 1575
#ifdef _WIN64
#line 1576
#line 1577
#else
#line 1578
#line 1579
#endif
#line 1580
#line 1594
#ifdef _NTSYSTEM_
#line 1595
#error T476-S1: branch not selected by product build
#line 1599
#else
#line 1600
#line 1604
#endif
#line 1605
#line 2234
#ifndef DEFINE_GUIDEX
#line 2235
#line 2236
#endif
#line 2237
#line 2238
#ifndef STATICGUIDOF
#line 2239
#line 2240
#endif
#line 2241
#line 2242
#ifndef __IID_ALIGNED__
#line 2243
#line 2244
#ifdef __cplusplus
#line 2245
#error Excluded unsupported original NT RTL configuration
#line 2249
#else // !__cplusplus
#line 2250
#line 2252
#endif
#line 2253
#endif
#line 2254
#line 2814
#if defined(_WIN64)
#line 2815
#line 2862
#endif
#line 2863
#line 2864
#if defined(_X86_)
#line 2865
#line 2912
#endif
#line 2913
#line 3052
#if !defined(MIDL_PASS)
#line 3053
#line 3078
#ifndef _MAC
#line 3079
#line 3093
#endif
#line 3094
#line 3096
#endif
#line 3097
#line 3126
#if defined(_WIN64)
#line 3127
#line 3135
#else
#line 3136
#line 3137
#endif
#line 3138
#line 3164
#if defined(_M_IX86) || defined(_M_MRX000) || defined(_M_ALPHA)
#line 3165
#line 3166
#if defined(_M_MRX000)
#line 3167
#error Excluded unsupported original NT RTL configuration
#line 3176
#else
#line 3177
#line 3178
#endif
#line 3179
#line 3185
#else // _M_PPC
#line 3186
#line 3239
#endif
#line 3240
#line 3251
#if defined(_M_ALPHA)
#line 3252
#error Excluded unsupported original NT RTL configuration
#line 3291
#else
#line 3292
#line 3297
#endif
#line 3298
#line 3422
#if DBG
#line 3423
#error Excluded unsupported original NT RTL configuration
#line 3428
#else
#line 3429
#line 3434
#endif
#line 3435
#line 3436
#ifndef _DBGNT_
#line 3437
#line 3451
#endif
#line 3452
#line 3486
#if defined(MIDL_PASS) || defined(__cplusplus) || !defined(_M_IX86)
#line 3487
#line 3561
#else
#line 3562
#line 3683
#endif
#line 3684
#line 3751
#if defined(MIDL_PASS) || defined(__cplusplus) || !defined(_M_IX86)
#line 3752
#line 3804
#else
#line 3805
#line 3932
#endif
#line 3933
#line 5691
#if defined(_WIN64)
#line 5692
#line 5693
#endif
#line 5694
#line 6166
#ifdef UNICODE
#line 6167
#line 6170
#else
#line 6171
#line 6174
#endif
#line 6175
#line 6203
#ifdef UNICODE
#line 6204
#line 6207
#else
#line 6208
#line 6211
#endif
#line 6212
#line 6337
#if defined (_MSC_VER) && ( _MSC_VER >= 800 )
#line 6338
#line 6338
#if _MSC_VER >= 1200
#line 6339
#line 6340
#else
#line 6341
#error Excluded unsupported original NT RTL configuration
#line 6344
#endif
#line 6345
#endif
#line 6346
#line 6333
#ifdef __cplusplus
#line 6334
#error Unsupported C++ inclusion of the NT4 RTL subset
#endif
#line 6336

#line 6347
#endif  // _NTRTL_
