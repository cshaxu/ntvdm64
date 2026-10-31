#ifndef OPENNT_ABI_NTPSAPI_WOW32_H
#define OPENNT_ABI_NTPSAPI_WOW32_H

/*
 * Selected x86 declarations from OpenNT public/sdk/inc/ntpsapi.h
 * (SHA-256 CAFD48B3A9C672C6A18503D4565E7864A284EC029F63AE04FF221C58A2DB5148).
 *
 * The existing opennt-host ntpsapi carrier intentionally supplies only
 * NtAlertThread.  WOW32 also reaches the original thread-query contract.
 * Importing the whole NT4 header collides with current SDK process/thread
 * declarations, so this carrier retains only the source-used x86 layout and
 * signatures.  It does not emulate a thread, a TEB, or an NTDLL service.
 */
#ifndef ThreadBasicInformation
#define ThreadBasicInformation ((THREADINFOCLASS)0)
#endif

#ifndef OPENNT_WOW_PCLIENT_ID_DEFINED
#define OPENNT_WOW_PCLIENT_ID_DEFINED
typedef CLIENT_ID *PCLIENT_ID;
#endif

typedef struct _THREAD_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PVOID TebBaseAddress;
    CLIENT_ID ClientId;
    ULONG_PTR AffinityMask;
    KPRIORITY Priority;
    LONG BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

NTSYSAPI
NTSTATUS
NTAPI
NtOpenThread(
    OUT PHANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PCLIENT_ID ClientId OPTIONAL
    );

#ifndef NtCurrentThread
#define NtCurrentThread() ((HANDLE)-2)
#endif

#endif /* OPENNT_ABI_NTPSAPI_WOW32_H */
