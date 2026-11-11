/* T412 S1 compile-only boundary probe. Not a product ABI/provider.
 * Declaration owners: original public/sdk/inc/{ntlpcapi,ntcsrsrv,ntexapi,
 * ntseapi,ntrtl,nturtl,ntldr,ntobapi,ntdef}.h. No service bodies here.
 * The probe deliberately leaves request binding and RTL inline resolution
 * undefined: producing an object is NOT a successful link/runtime claim.
 */
typedef CLIENT_ID *PCLIENT_ID;
typedef struct _HARDERROR_MSG *PHARDERROR_MSG;
#include <ntlpcapi.h>
#include <ntcsrsrv.h>
#undef CSR_SERVER_QUERYCLIENTTHREAD
PCSR_THREAD ProbeAuthenticatedRequestThread(void);
#define CSR_SERVER_QUERYCLIENTTHREAD() ProbeAuthenticatedRequestThread()
#ifndef STATUS_ACCESS_DENIED
#define STATUS_ACCESS_DENIED ((NTSTATUS)0xC0000022L)
#endif
LUID NTAPI RtlConvertLongToLuid(LONG value);
typedef enum _EVENT_TYPE { NotificationEvent, SynchronizationEvent } EVENT_TYPE;
NTSTATUS NTAPI NtCreateEvent(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, EVENT_TYPE, BOOLEAN);
NTSTATUS NTAPI NtSetEvent(HANDLE, PLONG);
NTSTATUS NTAPI NtResetEvent(HANDLE, PLONG);
NTSTATUS NTAPI NtOpenProcessToken(HANDLE, ACCESS_MASK, PHANDLE);
NTSTATUS NTAPI NtOpenThreadToken(HANDLE, ACCESS_MASK, BOOLEAN, PHANDLE);
NTSTATUS NTAPI NtDuplicateObject(HANDLE, HANDLE, HANDLE, PHANDLE, ACCESS_MASK, ULONG, ULONG);
NTSTATUS NTAPI NtQueryInformationToken(HANDLE, TOKEN_INFORMATION_CLASS, PVOID, ULONG, PULONG);
NTSTATUS NTAPI LdrLoadDll(PWSTR, PULONG, PUNICODE_STRING, PHANDLE);
NTSTATUS NTAPI LdrGetProcedureAddress(HANDLE, PANSI_STRING, ULONG, PVOID *);
NTSTATUS NTAPI LdrUnloadDll(HANDLE);
NTSTATUS NTAPI RtlAllocateAndInitializeSid(PSID_IDENTIFIER_AUTHORITY, UCHAR,
    ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, ULONG, PSID *);
BOOLEAN NTAPI RtlEqualSid(PSID, PSID);
#define RtlEqualLuid(L1,L2) (((L1)->HighPart == (L2)->HighPart) && ((L1)->LowPart == (L2)->LowPart))
#define RTL_HEAP_MAKE_TAG HEAP_MAKE_TAG_FLAGS
#define KdPrint(x) DbgPrint x
ULONG __cdecl DbgPrint(PCSTR, ...);
