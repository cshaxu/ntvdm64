/* Test host mechanics only. All VDM record policy is linked from srvvdm.c. */
#include "basesrv.h"
#include <stdio.h>
#include <string.h>

static CSR_PROCESS caller;
static CSR_THREAD thread;
static OPENNT_SUPPORT_PROCESS_PARAMETERS parameters;
static OPENNT_SUPPORT_PEB peb;
static OPENNT_SUPPORT_TEB teb;
extern HANDLE hwndWowExec;
extern ULONG ulWowExecProcessSequenceNumber;
BOOL BaseUpdateVDMEntry(ULONG, HANDLE *, ULONG, ULONG);
BOOL BaseCheckVDM(ULONG, PCWCH, PCWCH, PCWCH, ANSI_STRING *, PBASE_API_MSG, PULONG, DWORD, LPSTARTUPINFOW);
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
static BOOL failCapture;
static ULONG launchCalls;
static HANDLE enqueueGate, queuedParent;
static ULONG retryCalls, retryExit;
static ULONG enqueueStatus;
PCSR_CAPTURE_HEADER NTAPI CsrAllocateCaptureBuffer(ULONG messages, ULONG pointers, ULONG size)
{
    PCSR_CAPTURE_HEADER capture;
    (void)messages; (void)pointers;
    if(failCapture) return NULL;
    if (size > 1024 * 1024) return NULL;
    size = ROUND_UP(size,4);
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
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepCheckVDM):
        ++launchCalls;
        result = BaseSrvCheckVDM(message,&reply); break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepIsFirstVDM):
        result = BaseSrvIsFirstVDM(message,&reply); break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand):
        if (enqueueGate) {
            PBASE_GET_NEXT_VDM_COMMAND_MSG request = &((PBASE_API_MSG)message)->u.GetNextVDMCommand;
            ++retryCalls;
            if (retryCalls == 2) retryExit = request->ExitCode;
        }
        result = BaseSrvGetNextVDMCommand(message,&reply);
        if (enqueueGate && ((PBASE_API_MSG)message)->u.GetNextVDMCommand.WaitObjectForVDM)
            SetEvent(enqueueGate);
        break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepSetReenterCount):
        result = BaseSrvSetReenterCount(message,&reply); break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepExitVDM):
        result = BaseSrvExitVDM(message,&reply); break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepSetVDMCurDirs):
        result = BaseSrvSetVDMCurDirs(message,&reply); break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetVDMCurDirs):
        result = BaseSrvGetVDMCurDirs(message,&reply); break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepBatNotification):
        result = BaseSrvBatNotification(message,&reply); break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepRegisterWowExec):
        result = BaseSrvRegisterWowExec(message,&reply); break;
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepUpdateVDMEntry):
        result = BaseSrvUpdateVDMEntry(message,&reply); break;
    default: return (NTSTATUS)STATUS_INVALID_PARAMETER;
    }
    message->ReturnValue = result;
    return result;
}

