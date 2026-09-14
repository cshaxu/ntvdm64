/* Test host mechanics only. All VDM record policy is linked from srvvdm.c. */
#include "basesrv.h"
#include <stdio.h>
#include <string.h>

static CSR_PROCESS caller;
static CSR_THREAD thread;
static OPENNT_SUPPORT_PROCESS_PARAMETERS parameters;
static OPENNT_SUPPORT_PEB peb;
static OPENNT_SUPPORT_TEB teb;
POPENNT_SUPPORT_PEB NTAPI NtCurrentPeb(VOID) { return &peb; }
POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(VOID) { return &teb; }
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

/* Test-local capture/dispatch transport; original client owns retry/copy policy. */
static ULONG captures;
PCSR_CAPTURE_HEADER NTAPI CsrAllocateCaptureBuffer(ULONG messages, ULONG pointers, ULONG size)
{
    PCSR_CAPTURE_HEADER capture;
    (void)messages; (void)pointers;
    if (size > 1024 * 1024) return NULL;
    capture = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*capture) + size);
    if (!capture) return NULL;
    capture->Length = sizeof(*capture) + size;
    capture->FreeSpace = (PCHAR)(capture + 1);
    ++captures;
    return capture;
}
VOID NTAPI CsrFreeCaptureBuffer(PCSR_CAPTURE_HEADER capture)
{
    if (capture) { --captures; HeapFree(GetProcessHeap(),0,capture); }
}
ULONG NTAPI CsrAllocateMessagePointer(PCSR_CAPTURE_HEADER capture, ULONG size, PVOID *pointer)
{
    ULONG aligned = ROUND_UP(size,4);
    if (aligned < size || aligned > capture->Length ||
        capture->FreeSpace > (PCHAR)capture + capture->Length - aligned) {
        *pointer = NULL; return 0;
    }
    *pointer = capture->FreeSpace;
    capture->FreeSpace += aligned;
    return aligned;
}
NTSTATUS NTAPI CsrClientCallServer(PCSR_API_MSG message, PCSR_CAPTURE_HEADER capture,
    CSR_API_NUMBER number, ULONG length)
{
    CSR_REPLY_STATUS reply = 0;
    ULONG result;
    (void)capture; (void)length;
    switch (number) {
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepIsFirstVDM):
        result = BaseSrvIsFirstVDM(message,&reply); break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand):
        result = BaseSrvGetNextVDMCommand(message,&reply); break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepSetReenterCount):
        result = BaseSrvSetReenterCount(message,&reply); break;
    default: return (NTSTATUS)STATUS_INVALID_PARAMETER;
    }
    message->ReturnValue = result;
    return result;
}

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
    VDMINFO clientInfo;
    BaseSrvHeap = GetProcessHeap();
    parameters.ConsoleHandle = (HANDLE)1;
    peb.ProcessParameters = &parameters;
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
    CHECK(GetNextVDMCommand(NULL) == FALSE);

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
    ZeroMemory(&clientInfo,sizeof(clientInfo));
    clientInfo.VDMState = ASKING_FOR_FIRST_COMMAND;
    clientInfo.CmdLine = output;
    clientInfo.CmdSize = 1;
    CHECK(!GetNextVDMCommand(&clientInfo));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(clientInfo.CmdSize == sizeof(command) && captures == 0);
    CHECK(record->DOSRecord->VDMState == VDM_TO_TAKE_A_COMMAND);
    CHECK(GetNextVDMCommand(&clientInfo) && captures == 0);
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
    ZeroMemory(&clientInfo,sizeof(clientInfo));
    clientInfo.VDMState = ASKING_FOR_WOW_BINARY;
    clientInfo.CmdLine = output;
    clientInfo.CmdSize = sizeof(output);
    CHECK(GetNextVDMCommand(&clientInfo));
    CHECK(!clientInfo.CmdSize && captures == 0);
    puts("PASS: original first-VDM, record/command/directory capacity, dispatch/completion, parent/worker events, reentry, empty-WOW, cleanup");
    return 0;
}
