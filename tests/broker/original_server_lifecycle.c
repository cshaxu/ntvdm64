/* Test host mechanics only. All VDM record policy is linked from srvvdm.c. */
#include "basesrv.h"
#include <stdio.h>
#include <string.h>

static CSR_PROCESS caller;
static CSR_THREAD thread;
PFNNOTIFYPROCESSCREATE UserNotifyProcessCreate = NULL;
PVOID NTAPI RtlProcessHeap(VOID) { return GetProcessHeap(); }
PCSR_THREAD ProbeAuthenticatedRequestThread(void) { return &thread; }
NTSTATUS NTAPI CsrLockProcessByClientId(HANDLE id, PCSR_PROCESS *out)
{
    if (id != (HANDLE)GetCurrentProcessId()) return (NTSTATUS)0xc000000b;
    *out = &caller;
    return 0;
}
NTSTATUS NTAPI CsrUnlockProcess(PCSR_PROCESS process) { (void)process; return 0; }

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x); return 1; } } while(0)

int main(void)
{
    BASE_API_MSG m;
    CSR_REPLY_STATUS reply = 0;
    PCONSOLERECORD record = NULL;
    char command[] = "MEM.EXE\r\n", dirs[] = "C:\\TEST\0", output[32];
    STARTUPINFOA startup = {sizeof(startup)};
    ULONG status;
    HANDLE parentWait, workerWait;
    BaseSrvHeap = GetProcessHeap();
    caller.ProcessHandle = GetCurrentProcess();
    caller.SequenceNumber = 1;
    thread.Process = &caller;
    thread.ThreadHandle = GetCurrentThread();
    thread.ClientId.UniqueProcess = (HANDLE)GetCurrentProcessId();
    thread.ClientId.UniqueThread = (HANDLE)GetCurrentThreadId();
    BaseSrvVDMInit();

    ZeroMemory(&m,sizeof(m));
    CHECK(BaseSrvIsFirstVDM((PCSR_API_MSG)&m,&reply) == 0 && m.u.IsFirstVDM.FirstVDM);
    CHECK(BaseSrvIsFirstVDM((PCSR_API_MSG)&m,&reply) == 0 && !m.u.IsFirstVDM.FirstVDM);

    ZeroMemory(&m,sizeof(m));
    m.u.CheckVDM.ConsoleHandle = (HANDLE)1; /* Local fixture record key, not IPC/native handle. */
    m.u.CheckVDM.BinaryType = BINARY_TYPE_DOS;
    m.u.CheckVDM.CmdLine = command;
    m.u.CheckVDM.CmdLen = sizeof(command);
    m.u.CheckVDM.StartupInfo = &startup;
    CHECK(BaseSrvCheckVDM((PCSR_API_MSG)&m,&reply) == 0);
    CHECK(m.u.CheckVDM.VDMState == VDM_NOT_PRESENT);
    CHECK(BaseSrvGetConsoleRecord((HANDLE)1,&record) == 0 && record != NULL);
    CHECK(record->DOSRecord->VDMState == VDM_TO_TAKE_A_COMMAND);
    CHECK(!memcmp(record->DOSRecord->lpVDMInfo->CmdLine,command,sizeof(command)));

    ZeroMemory(&m,sizeof(m));
    m.u.GetSetVDMCurDirs.ConsoleHandle = (HANDLE)1;
    m.u.GetSetVDMCurDirs.lpszzCurDirs = dirs;
    m.u.GetSetVDMCurDirs.cchCurDirs = sizeof(dirs);
    CHECK(BaseSrvSetVDMCurDirs((PCSR_API_MSG)&m,&reply) == 0);
    m.u.GetSetVDMCurDirs.lpszzCurDirs = output;
    m.u.GetSetVDMCurDirs.cchCurDirs = 1;
    status = BaseSrvGetVDMCurDirs((PCSR_API_MSG)&m,&reply);
    CHECK(status == (ULONG)STATUS_INVALID_PARAMETER);
    CHECK(m.u.GetSetVDMCurDirs.cchCurDirs == sizeof(dirs));
    CHECK(record->lpszzCurDirs != NULL);
    CHECK(BaseSrvGetVDMCurDirs((PCSR_API_MSG)&m,&reply) == 0);
    CHECK(!memcmp(output,dirs,sizeof(dirs)) && record->lpszzCurDirs == NULL);
    CHECK(BaseSrvGetVDMCurDirs((PCSR_API_MSG)&m,&reply) == 0);
    CHECK(m.u.GetSetVDMCurDirs.cchCurDirs == 0);

    ZeroMemory(&m,sizeof(m));
    m.u.UpdateVDMEntry.ConsoleHandle = (HANDLE)1;
    m.u.UpdateVDMEntry.BinaryType = BINARY_TYPE_DOS;
    m.u.UpdateVDMEntry.EntryIndex = UPDATE_VDM_PROCESS_HANDLE;
    m.u.UpdateVDMEntry.VDMProcessHandle = GetCurrentProcess();
    CHECK(BaseSrvUpdateVDMEntry((PCSR_API_MSG)&m,&reply) == 0);
    parentWait = m.u.UpdateVDMEntry.WaitObjectForParent;
    CHECK(parentWait && WaitForSingleObject(parentWait,0) == WAIT_TIMEOUT);

    ZeroMemory(&m,sizeof(m));
    m.u.GetNextVDMCommand.ConsoleHandle = (HANDLE)1;
    m.u.GetNextVDMCommand.VDMState = ASKING_FOR_FIRST_COMMAND;
    m.u.GetNextVDMCommand.CmdLine = output;
    m.u.GetNextVDMCommand.CmdLen = 1;
    m.u.GetNextVDMCommand.StartupInfo = &startup;
    CHECK(BaseSrvGetNextVDMCommand((PCSR_API_MSG)&m,&reply) == (ULONG)STATUS_INVALID_PARAMETER);
    CHECK(m.u.GetNextVDMCommand.CmdLen == sizeof(command));
    CHECK(record->DOSRecord->VDMState == VDM_TO_TAKE_A_COMMAND);
    CHECK(BaseSrvGetNextVDMCommand((PCSR_API_MSG)&m,&reply) == 0);
    CHECK(!memcmp(output,command,sizeof(command)));
    CHECK(record->DOSRecord->VDMState == VDM_BUSY);
    CHECK(WaitForSingleObject(parentWait,0) == WAIT_TIMEOUT);

    ZeroMemory(&m,sizeof(m));
    m.u.GetNextVDMCommand.ConsoleHandle = (HANDLE)1;
    m.u.GetNextVDMCommand.ExitCode = 7;
    CHECK(BaseSrvGetNextVDMCommand((PCSR_API_MSG)&m,&reply) == 0);
    workerWait = m.u.GetNextVDMCommand.WaitObjectForVDM;
    CHECK(workerWait && WaitForSingleObject(workerWait,0) == WAIT_TIMEOUT);
    CHECK(WaitForSingleObject(parentWait,0) == WAIT_OBJECT_0);
    CHECK(record->DOSRecord->ErrorCode == 7);
    CHECK(record->DOSRecord->VDMState == VDM_HAS_RETURNED_ERROR_CODE);
    CloseHandle(parentWait);

    ZeroMemory(&m,sizeof(m));
    m.u.SetReenterCount.ConsoleHandle = (HANDLE)1;
    m.u.SetReenterCount.fIncDec = INCREMENT_REENTER_COUNT;
    CHECK(BaseSrvSetReenterCount((PCSR_API_MSG)&m,&reply) == TRUE);
    CHECK(BaseSrvSetReenterCount((PCSR_API_MSG)&m,&reply) == TRUE);
    m.u.SetReenterCount.fIncDec = DECREMENT_REENTER_COUNT;
    CHECK(BaseSrvSetReenterCount((PCSR_API_MSG)&m,&reply) == TRUE);
    CHECK(record->nReEntrancy == 1 && WaitForSingleObject(workerWait,0) == WAIT_OBJECT_0);
    CHECK(BaseSrvSetReenterCount((PCSR_API_MSG)&m,&reply) == TRUE);

    ZeroMemory(&m,sizeof(m));
    m.u.ExitVDM.ConsoleHandle = (HANDLE)1;
    CHECK(BaseSrvExitDOSTask(&m.u.ExitVDM) == 0);
    CHECK(m.u.ExitVDM.WaitObjectForVDM == workerWait);
    CloseHandle(workerWait);
    CHECK(BaseSrvGetConsoleRecord((HANDLE)1,&record) == (ULONG)STATUS_INVALID_PARAMETER);
    CHECK(BaseSrvExitDOSTask(&m.u.ExitVDM) == (ULONG)STATUS_INVALID_PARAMETER);
    CHECK(BaseSrvIsFirstVDM((PCSR_API_MSG)&m,&reply) == 0 && !m.u.IsFirstVDM.FirstVDM);
    ZeroMemory(&m,sizeof(m));
    m.u.GetNextVDMCommand.ConsoleHandle = (HANDLE)-1;
    m.u.GetNextVDMCommand.CmdLen = 1;
    m.u.GetNextVDMCommand.WaitObjectForVDM = (HANDLE)1;
    CHECK(BaseSrvGetNextVDMCommand((PCSR_API_MSG)&m,&reply) == 0);
    CHECK(!m.u.GetNextVDMCommand.WaitObjectForVDM && !m.u.GetNextVDMCommand.CmdLen);
    puts("PASS: original first-VDM, record/command/directory capacity, dispatch/completion, parent/worker events, reentry, empty-WOW, cleanup");
    return 0;
}
