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

ULONG NTAPI RtlUnicodeStringToOemSize(PCUNICODE_STRING UnicodeString);

ULONG NTAPI RtlOemStringToUnicodeSize(PCOEM_STRING String);
ULONG NTAPI RtlGetFullPathName_U(PCWSTR FileName, ULONG Size, PWSTR Buffer, PWSTR *ShortName);
ULONG NTAPI RtlGetCurrentDirectory_U(ULONG MaximumLength, PWSTR Buffer);
NTSTATUS NTAPI RtlSetCurrentDirectory_U(PCUNICODE_STRING PathName);
#define RtlNtStatusToDosError OpenNtRtlNtStatusToDosError
#define RtlNtStatusToDosErrorNoTeb OpenNtRtlNtStatusToDosErrorNoTeb

NTSTATUS NTAPI RtlEnterCriticalSection(PRTL_CRITICAL_SECTION CriticalSection);
NTSTATUS NTAPI RtlLeaveCriticalSection(PRTL_CRITICAL_SECTION CriticalSection);
NTSTATUS NTAPI RtlInitializeCriticalSection(PRTL_CRITICAL_SECTION CriticalSection);
NTSTATUS NTAPI NtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);
#ifndef NtCurrentProcess
#define NtCurrentProcess() ( (HANDLE) -1 )
#endif

#ifndef InitializeListHead
#line 106
#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead))
#endif

#ifndef IsListEmpty
#line 116
#define IsListEmpty(ListHead) \
    ((ListHead)->Flink == (ListHead))
#endif

#ifndef RemoveHeadList
#line 126
#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {RemoveEntryList((ListHead)->Flink)}
#endif

#ifndef RemoveEntryList
#line 148
#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_Flink;\
    _EX_Flink = (Entry)->Flink;\
    _EX_Blink = (Entry)->Blink;\
    _EX_Blink->Flink = _EX_Flink;\
    _EX_Flink->Blink = _EX_Blink;\
    }
#endif

#ifndef InsertTailList
#line 165
#define InsertTailList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Blink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Blink = _EX_ListHead->Blink;\
    (Entry)->Flink = _EX_ListHead;\
    (Entry)->Blink = _EX_Blink;\
    _EX_Blink->Flink = (Entry);\
    _EX_ListHead->Blink = (Entry);\
    }
#endif

#ifndef InsertHeadList
#line 184
#define InsertHeadList(ListHead,Entry) {\
    PLIST_ENTRY _EX_Flink;\
    PLIST_ENTRY _EX_ListHead;\
    _EX_ListHead = (ListHead);\
    _EX_Flink = _EX_ListHead->Flink;\
    (Entry)->Flink = _EX_Flink;\
    (Entry)->Blink = _EX_ListHead;\
    _EX_Flink->Blink = (Entry);\
    _EX_ListHead->Flink = (Entry);\
    }
#endif

#line 937
PVOID
NTAPI
RtlAllocateHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN SIZE_T Size
    );

#line 946
BOOLEAN
NTAPI
RtlFreeHeap(
    IN PVOID HeapHandle,
    IN ULONG Flags,
    IN PVOID BaseAddress
    );

#line 1610
VOID
NTAPI
RtlInitString(
    PSTRING DestinationString,
    PCSZ SourceString
    );

#line 1618
VOID
NTAPI
RtlInitAnsiString(
    PANSI_STRING DestinationString,
    PCSZ SourceString
    );

#line 1626
VOID
NTAPI
RtlInitUnicodeString(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    );

#line 1636
BOOLEAN
NTAPI
RtlCreateUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString
    );

