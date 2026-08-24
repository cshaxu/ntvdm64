/*
 * Declaration-only native host surface needed by recovered historical host
 * sources. Layouts, values, and prototypes are retained OpenNT declarations
 * from public/sdk/inc/ntdef.h, ntioapi.h, and ntmmapi.h. This header provides
 * no host operation and is applied only to default-disabled historical gates.
 */
#ifndef APP_OPENNT_HOST_NATIVE_IO_COMPAT_H
#define APP_OPENNT_HOST_NATIVE_IO_COMPAT_H

#define APP_NTSYSAPI __declspec(dllimport)

/* OpenNT ntdef.h uses the same IA-32 base type.  Win32-first host units do
 * not otherwise receive this native status alias before this declaration set. */
typedef LONG NTSTATUS;

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} STRING, *PSTRING;

typedef STRING ANSI_STRING, *PANSI_STRING;
typedef STRING OEM_STRING, *POEM_STRING;
typedef const OEM_STRING *PCOEM_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

#define InitializeObjectAttributes(p, n, a, r, s) { \
    (p)->Length = sizeof(OBJECT_ATTRIBUTES);          \
    (p)->RootDirectory = r;                            \
    (p)->Attributes = a;                               \
    (p)->ObjectName = n;                               \
    (p)->SecurityDescriptor = s;                       \
    (p)->SecurityQualityOfService = NULL;              \
}

typedef struct _IO_STATUS_BLOCK {
    NTSTATUS Status;
    ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef enum _WAIT_TYPE {
    WaitAll,
    WaitAny
} WAIT_TYPE;

typedef VOID (NTAPI *PIO_APC_ROUTINE)(
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG Reserved);

#ifndef FILE_OPEN_IF
#define FILE_OPEN_IF 0x00000003UL
#endif
#ifndef OBJ_CASE_INSENSITIVE
#define OBJ_CASE_INSENSITIVE 0x00000040L
#endif
#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif
#ifndef STATUS_ALERTED
#define STATUS_ALERTED ((NTSTATUS)0x00000101L)
#endif
#ifndef STATUS_TIMEOUT
#define STATUS_TIMEOUT ((NTSTATUS)0x00000102L)
#endif

/* OpenNT public/sdk/inc/ntmmapi.h: native section-mapping contract. */
typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

#ifndef MEM_DOS_LIM
#define MEM_DOS_LIM 0x40000000
#endif

APP_NTSYSAPI LARGE_INTEGER NTAPI RtlExtendedLargeIntegerDivide(
    LARGE_INTEGER Dividend,
    ULONG Divisor,
    PULONG Remainder);

APP_NTSYSAPI LARGE_INTEGER NTAPI RtlExtendedIntegerMultiply(
    LARGE_INTEGER Multiplicand,
    LONG Multiplier);

APP_NTSYSAPI VOID NTAPI RtlInitUnicodeString(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString);

APP_NTSYSAPI VOID NTAPI RtlInitString(PSTRING, const char *);
APP_NTSYSAPI NTSTATUS NTAPI RtlOemStringToUnicodeString(PUNICODE_STRING, PCOEM_STRING, BOOLEAN);
APP_NTSYSAPI NTSTATUS NTAPI RtlUnicodeStringToAnsiString(PANSI_STRING, PCUNICODE_STRING, BOOLEAN);
APP_NTSYSAPI VOID NTAPI RtlFreeUnicodeString(PUNICODE_STRING);
APP_NTSYSAPI VOID NTAPI RtlFreeAnsiString(PANSI_STRING);

APP_NTSYSAPI NTSTATUS NTAPI NtCreateFile(
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    PLARGE_INTEGER AllocationSize,
    ULONG FileAttributes,
    ULONG ShareAccess,
    ULONG CreateDisposition,
    ULONG CreateOptions,
    PVOID EaBuffer,
    ULONG EaLength);

APP_NTSYSAPI NTSTATUS NTAPI NtCreateSection(
    PHANDLE SectionHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PLARGE_INTEGER MaximumSize,
    ULONG SectionPageProtection,
    ULONG AllocationAttributes,
    HANDLE FileHandle);

APP_NTSYSAPI NTSTATUS NTAPI NtMapViewOfSection(
    HANDLE SectionHandle,
    HANDLE ProcessHandle,
    PVOID *BaseAddress,
    ULONG ZeroBits,
    ULONG CommitSize,
    PLARGE_INTEGER SectionOffset,
    PULONG ViewSize,
    SECTION_INHERIT InheritDisposition,
    ULONG AllocationType,
    ULONG Protect);

APP_NTSYSAPI NTSTATUS NTAPI NtUnmapViewOfSection(
    HANDLE ProcessHandle,
    PVOID BaseAddress);

APP_NTSYSAPI NTSTATUS NTAPI NtDeviceIoControlFile(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG IoControlCode,
    PVOID InputBuffer,
    ULONG InputBufferLength,
    PVOID OutputBuffer,
    ULONG OutputBufferLength);

APP_NTSYSAPI NTSTATUS NTAPI NtWaitForSingleObject(HANDLE, BOOLEAN, PLARGE_INTEGER);
APP_NTSYSAPI NTSTATUS NTAPI NtWaitForMultipleObjects(ULONG, PHANDLE, WAIT_TYPE, BOOLEAN, PLARGE_INTEGER);
APP_NTSYSAPI NTSTATUS NTAPI NtAlertThread(HANDLE);
APP_NTSYSAPI NTSTATUS NTAPI NtQueryPerformanceCounter(PLARGE_INTEGER, PLARGE_INTEGER);
APP_NTSYSAPI ULONG NTAPI NtGetTickCount(VOID);
APP_NTSYSAPI NTSTATUS NTAPI NtTerminateProcess(HANDLE, NTSTATUS);
APP_NTSYSAPI NTSTATUS NTAPI RtlEnterCriticalSection(PRTL_CRITICAL_SECTION);
APP_NTSYSAPI NTSTATUS NTAPI RtlLeaveCriticalSection(PRTL_CRITICAL_SECTION);
APP_NTSYSAPI NTSTATUS NTAPI RtlInitializeCriticalSection(PRTL_CRITICAL_SECTION);
APP_NTSYSAPI ULONG NTAPI RtlNtStatusToDosError(NTSTATUS);

#ifndef NtCurrentProcess
#define NtCurrentProcess() ((HANDLE)(LONG_PTR)-1)
#endif

/* The historical host helper owns this unqualified name. */
#ifdef PlaySound
#undef PlaySound
#endif

#endif
