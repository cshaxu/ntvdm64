/* Demonstrates why process-job occupancy is not Console occupancy.
 * Real production ConPTY; no helper, desktop input or product policy change. */
#include "ntkvm-exe/native_conpty.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

static HANDLE capability(PCWSTR name)
{
    WCHAR value[64];
    return GetEnvironmentVariableW(name,value,64) ?
        (HANDLE)(ULONG_PTR)wcstoul(value,NULL,16) : NULL;
}
static DWORD drain(void *context,const BYTE *bytes,DWORD count)
{
    (void)context;(void)bytes;(void)count;return 0;
}
int wmain(int argc,WCHAR **argv)
{
    HANDLE ready=NULL,finish=NULL,job=NULL;
    PROCESS_INFORMATION child={0};run16_native_start start={0};
    ntkvm_conpty *pty=NULL;JOBOBJECT_BASIC_ACCOUNTING_INFORMATION accounting={0};
    WCHAR self[MAX_PATH],directory[MAX_PATH],command[1024];
    DWORD error=0,code=0,member=0;COORD size={80,25};int result=1;
    static const WCHAR environment[]={0,0};
    if(argc==2 && !wcscmp(argv[1],L"--detached")) {
        if(!GetConsoleProcessList(&member,1) || !FreeConsole())return 81;
        if(GetConsoleProcessList(&member,1) || GetLastError()!=ERROR_INVALID_HANDLE)return 82;
        if(!SetEvent(capability(L"NTVDM_FRONTEND_CAPABILITY")))return 83;
        return WaitForSingleObject(capability(L"NTVDM_EXECUTION_CONSOLE"),10000)==WAIT_OBJECT_0 ? 37 : 84;
    }
    if(argc!=1)return 2;
    if(!GetModuleFileNameW(NULL,self,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,directory))goto done;
    ready=CreateEventW(NULL,TRUE,FALSE,NULL);finish=CreateEventW(NULL,TRUE,FALSE,NULL);
    job=CreateJobObjectW(NULL,NULL); /* No limits, no KILL_ON_JOB_CLOSE. */
    if(!ready || !finish || !job || ntkvm_conpty_open(size,drain,NULL,&pty))goto done;
    swprintf_s(command,1024,L"\"%ls\" --detached",self);
    start.application=self;start.command=command;start.directory=directory;
    start.environment=environment;start.console_mask=7;
    start.capabilities[0]=ready;start.capabilities[1]=finish;
    error=ntkvm_conpty_launch(pty,&start,&child);
    if(error || !AssignProcessToJobObject(job,child.hProcess) ||
        WaitForSingleObject(ready,5000)!=WAIT_OBJECT_0)goto done;
    if(!QueryInformationJobObject(job,JobObjectBasicAccountingInformation,
        &accounting,sizeof(accounting),NULL) || accounting.ActiveProcesses!=1 ||
        WaitForSingleObject(child.hProcess,0)!=WAIT_TIMEOUT)goto done;
    error=ntkvm_conpty_release(pty);
    if(error || WaitForSingleObject(ntkvm_conpty_ended(pty),5000)!=WAIT_OBJECT_0 ||
        ntkvm_conpty_error(pty) || WaitForSingleObject(child.hProcess,0)!=WAIT_TIMEOUT)goto done;
    puts("CONPTY-JOB-MISMATCH PROVED active-job=1 live-target=yes console-EOF=yes");
    SetEvent(finish);
    if(WaitForSingleObject(child.hProcess,5000)!=WAIT_OBJECT_0 ||
        !GetExitCodeProcess(child.hProcess,&code) || code!=37)goto done;
    result=0;
done:
    if(finish)SetEvent(finish);
    if(child.hProcess && WaitForSingleObject(child.hProcess,1000)==WAIT_TIMEOUT)
        TerminateProcess(child.hProcess,99); /* Exact authored test only. */
    ntkvm_conpty_close(pty);
    if(child.hThread)CloseHandle(child.hThread);if(child.hProcess)CloseHandle(child.hProcess);
    if(job)CloseHandle(job);if(ready)CloseHandle(ready);if(finish)CloseHandle(finish);
    printf("CONPTY-JOB-MEMBERSHIP %s error=%lu exit=%lu\n",result ? "FAIL" : "PASS",error,code);
    return result;
}