#line 1757
NTSTATUS
NTAPI
RtlAnsiStringToUnicodeString(
    PUNICODE_STRING DestinationString,
    PCANSI_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

#line 1777
NTSTATUS
NTAPI
RtlUnicodeStringToAnsiString(
    PANSI_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

#line 1799
NTSTATUS
NTAPI
RtlOemStringToUnicodeString(
    PUNICODE_STRING DestinationString,
    PCOEM_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

#line 1808
NTSTATUS
NTAPI
RtlUnicodeStringToOemString(
    POEM_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

#line 1844
NTSTATUS
NTAPI
RtlUpcaseUnicodeStringToCountedOemString(
    POEM_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

#line 1855
LONG
NTAPI
RtlCompareUnicodeString(
    PCUNICODE_STRING String1,
    PCUNICODE_STRING String2,
    BOOLEAN CaseInSensitive
    );

#line 1864
BOOLEAN
NTAPI
RtlEqualUnicodeString(
    IN PCUNICODE_STRING String1,
    IN PCUNICODE_STRING String2,
    IN BOOLEAN CaseInSensitive
    );

#line 1922
NTSTATUS
NTAPI
RtlAppendUnicodeToString (
    IN PUNICODE_STRING Destination,
    IN PCWSTR Source
    );

#line 1941
VOID
NTAPI
RtlFreeUnicodeString(
    PUNICODE_STRING UnicodeString
    );

#line 1948
VOID
NTAPI
RtlFreeAnsiString(
    PANSI_STRING AnsiString
    );

#line 1957
VOID
NTAPI
RtlFreeOemString(
    POEM_STRING OemString
    );

#line 2131
NTSTATUS
NTAPI
RtlOemToUnicodeN(
    PWSTR UnicodeString,
    ULONG MaxBytesInUnicodeString,
    PULONG BytesInUnicodeString,
    PCHAR OemString,
    ULONG BytesInOemString
    );

#line 2142
NTSTATUS
NTAPI
RtlUnicodeToOemN(
    PCHAR OemString,
    ULONG MaxBytesInOemString,
    PULONG BytesInOemString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
    );

#line 2366
VOID
NTAPI
RtlAcquirePebLock(
    VOID
    );

#line 2373
VOID
NTAPI
RtlReleasePebLock(
    VOID
    );

#line 2400
NTSTATUS
NTAPI
RtlCreateEnvironment(
    BOOLEAN CloneCurrentEnvironment,
    PVOID *Environment
    );

#line 2408
NTSTATUS
NTAPI
RtlDestroyEnvironment(
    PVOID Environment
    );

#line 2423
NTSTATUS
NTAPI
RtlSetEnvironmentVariable(
    PVOID *Environment,
    PCUNICODE_STRING Name,
    PCUNICODE_STRING Value
    );

#line 3311
VOID
NTAPI
RtlFillMemoryUlong (
   PVOID Destination,
   SIZE_T Length,
   ULONG Pattern
   );

#line 3401
VOID
NTAPI
DbgBreakPoint(
    VOID
    );

#line 3704
LARGE_INTEGER
NTAPI
RtlExtendedLargeIntegerDivide (
    LARGE_INTEGER Dividend,
    ULONG Divisor,
    PULONG Remainder
    );

#line 3730
LARGE_INTEGER
NTAPI
RtlExtendedIntegerMultiply (
    LARGE_INTEGER Multiplicand,
    LONG Multiplier
    );

#line 4061
typedef struct _TIME_FIELDS {
    CSHORT Year;        // range [1601...]
    CSHORT Month;       // range [1..12]
    CSHORT Day;         // range [1..31]
    CSHORT Hour;        // range [0..23]
    CSHORT Minute;      // range [0..59]
    CSHORT Second;      // range [0..59]
    CSHORT Milliseconds;// range [0..999]
    CSHORT Weekday;     // range [0..6] == [Sunday..Saturday]
} TIME_FIELDS;
typedef TIME_FIELDS *PTIME_FIELDS;

#line 4116
VOID
NTAPI
RtlTimeToTimeFields (
    PLARGE_INTEGER Time,
    PTIME_FIELDS TimeFields
    );

#line 4128
BOOLEAN
NTAPI
RtlTimeFieldsToTime (
    PTIME_FIELDS TimeFields,
    PLARGE_INTEGER Time
    );

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

#line 5773
ULONG
NTAPI
RtlNtStatusToDosError (
   NTSTATUS Status
   );

#line 5780
ULONG
NTAPI
RtlNtStatusToDosErrorNoTeb (
   NTSTATUS Status
   );


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
