/* Test host mechanics only. All VDM record policy is linked from srvvdm.c. */
#include "basesrv.h"
#include <base_interactive.h>
#include <base_capture.h>
#include <base_config.h>
#include <base_process.h>
#include <stdio.h>
#include <string.h>

static CSR_PROCESS caller;
static CSR_THREAD thread;
static OPENNT_BASE_PROCESS_REGISTRY processRegistry;
extern HANDLE hwndWowExec;
extern ULONG ulWowExecProcessSequenceNumber;
extern NTSTATUS (*UserTestTokenForInteractive)(HANDLE, PLUID);
extern PWOWHEAD WOWHead;
extern LUID WowAuthId;
BOOL BaseUpdateVDMEntry(ULONG, HANDLE *, ULONG, ULONG);
BOOL BaseCheckForVDM(HANDLE, LPDWORD);
BOOL BaseCheckVDM(ULONG, PCWCH, PCWCH, PCWCH, ANSI_STRING *, PBASE_API_MSG, PULONG, DWORD, LPSTARTUPINFOW);
PFNNOTIFYPROCESSCREATE UserNotifyProcessCreate = NULL;

/* Test dispatch only; original capture routines use a real private heap. */
PVOID CsrPortHeap;
#define captures capture_blocks()
static ULONG launchCalls;
static HANDLE enqueueGate, queuedParent;
static ULONG retryCalls, retryExit;
static ULONG enqueueStatus;
static ULONG capture_blocks(void)
{
    PROCESS_HEAP_ENTRY entry={0};
    ULONG count=0;
    if(!HeapLock(CsrPortHeap)) return MAXULONG;
    while(HeapWalk(CsrPortHeap,&entry)) if(entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) ++count;
    HeapUnlock(CsrPortHeap);
    return count;
}
NTSTATUS NTAPI CsrClientCallServer(PCSR_API_MSG message, PCSR_CAPTURE_HEADER capture,
    CSR_API_NUMBER number, ULONG length)
{
    CSR_REPLY_STATUS reply = 0;
    ULONG result;
    (void)capture; (void)length;
    /* Test transport supplies its authenticated local caller, never wire PID. */
    message->h.ClientId=thread.ClientId;
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
    case CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetVDMExitCode):
        result = BaseSrvGetVDMExitCode(message,&reply); break;
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
    CSR_THREAD localThread = thread;
    if (OpenNtBaseServerRequestThread()!=NULL) return 3;
    if (WaitForSingleObject(enqueueGate,5000) != WAIT_OBJECT_0) return 2;
    localThread.ThreadHandle=GetCurrentThread();
    localThread.ClientId.UniqueThread=(HANDLE)GetCurrentThreadId();
    if (OpenNtBaseBindServerRequestThread(&localThread)!=NULL) return 4;
    if (OpenNtBaseBindProcessRegistry(&processRegistry)!=NULL) return 6;
    message.u.CheckVDM.ConsoleHandle = (HANDLE)1;
    message.u.CheckVDM.BinaryType = BINARY_TYPE_DOS;
    message.u.CheckVDM.CmdLine = command;
    message.u.CheckVDM.CmdLen = sizeof(command);
    message.u.CheckVDM.StartupInfo = &startup;
    enqueueStatus = BaseSrvCheckVDM((PCSR_API_MSG)&message,&reply);
    queuedParent = message.u.CheckVDM.WaitObjectForParent;
    if (OpenNtBaseBindProcessRegistry(NULL)!=&processRegistry) return 7;
    if (OpenNtBaseServerRequestThread()!=&localThread ||
        OpenNtBaseBindServerRequestThread(NULL)!=&localThread ||
        OpenNtBaseServerRequestThread()!=NULL) return 5;
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
    {
        OPENNT_BASE_VDM_CONFIG config;
        UNICODE_STRING commandLine={0};
        ULONG reserve=0;
        WCHAR shortKernel[MAX_PATH];
        CHAR tooLong[MAX_VDM_CFG_LINE+1];
        memset(tooLong,'x',sizeof(tooLong)-1); tooLong[sizeof(tooLong)-1]=0;
        CHECK(!OpenNtBaseInitializeVdmConfig(&config,tooLong,"kernel"));
        CHECK(!OpenNtBaseInitializeVdmConfig(&config,"bad\"image","kernel"));
        CHECK(!BaseGetVdmConfigInfo(NULL,0,BINARY_TYPE_DOS,&commandLine,&reserve));
        CHECK(commandLine.Buffer==NULL);
        CHECK(OpenNtBaseInitializeVdmConfig(&config,"O:\\package with spaces\\ntvdm.exe",
            "O:\\ntvdm64\\system32\\KRNL386"));
        CHECK(OpenNtBaseBindVdmConfig(&config)==NULL);
        {
            CHAR small[2]={'x',0};
            DWORD capacity=sizeof(small), size=0xfeed;
            CHECK(!BaseGetVDMKeyword(CMDLINE,small,&capacity,DOSSIZE,&size));
            CHECK(small[0]=='x' && capacity==sizeof(small) && size==0xfeed);
            CHECK(!BaseGetVDMKeyword(CMDLINE,small,&capacity,WOWSIZE,&size));
        }
        CHECK(BaseGetVdmConfigInfo(NULL,0,BINARY_TYPE_DOS,&commandLine,&reserve));
        CHECK(!wcscmp(commandLine.Buffer,L"\"O:\\package with spaces\\ntvdm.exe\" -f"));
        CHECK(reserve==16*1024*1024);
        CHECK(HeapFree(GetProcessHeap(),0,commandLine.Buffer));
        CHECK(BaseGetVdmConfigInfo(NULL,0x12ab,BINARY_TYPE_DOS,&commandLine,&reserve));
        CHECK(!wcscmp(commandLine.Buffer,L"\"O:\\package with spaces\\ntvdm.exe\" -f -i12ab"));
        CHECK(HeapFree(GetProcessHeap(),0,commandLine.Buffer));
        CHECK(GetShortPathNameW(L"O:\\ntvdm64\\system32\\KRNL386.exe",shortKernel,MAX_PATH)>0);
        CHECK(BaseGetVdmConfigInfo(NULL,0,BINARY_TYPE_WIN16,&commandLine,&reserve));
        CHECK(wcsstr(commandLine.Buffer,L"\" -f -w -a ")!=NULL);
        CHECK(!wcscmp(wcsstr(commandLine.Buffer,L" -a ")+4,shortKernel));
        CHECK(HeapFree(GetProcessHeap(),0,commandLine.Buffer));
        CHECK(OpenNtBaseBindVdmConfig(NULL)==&config);
        puts("PASS: original DOS/WOW worker command construction, package path spaces, session ID and config rejection");
    }
    CsrPortHeap=HeapCreate(0,0,0);
    CHECK(CsrPortHeap!=NULL && captures==0);
    {
        PCSR_CAPTURE_HEADER capture=CsrAllocateCaptureBuffer(3,0,5);
        PVOID first=NULL,empty=(PVOID)1,last=NULL;
        CHECK(capture!=NULL && captures==1);
        CHECK(CsrAllocateMessagePointer(capture,1,&first)==4);
        CHECK(CsrAllocateMessagePointer(capture,0,&empty)==0 && empty==NULL);
        CHECK(CsrAllocateMessagePointer(capture,3,&last)==4 && (PCHAR)last==(PCHAR)first+4);
        CHECK(capture->CountMessagePointers==3 && capture->CountCapturePointers==0);
        CHECK(capture->MessagePointerOffsets[0]==(ULONG)&first && capture->MessagePointerOffsets[1]==0);
        CHECK(capture->MessagePointerOffsets[2]==(ULONG)&last);
        CHECK(capture->FreeSpace<=(PCHAR)capture+capture->Length);
        CsrFreeCaptureBuffer(capture);
        CHECK(captures==0 && CsrAllocateCaptureBuffer(1,0,MAXLONG)==NULL && captures==0);
    }
    CHECK(NtCurrentPeb()->ImageBaseAddress==GetModuleHandleW(NULL));
    CHECK(RtlProcessHeap()==GetProcessHeap());
    CHECK(opennt_support_current_teb()->ClientId.UniqueProcess==(HANDLE)GetCurrentProcessId());
    /* Local fixture Console association only. A stdout HANDLE is not the
     * authenticated cross-process Console ID required by product dispatch. */
    NtCurrentPeb()->ProcessParameters->ConsoleHandle = (HANDLE)1;
    CHECK(OpenNtBaseInitializeProcessRegistry(&processRegistry));
    CHECK(OpenNtBaseBindProcessRegistry(&processRegistry)==NULL);
    CHECK(OpenNtBaseRegisterProcess(&processRegistry,&caller,GetCurrentProcess()));
    CHECK(caller.SequenceNumber==1);
    {
        CSR_PROCESS duplicate={0};
        PCSR_PROCESS found=(PCSR_PROCESS)1;
        CHECK(!OpenNtBaseRegisterProcess(&processRegistry,&duplicate,GetCurrentProcess()));
        CHECK(GetLastError()==ERROR_ALREADY_EXISTS);
        CHECK(!OpenNtBaseDestroyProcessRegistry(&processRegistry));
        CHECK(GetLastError()==ERROR_BUSY);
        CHECK(CsrLockProcessByClientId(NULL,&found)<0 && found==NULL);
        CHECK(CsrLockProcessByClientId((HANDLE)GetCurrentProcessId(),&found)==0 && found==&caller);
        CHECK(!OpenNtBaseRemoveProcess(&processRegistry,&caller) && GetLastError()==ERROR_BUSY);
        CHECK(CsrUnlockProcess(found)==0);
    }
    thread.Process = &caller;
    thread.ThreadHandle = GetCurrentThread();
    thread.ClientId.UniqueProcess = (HANDLE)GetCurrentProcessId();
    thread.ClientId.UniqueThread = (HANDLE)GetCurrentThreadId();
    CHECK(OpenNtBaseServerRequestThread()==NULL);
    CHECK(OpenNtBaseBindServerRequestThread(&thread)==NULL);
    {
        CSR_THREAD nested=thread;
        PCSR_THREAD previous=OpenNtBaseBindServerRequestThread(&nested);
        CHECK(previous==&thread && OpenNtBaseServerRequestThread()==&nested);
        CHECK(OpenNtBaseBindServerRequestThread(previous)==&nested);
        CHECK(OpenNtBaseServerRequestThread()==&thread);
    }
    BaseSrvVDMInit();
    { LUID negative=RtlConvertLongToLuid(-1), positive=RtlConvertLongToLuid(0x7fffffff);
      CHECK(negative.LowPart==0xffffffff && negative.HighPart==-1);
      CHECK(positive.LowPart==0x7fffffff && positive.HighPart==0); }

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
    {
        DWORD taskExit=0xfeed;
        CHECK(!BaseCheckForVDM(GetCurrentProcess(),&taskExit) && taskExit==0xfeed);
        CHECK(!BaseCheckForVDM(NULL,&taskExit) && taskExit==0xfeed);
        CHECK(BaseCheckForVDM(parentWait,&taskExit) && taskExit==STILL_ACTIVE);
        CHECK(record->DOSRecord->VDMState==VDM_TO_TAKE_A_COMMAND);
    }

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
    {
        DWORD taskExit=0xfeed;
        CHECK(BaseCheckForVDM(parentWait,&taskExit) && taskExit==7);
        CHECK(record->DOSRecord->VDMState==VDM_READY && !record->DOSRecord->hWaitForParent);
        CHECK(BaseCheckForVDM(parentWait,&taskExit) && taskExit==0);
    }
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
        HANDLE normalHeap=CsrPortHeap;
        HANDLE exhaustedHeap=HeapCreate(0,4096,4096);
        ULONG allocations=0;
        CHECK(exhaustedHeap!=NULL);
        while(allocations<4096 && HeapAlloc(exhaustedHeap,0,128)) ++allocations;
        CHECK(allocations<4096);
        CsrPortHeap=exhaustedHeap;
        ZeroMemory(&m,sizeof(m));
        CHECK(!BaseCheckVDM(BINARY_TYPE_DOS,L"O:\\ntvdm64\\MEM.EXE",L"MEM.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(GetLastError()==ERROR_NOT_ENOUGH_MEMORY && launchCalls==before);
        CsrPortHeap=normalHeap;
        CHECK(captures==0 && HeapDestroy(exhaustedHeap));
        CHECK(!BaseCheckVDM(BINARY_TYPE_DOS,L"O:\\ntvdm64\\MEM.EXE",L"MEM.EXE",L"O:\\ntvdm64",NULL,&m,&task,0,&launch));
        CHECK(GetLastError()==ERROR_INVALID_PARAMETER && captures==0 && launchCalls==before);
        puts("PASS: original BaseCheckVDM command/environment/startup capture, server ownership and allocation failure");
    }
    {
        OPENNT_BASE_INTERACTIVE_SCOPE scope,wrong;
        STARTUPINFOW launch={sizeof(launch)};
        CHAR envBytes[]="PATH=O:\\ntvdm64\0";
        ANSI_STRING env={sizeof(envBytes),sizeof(envBytes),envBytes};
        ULONG task=0;
        HANDLE undo;
        LUID invalidOutput={0x12345678,0x12345678};
        CHECK(!OpenNtBaseInitializeInteractiveScope(NULL) && GetLastError()==ERROR_INVALID_PARAMETER);
        CHECK(!NT_SUCCESS(_UserTestTokenForInteractive(NULL,&invalidOutput)));
        CHECK(invalidOutput.LowPart==0x12345678 && invalidOutput.HighPart==0x12345678);
        CHECK(OpenNtBaseInitializeInteractiveScope(&scope));
        wrong=scope; wrong.AuthenticationId.LowPart=0x3e7; wrong.AuthenticationId.HighPart=0;
        CHECK(OpenNtBaseBindInteractiveScope(&wrong)==NULL);
        CHECK(!NT_SUCCESS(NtUserTestForInteractiveUser(&wrong.AuthenticationId)));
        CHECK(OpenNtBaseBindInteractiveScope(NULL)==&wrong);
        UserTestTokenForInteractive=_UserTestTokenForInteractive;
        ZeroMemory(&m,sizeof(m));
        CHECK(!BaseCheckVDM(BINARY_TYPE_WIN16,L"O:\\ntvdm64\\system32\\WRITE.EXE",L"WRITE.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(GetLastError()==ERROR_ACCESS_DENIED && WOWHead==NULL && captures==0);
        wrong=scope; wrong.AuthenticationId.LowPart^=1;
        CHECK(OpenNtBaseBindInteractiveScope(&wrong)==NULL);
        ZeroMemory(&m,sizeof(m));
        CHECK(!BaseCheckVDM(BINARY_TYPE_WIN16,L"O:\\ntvdm64\\system32\\WRITE.EXE",L"WRITE.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(GetLastError()==ERROR_ACCESS_DENIED && WOWHead==NULL && captures==0);
        CHECK(OpenNtBaseBindInteractiveScope(&scope)==&wrong);
        ZeroMemory(&m,sizeof(m));
        CHECK(BaseCheckVDM(BINARY_TYPE_WIN16,L"O:\\ntvdm64\\system32\\WRITE.EXE",L"WRITE.EXE",L"O:\\ntvdm64",&env,&m,&task,0,&launch));
        CHECK(WOWHead!=NULL && task!=0 && m.u.CheckVDM.VDMState==VDM_NOT_PRESENT && captures==0);
        CHECK(WOWHead->WOWRecord->iTask==task);
        CHECK(WowAuthId.LowPart==scope.AuthenticationId.LowPart && WowAuthId.HighPart==scope.AuthenticationId.HighPart);
        CHECK(!strcmp(WOWHead->WOWRecord->lpVDMInfo->AppName,"O:\\ntvdm64\\system32\\WRITE.EXE"));
        undo=(HANDLE)task;
        CHECK(BaseUpdateVDMEntry(UPDATE_VDM_UNDO_CREATION,&undo,VDM_PARTIALLY_CREATED,BINARY_TYPE_WIN16));
        CHECK(WOWHead==NULL && OpenNtBaseBindInteractiveScope(NULL)==&scope);
        CHECK(WowAuthId.LowPart==0xffffffff && WowAuthId.HighPart==-1);
        puts("PASS: original shared-WOW admission rejects absent/wrong scope, accepts matching logon and undoes launch");
    }
    {
        OPENNT_BASE_INTERACTIVE_SCOPE scope;
        STARTUPINFOW launch={sizeof(launch)};
        CHAR envBytes[]="PATH=O:\\ntvdm64\0";
        ANSI_STRING env={sizeof(envBytes),sizeof(envBytes),envBytes};
        ULONG types[2]={BINARY_TYPE_DOS,BINARY_TYPE_WIN16},i;
        CHECK(OpenNtBaseInitializeInteractiveScope(&scope));
        CHECK(OpenNtBaseBindInteractiveScope(&scope)==NULL);
        for(i=0;i<2;++i) {
            CSR_PROCESS worker={0};
            ULONG task=0, sequence=0x100+i;
            HANDLE console=i?(HANDLE)-1:(HANDLE)1;
            HANDLE wait=GetCurrentProcess();
            LPCWSTR app=i?L"O:\\ntvdm64\\system32\\WRITE.EXE":L"O:\\ntvdm64\\MEM.EXE";
            ZeroMemory(&m,sizeof(m));
            CHECK(BaseCheckVDM(types[i],app,app,L"O:\\ntvdm64",&env,&m,&task,0,&launch));
            CHECK(BaseUpdateVDMEntry(UPDATE_VDM_PROCESS_HANDLE,&wait,task,types[i]));
            CHECK(WaitForSingleObject(wait,0)==WAIT_TIMEOUT);
            BaseSrvUpdateVDMSequenceNumber(console,sequence,task);
            BaseSrvUpdateVDMSequenceNumber(console,sequence+10,task);
            if(i) CHECK(WOWHead && WOWHead->SequenceNumber==sequence);
            else {
                CHECK(BaseSrvGetConsoleRecord(console,&record)==0);
                CHECK(record->SequenceNumber==sequence);
            }
            worker.fVDM=TRUE; worker.SequenceNumber=sequence+10;
            BaseSrvCleanupVDMResources(&worker);
            CHECK(WaitForSingleObject(wait,0)==WAIT_TIMEOUT);
            worker.fVDM=FALSE; worker.SequenceNumber=sequence;
            BaseSrvCleanupVDMResources(&worker);
            CHECK(WaitForSingleObject(wait,0)==WAIT_TIMEOUT);
            worker.fVDM=TRUE;
            BaseSrvCleanupVDMResources(&worker);
            CHECK(WaitForSingleObject(wait,0)==WAIT_OBJECT_0);
            if(i) CHECK(WOWHead==NULL && hwndWowExec==NULL && ulWowExecProcessSequenceNumber==0);
            else CHECK(BaseSrvGetConsoleRecord(console,&record)==(ULONG)STATUS_INVALID_PARAMETER);
            BaseSrvCleanupVDMResources(&worker);
            CHECK(WaitForSingleObject(wait,0)==WAIT_OBJECT_0);
            CHECK(CloseHandle(wait) && captures==0);
        }
        CHECK(OpenNtBaseBindInteractiveScope(NULL)==&scope);
        puts("PASS: original DOS/WOW generation registration and termination cleanup, wrong generation/role rejection and repeat cleanup");
    }
    CHECK(OpenNtBaseServerRequestThread()==&thread);
    CHECK(OpenNtBaseBindServerRequestThread(NULL)==&thread);
    CHECK(OpenNtBaseServerRequestThread()==NULL);
    puts("PASS: request-thread isolation, nested restoration and explicit unbind");
    CHECK(OpenNtBaseRemoveProcess(&processRegistry,&caller));
    CHECK(!OpenNtBaseRemoveProcess(&processRegistry,&caller) && GetLastError()==ERROR_NOT_FOUND);
    CHECK(!OpenNtBaseRegisterProcess(&processRegistry,&caller,NULL));
    CHECK(OpenNtBaseRegisterProcess(&processRegistry,&caller,GetCurrentProcess()));
    CHECK(caller.SequenceNumber==2);
    CHECK(OpenNtBaseRemoveProcess(&processRegistry,&caller));
    processRegistry.NextSequence=MAXULONG;
    CHECK(!OpenNtBaseRegisterProcess(&processRegistry,&caller,GetCurrentProcess()));
    CHECK(GetLastError()==ERROR_ARITHMETIC_OVERFLOW);
    CHECK(OpenNtBaseBindProcessRegistry(NULL)==&processRegistry);
    CHECK(OpenNtBaseDestroyProcessRegistry(&processRegistry));
    puts("PASS: registered process ownership, duplicate/missing/pinned removal, empty teardown and sequence exhaustion");
    CHECK(captures==0 && HeapDestroy(CsrPortHeap));
    puts("PASS: original first-VDM, record/command/directory capacity, dispatch/completion, parent/worker events, reentry, empty-WOW, cleanup");
    return 0;
}
