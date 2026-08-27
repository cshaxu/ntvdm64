#ifndef OPENNT_SUPPORT_NTRTL_H
#define OPENNT_SUPPORT_NTRTL_H

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
NTSTATUS NTAPI RtlUnicodeStringToOemString(POEM_STRING DestinationString, PCUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString);
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

BOOLEAN NTAPI RtlCreateUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString);
LONG NTAPI RtlCompareUnicodeString(PCUNICODE_STRING String1, PCUNICODE_STRING String2, BOOLEAN CaseInSensitive);
NTSTATUS NTAPI RtlAppendUnicodeToString(PUNICODE_STRING Destination, PCWSTR Source);
NTSTATUS NTAPI RtlUpcaseUnicodeStringToCountedOemString(POEM_STRING DestinationString, PCUNICODE_STRING SourceString, BOOLEAN AllocateDestinationString);

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