static DWORD WINAPI enqueue_after_wait(LPVOID unused)
{
    BASE_API_MSG message = {0};
    CSR_REPLY_STATUS reply = 0;
    STARTUPINFOA startup = {sizeof(startup)};
    char command[] = "NEXT.COM\r\n";
    (void)unused;
    if (WaitForSingleObject(enqueueGate,5000) != WAIT_OBJECT_0) return 2;
    message.u.CheckVDM.ConsoleHandle = (HANDLE)1;
    message.u.CheckVDM.BinaryType = BINARY_TYPE_DOS;
    message.u.CheckVDM.CmdLine = command;
    message.u.CheckVDM.CmdLen = sizeof(command);
    message.u.CheckVDM.StartupInfo = &startup;
    enqueueStatus = BaseSrvCheckVDM((PCSR_API_MSG)&message,&reply);
    queuedParent = message.u.CheckVDM.WaitObjectForParent;
    return 0;
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
    HWND registrationWindow;
    HANDLE sender;
    DWORD senderExit;
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
    CHECK(SetVDMCurrentDirectories(sizeof(dirs),dirs) && captures == 0);
    CHECK(GetVDMCurrentDirectories(0,NULL) == sizeof(dirs) && captures == 0);
    CHECK(record->lpszzCurDirs != NULL);
    CHECK(GetVDMCurrentDirectories(sizeof(output),output) == sizeof(dirs));
    CHECK(!memcmp(output,dirs,sizeof(dirs)) && captures == 0);
    CHECK(GetVDMCurrentDirectories(sizeof(output),output) == 0 && captures == 0);
    CmdBatNotification(CMD_BAT_OPERATION_STARTING);
    CHECK(BaseSrvGetBatRecord((HANDLE)1) != NULL);
    CmdBatNotification(CMD_BAT_OPERATION_TERMINATING);
    CHECK(BaseSrvGetBatRecord((HANDLE)1) == NULL);

    /* Registration-only test: an owned message-only window, not guest UI. */
    registrationWindow = CreateWindowExA(0,"STATIC","broker-registration-fixture",
        0,0,0,1,1,HWND_MESSAGE,NULL,GetModuleHandleW(NULL),NULL);
    CHECK(registrationWindow != NULL);
    RegisterWowExec(registrationWindow);
    CHECK(hwndWowExec == registrationWindow && ulWowExecProcessSequenceNumber == 1);
    CHECK(DestroyWindow(registrationWindow));
    RegisterWowExec(NULL);
    CHECK(hwndWowExec == NULL);

    ZeroMemory(&m,sizeof(m));
    m.u.UpdateVDMEntry.ConsoleHandle = (HANDLE)1;
    m.u.UpdateVDMEntry.BinaryType = BINARY_TYPE_DOS;
    m.u.UpdateVDMEntry.EntryIndex = UPDATE_VDM_PROCESS_HANDLE;
    m.u.UpdateVDMEntry.VDMProcessHandle = GetCurrentProcess();
    parentWait = GetCurrentProcess();
    CHECK(BaseUpdateVDMEntry(UPDATE_VDM_PROCESS_HANDLE,&parentWait,0,BINARY_TYPE_DOS));
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

    enqueueGate = CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(enqueueGate != NULL);
    sender = CreateThread(NULL,0,enqueue_after_wait,NULL,0,NULL);
    CHECK(sender != NULL);
    ZeroMemory(&clientInfo,sizeof(clientInfo));
    clientInfo.VDMState = NO_PARENT_TO_WAKE;
    clientInfo.ErrorCode = 99;
    clientInfo.CmdLine = output;
    clientInfo.CmdSize = sizeof(output);
    CHECK(GetNextVDMCommand(&clientInfo));
    CHECK(WaitForSingleObject(sender,5000) == WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(sender,&senderExit) && senderExit == 0 && enqueueStatus == 0);
    CHECK(retryCalls == 2 && retryExit == 0 && captures == 0);
    CHECK(!strcmp(output,"NEXT.COM\r\n") && queuedParent != NULL);
    CHECK(WaitForSingleObject(queuedParent,0) == WAIT_TIMEOUT);
    CloseHandle(sender); CloseHandle(enqueueGate); enqueueGate = NULL;

    ZeroMemory(&m,sizeof(m));
    m.u.ExitVDM.ConsoleHandle = (HANDLE)1;
    ExitVDM(FALSE,0);
    CHECK(WaitForSingleObject(queuedParent,0) == WAIT_OBJECT_0);
    CloseHandle(queuedParent);
    { DWORD handleFlags;
      CHECK(!GetHandleInformation(workerWait,&handleFlags) && GetLastError() == ERROR_INVALID_HANDLE); }
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
    /* Original launcher construction -> actual original server deep copy. */
    {
        STARTUPINFOW launch={sizeof(launch)};
        CHAR envBytes[]="PATH=O:\\ntvdm64\0";
        ANSI_STRING env={sizeof(envBytes),sizeof(envBytes),envBytes};
        ULONG task=0, before;
        launch.lpTitle=L"original launch test";
        launch.lpDesktop=L"default";
        launch.lpReserved=L"test reserved";
        launch.dwFlags=STARTF_USESTDHANDLES;
        ZeroMemory(&m,sizeof(m));
        CHECK(BaseCheckVDM(BINARY_TYPE_DOS|BINARY_TYPE_DOS_EXE,L"O:\\ntvdm64\\MEM.EXE",
            L"\"O:\\ntvdm64\\MEM.EXE\"  /?",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(launchCalls==1 && captures==0 && m.u.CheckVDM.VDMState==VDM_NOT_PRESENT);
        CHECK(BaseSrvGetConsoleRecord((HANDLE)1,&record)==0 && record!=NULL);
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->CmdLine,"/?\r\n"));
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->AppName,"O:\\ntvdm64\\MEM.EXE"));
        CHECK(record->DOSRecord->lpVDMInfo->EnviornmentSize==sizeof(envBytes));
        CHECK(!memcmp(record->DOSRecord->lpVDMInfo->Enviornment,envBytes,sizeof(envBytes)));
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->Title,"original launch test"));
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->Desktop,"default"));
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->Reserved,"test reserved"));
        ExitVDM(FALSE,0);
        CHECK(BaseSrvGetConsoleRecord((HANDLE)1,&record)==(ULONG)STATUS_INVALID_PARAMETER);
        launch.dwFlags=STARTF_USEHOTKEY;
        launch.hStdInput=(HANDLE)42;
        ZeroMemory(&m,sizeof(m));
        CHECK(BaseCheckVDM(BINARY_TYPE_DOS,L"O:\\ntvdm64\\MEM.EXE",L"MEM.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(BaseSrvGetConsoleRecord((HANDLE)1,&record)==0 && record!=NULL);
        CHECK(!strcmp(record->DOSRecord->lpVDMInfo->Reserved,"hotkey.42 test reserved"));
        CHECK(!(launch.dwFlags & STARTF_USEHOTKEY) && launch.hStdInput==NULL && captures==0);
        /* Original routine frees its replacement reserved string on return. */
        launch.lpReserved=NULL;
        ExitVDM(FALSE,0);
        before=launchCalls;
        failCapture=TRUE;
        ZeroMemory(&m,sizeof(m));
        CHECK(!BaseCheckVDM(BINARY_TYPE_DOS,L"O:\\ntvdm64\\MEM.EXE",L"MEM.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(GetLastError()==ERROR_NOT_ENOUGH_MEMORY && captures==0 && launchCalls==before);
        failCapture=FALSE;
        CHECK(!BaseCheckVDM(BINARY_TYPE_DOS,L"O:\\ntvdm64\\MEM.EXE",L"MEM.EXE",L"O:\\ntvdm64",NULL,&m,&task,0,&launch));
        CHECK(GetLastError()==ERROR_INVALID_PARAMETER && captures==0 && launchCalls==before);
        puts("PASS: original BaseCheckVDM command/environment/startup capture, server ownership and allocation failure");
    }
    puts("PASS: original first-VDM, record/command/directory capacity, dispatch/completion, parent/worker events, reentry, empty-WOW, cleanup");
    return 0;
}
