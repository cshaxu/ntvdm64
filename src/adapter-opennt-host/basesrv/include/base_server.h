/* BaseSrv-private native declarations; original owners are OpenNT
 * public/sdk/inc/{ntlpcapi,ntcsrsrv,ntexapi,ntseapi,ntrtl,nturtl,ntldr,
 * ntobapi,ntdef}.h. No CSR transport or server policy is implemented here.
 * The process binding supplies the authenticated request thread; callers
 * must never synthesize it from an untrusted wire PID. */
#ifndef OPENNT_BASE_SERVER_BINDING_H
#define OPENNT_BASE_SERVER_BINDING_H
#include <nt.h>
typedef CLIENT_ID *PCLIENT_ID;
typedef struct _HARDERROR_MSG *PHARDERROR_MSG;
#include <ntlpcapi.h>
#include <ntcsrsrv.h>
#undef CSR_SERVER_QUERYCLIENTTHREAD
PCSR_THREAD OpenNtBaseServerRequestThread(void);
#define CSR_SERVER_QUERYCLIENTTHREAD() OpenNtBaseServerRequestThread()
#ifndef STATUS_ACCESS_DENIED
#define STATUS_ACCESS_DENIED ((NTSTATUS)0xC0000022L)
#endif
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
#endif
