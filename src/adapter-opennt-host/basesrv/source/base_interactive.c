/* Original NtUserTestForInteractiveUser compares the caller LUID to the
 * kernel's interactive-windowstation user. The standalone broker has an
 * explicitly scoped process logon instead; capture that trusted local scope
 * only on a visible process windowstation. Never accept a wire-provided LUID.
 * Token inspection and SYSTEM impersonation policy remain original owners. */
#include <base_interactive.h>
static __declspec(thread) const OPENNT_BASE_INTERACTIVE_SCOPE *boundScope;
BOOL OpenNtBaseInitializeInteractiveScope(OPENNT_BASE_INTERACTIVE_SCOPE *scope)
{
    USEROBJECTFLAGS flags;
    TOKEN_STATISTICS statistics;
    DWORD size;
    HANDLE token;
    BOOL ok;
    if(!scope) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    ZeroMemory(scope,sizeof(*scope));
    if(!GetUserObjectInformationW(GetProcessWindowStation(),UOI_FLAGS,&flags,sizeof(flags),&size) ||
        !(flags.dwFlags & WSF_VISIBLE)) { SetLastError(ERROR_ACCESS_DENIED); return FALSE; }
    if(!OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&token)) return FALSE;
    ok=GetTokenInformation(token,TokenStatistics,&statistics,sizeof(statistics),&size);
    { DWORD error=GetLastError(); CloseHandle(token); SetLastError(error); }
    if(!ok) return FALSE;
    /* SYSTEM_LUID from original winnt.h is not an interactive user's logon.
     * Do not turn a visible service process into a shared-WOW allow rule. */
    if(statistics.AuthenticationId.LowPart==0x3e7 && statistics.AuthenticationId.HighPart==0) {
        SetLastError(ERROR_ACCESS_DENIED); return FALSE;
    }
    scope->AuthenticationId=statistics.AuthenticationId;
    scope->Valid=TRUE;
    return TRUE;
}
const OPENNT_BASE_INTERACTIVE_SCOPE *OpenNtBaseBindInteractiveScope(const OPENNT_BASE_INTERACTIVE_SCOPE *scope)
{
    const OPENNT_BASE_INTERACTIVE_SCOPE *previous=boundScope;
    boundScope=scope;
    return previous;
}
NTSTATUS NtUserTestForInteractiveUser(PLUID caller)
{
    if(!boundScope || !boundScope->Valid || !caller ||
        (boundScope->AuthenticationId.LowPart==0x3e7 && boundScope->AuthenticationId.HighPart==0) ||
        caller->LowPart!=boundScope->AuthenticationId.LowPart ||
        caller->HighPart!=boundScope->AuthenticationId.HighPart)
        return (NTSTATUS)0xc0000022L;
    return 0;
}
