/* Finite binding for original winsrv token-query helper used by BaseSrv. */
#ifndef OPENNT_BASE_INTERACTIVE_H
#define OPENNT_BASE_INTERACTIVE_H
#include <nt.h>
typedef struct {
    LUID AuthenticationId;
    BOOL Valid;
} OPENNT_BASE_INTERACTIVE_SCOPE;
BOOL OpenNtBaseInitializeInteractiveScope(OPENNT_BASE_INTERACTIVE_SCOPE *);
const OPENNT_BASE_INTERACTIVE_SCOPE *OpenNtBaseBindInteractiveScope(const OPENNT_BASE_INTERACTIVE_SCOPE *);
NTSTATUS NtUserTestForInteractiveUser(PLUID);
NTSTATUS _UserTestTokenForInteractive(HANDLE, PLUID);
NTSTATUS NTAPI NtQueryInformationToken(HANDLE, TOKEN_INFORMATION_CLASS, PVOID, ULONG, PULONG);
#ifndef STATUS_BUFFER_TOO_SMALL
#define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
#endif
#endif
