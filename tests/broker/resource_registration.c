/* Test bridge between native RPC and the actual private process binding.
 * No service policy and no fixture implementation of registry operations. */
#include "basesrv.h"
#include <base_process.h>
#include <base_dispatch.h>
#include <stdio.h>
static OPENNT_BASE_PROCESS_REGISTRY state;
static CSR_PROCESS peer;
static BOOL registered;
static CRITICAL_SECTION dispatchLock;
PFNNOTIFYPROCESSCREATE UserNotifyProcessCreate=NULL;

DWORD test_registration_start(void)
{
    if (!OpenNtBaseInitializeProcessRegistry(&state)) return GetLastError();
    InitializeCriticalSection(&dispatchLock);
    BaseSrvVDMInit();
    return 0;
}

DWORD test_registration_first(ULONG *first)
{
    BASE_API_MSG message={0};
    CSR_THREAD thread={0};
    PCSR_THREAD previousThread;
    OPENNT_BASE_PROCESS_REGISTRY *previousRegistry;
    NTSTATUS result;
    *first=0;
    if (!registered) return ERROR_ACCESS_DENIED;
    thread.Process=&peer;
    thread.ClientId.UniqueProcess=peer.ClientId.UniqueProcess;
    /* This operation has no guest/remote-thread dependency. Do not invent a
     * caller TID from an RPC worker thread ID. */
    EnterCriticalSection(&dispatchLock);
    previousThread=OpenNtBaseBindServerRequestThread(&thread);
    previousRegistry=OpenNtBaseBindProcessRegistry(&state);
    result=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_IS_FIRST,sizeof(message.u.IsFirstVDM));
    OpenNtBaseBindProcessRegistry(previousRegistry);
    OpenNtBaseBindServerRequestThread(previousThread);
    LeaveCriticalSection(&dispatchLock);
    if (result) return ERROR_INVALID_DATA;
    *first=message.u.IsFirstVDM.FirstVDM;
    return 0;
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
    if (!OpenNtBaseDestroyProcessRegistry(&state)) return GetLastError();
    DeleteCriticalSection(&dispatchLock);
    return 0;
}
