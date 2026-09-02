/*
 * DIVERGENCE OPENNT-HOST-003: source-identified true subset of the original
 * public/sdk/inc/ntrtl.h.  The full NT4 declaration carrier reaches private
 * product APIs that are outside the selected modern user-mode closure.  This
 * file preserves only declarations reached by imported MVDM/OpenNT sources;
 * adapter-mvdm-host-out owns any modern implementation binding.
 */
#ifndef OPENNT_HOST_NTRTL_SUBSET_H
#define OPENNT_HOST_NTRTL_SUBSET_H

#include <nt.h>

#ifdef __cplusplus
extern "C" {
#endif

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
ULONG NTAPI RtlNtStatusToDosError(NTSTATUS Status);
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
#define NtCurrentProcess() ((HANDLE)(LONG_PTR)-1)
#endif

#ifndef InitializeListHead
#define InitializeListHead(ListHead) ((ListHead)->Flink = (ListHead)->Blink = (ListHead))
#define IsListEmpty(ListHead) ((ListHead)->Flink == (ListHead))
#define RemoveEntryList(Entry) { PLIST_ENTRY _blink = (Entry)->Blink; PLIST_ENTRY _flink = (Entry)->Flink; _blink->Flink = _flink; _flink->Blink = _blink; }
#define InsertHeadList(ListHead,Entry) { PLIST_ENTRY _flink = (ListHead)->Flink; (Entry)->Flink = _flink; (Entry)->Blink = (ListHead); _flink->Blink = (Entry); (ListHead)->Flink = (Entry); }
#endif

#ifdef __cplusplus
}
#endif

#endif
