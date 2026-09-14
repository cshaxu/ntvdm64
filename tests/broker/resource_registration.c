/* Test bridge between native RPC and the actual private process binding.
 * No service policy and no fixture implementation of registry operations. */
#include <base_process.h>
#include <stdio.h>
static OPENNT_BASE_PROCESS_REGISTRY state;
static CSR_PROCESS peer;
static BOOL registered;

DWORD test_registration_start(void)
{
    return OpenNtBaseInitializeProcessRegistry(&state)?0:GetLastError();
}

DWORD test_registration_retain(HANDLE process, DWORD expectedPid)
{
    PCSR_PROCESS found=NULL;
    OPENNT_BASE_PROCESS_REGISTRY *previous;
    DWORD error=0;
    if (!OpenNtBaseRegisterProcess(&state,&peer,process)) return GetLastError();
    registered=TRUE;
    previous=OpenNtBaseBindProcessRegistry(&state);
    if (CsrLockProcessByClientId((HANDLE)expectedPid,&found)<0) error=ERROR_NOT_FOUND;
    else {
        if (found!=&peer || GetProcessId(found->ProcessHandle)!=expectedPid) error=ERROR_INVALID_DATA;
        CsrUnlockProcess(found);
    }
    OpenNtBaseBindProcessRegistry(previous);
    printf("REGISTER status=%lu\n",error); fflush(stdout);
    return error;
}

DWORD test_registration_finish(void)
{
    if (registered) {
        /* Called after unregister waits for all RPC calls to complete. The
         * incoming attachment has expired, but our owned reference must work. */
        if (GetProcessId(peer.ProcessHandle)!=(DWORD)peer.ClientId.UniqueProcess ||
            WaitForSingleObject(peer.ProcessHandle,0)==WAIT_FAILED) return ERROR_INVALID_HANDLE;
        if (!OpenNtBaseRemoveProcess(&state,&peer)) return GetLastError();
        puts("REGISTER retained-after-call=1 drained=1"); fflush(stdout);
    }
    return OpenNtBaseDestroyProcessRegistry(&state)?0:GetLastError();
}
