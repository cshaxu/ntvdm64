/* Test bridge between native RPC and the actual private process binding.
 * No service policy and no fixture implementation of registry operations. */
#include "basesrv.h"
#include <base_process.h>
#include <base_dispatch.h>
#include <base_command.h>
#include <string.h>
/* Original private declaration in srvvdm.c; used only for fixture inspection. */
NTSTATUS GetConsoleRecordDosSesId(ULONG,PCONSOLERECORD *);
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

static NTSTATUS dispatch(BASE_API_MSG *message,ULONG operation,ULONG bytes)
{
    CSR_THREAD thread={0};
    PCSR_THREAD previousThread;
    OPENNT_BASE_PROCESS_REGISTRY *previousRegistry;
    NTSTATUS result;
    if (!registered) return STATUS_ACCESS_DENIED;
    thread.Process=&peer;
    thread.ClientId.UniqueProcess=peer.ClientId.UniqueProcess;
    /* This operation has no guest/remote-thread dependency. Do not invent a
     * caller TID from an RPC worker thread ID. */
    EnterCriticalSection(&dispatchLock);
    previousThread=OpenNtBaseBindServerRequestThread(&thread);
    previousRegistry=OpenNtBaseBindProcessRegistry(&state);
    result=OpenNtBaseDispatchOperation((PCSR_API_MSG)message,operation,bytes);
    OpenNtBaseBindProcessRegistry(previousRegistry);
    OpenNtBaseBindServerRequestThread(previousThread);
    LeaveCriticalSection(&dispatchLock);
    return result;
}

DWORD test_registration_first(ULONG *first)
{
    BASE_API_MSG message={0};
    *first=0;
    if (dispatch(&message,BROKER_VDM_IS_FIRST,sizeof(message.u.IsFirstVDM))) return ERROR_INVALID_DATA;
    *first=message.u.IsFirstVDM.FirstVDM;
    return 0;
}

DWORD test_registration_check(void *input,ULONG bytes,ULONG *task,ULONG *vdmState)
{
    BASE_API_MSG message={0},undo={0};
    STARTUPINFOA startup;
    uint32_t request;
    PCONSOLERECORD record=NULL;
    DWORD result=ERROR_INVALID_DATA;
    *task=*vdmState=0;
    if (!OpenNtBaseDecodeCheckCommand(input,bytes,1,&message,&startup,&request) ||
        message.u.CheckVDM.BinaryType!=BINARY_TYPE_DOS) return ERROR_INVALID_DATA;
    if (dispatch(&message,BROKER_VDM_CHECK,sizeof(message.u.CheckVDM))) return ERROR_INVALID_DATA;
    *task=message.u.CheckVDM.iTask; *vdmState=message.u.CheckVDM.VDMState;
    if (*task && *vdmState==VDM_NOT_PRESENT && !message.u.CheckVDM.WaitObjectForParent &&
        GetConsoleRecordDosSesId(*task,&record)==STATUS_SUCCESS && record && record->DOSRecord &&
        !strcmp(record->DOSRecord->lpVDMInfo->CmdLine,message.u.CheckVDM.CmdLine)) {
        /* Original record must own its copy after the request expires. */
        memset(message.u.CheckVDM.CmdLine,0,message.u.CheckVDM.CmdLen);
        if (!strcmp(record->DOSRecord->lpVDMInfo->CmdLine,"MEM.EXE\r\n")) result=0;
    }
    undo.u.UpdateVDMEntry.iTask=*task;
    undo.u.UpdateVDMEntry.BinaryType=BINARY_TYPE_DOS;
    undo.u.UpdateVDMEntry.EntryIndex=UPDATE_VDM_UNDO_CREATION;
    undo.u.UpdateVDMEntry.VDMCreationState=VDM_PARTIALLY_CREATED;
    if (dispatch(&undo,BROKER_VDM_UPDATE,sizeof(undo.u.UpdateVDMEntry))) result=ERROR_INVALID_DATA;
    record=NULL;
    if (GetConsoleRecordDosSesId(*task,&record)==STATUS_SUCCESS) result=ERROR_INVALID_DATA;
    printf("ORIGINAL-CHECK copied=1 no-worker=1 abort=%lu\n",result); fflush(stdout);
    return result;
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
