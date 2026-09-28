/* Public-API admission probe: real targets, no product helper or desktop. */
#define _WIN32_WINNT 0x0A00
#include "ntkvm-exe/native_conpty.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>

typedef struct output_probe {
    HANDLE seen;
    char bytes[32768];
    DWORD used;
} output_probe;

static HANDLE capability(PCWSTR name)
{
    WCHAR value[64];
    return GetEnvironmentVariableW(name,value,64) ? (HANDLE)(ULONG_PTR)wcstoul(value,NULL,16) : NULL;
}
static DWORD drain(void *context,const BYTE *bytes,DWORD count)
{
    output_probe *probe=context;
    if(count>=sizeof(probe->bytes)-probe->used)return ERROR_BUFFER_OVERFLOW;
    memcpy(probe->bytes+probe->used,bytes,count);probe->used+=count;
    probe->bytes[probe->used]=0;
    if(strstr(probe->bytes,"ADMISSION-OUTPUT-READY"))SetEvent(probe->seen);
    return 0;
}
int wmain(int argc,WCHAR **argv)
{
    HANDLE ready=NULL,finish=NULL;
    ntkvm_conpty *console=NULL;PROCESS_INFORMATION hold={0},fast={0};run16_native_start start={0};
    WCHAR self[MAX_PATH],command[1024],directory[MAX_PATH];DWORD error=0,code=0;int result=1;
    DWORD released=MAXDWORD;
    DWORD delivered=0;
    output_probe probe={0};
    BOOL retain=argc==2 && !wcscmp(argv[1],L"--retain");
    static const WCHAR environment[]={0,0};COORD size={80,25};
    if(argc==2 && !wcscmp(argv[1],L"--hold")) {
        if(!SetEvent(capability(L"NTVDM_FRONTEND_CAPABILITY")))return 81;
        return WaitForSingleObject(capability(L"NTVDM_EXECUTION_CONSOLE"),10000)==WAIT_OBJECT_0 ? 23 : 82;
    }
    if(argc==2 && !wcscmp(argv[1],L"--fast")) {
        DWORD count=0;char key=0;
        HANDLE input=GetStdHandle(STD_INPUT_HANDLE),output=GetStdHandle(STD_OUTPUT_HANDLE);
        if(!SetConsoleMode(input,0) ||
            !WriteConsoleA(output,"ADMISSION-OUTPUT-READY",22,&count,NULL) || count!=22)return 83;
        if(!ReadFile(input,&key,1,&count,NULL) || count!=1 || key!='x')return 84;
        return 37;
    }
    if(argc!=1 && !retain)return 2;
    if(!GetModuleFileNameW(NULL,self,MAX_PATH) || !GetCurrentDirectoryW(MAX_PATH,directory))goto done;
    ready=CreateEventW(NULL,TRUE,FALSE,NULL);finish=CreateEventW(NULL,TRUE,FALSE,NULL);
    probe.seen=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!ready || !finish || !probe.seen || ntkvm_conpty_open(size,drain,&probe,&console))goto done;
    start.application=self;start.directory=directory;start.environment=environment;start.console_mask=7;
    start.capabilities[0]=ready;start.capabilities[1]=finish;
    swprintf_s(command,1024,L"\"%ls\" --hold",self);start.command=command;
    error=ntkvm_conpty_launch(console,&start,&hold);
    if(error || WaitForSingleObject(ready,5000)!=WAIT_OBJECT_0)goto done;
    if(!retain) {released=ntkvm_conpty_release(console);if(released)goto done;}
    swprintf_s(command,1024,L"\"%ls\" --fast",self);
    error=ntkvm_conpty_launch(console,&start,&fast);
    printf("release=%08lx new-launch=%lu holding-target=%lu\n",(unsigned long)released,error,
        WaitForSingleObject(hold.hProcess,0));
    if(!retain) {
        if(error!=ERROR_SHUTDOWN_IN_PROGRESS || fast.hProcess || fast.hThread ||
            WaitForSingleObject(hold.hProcess,0)!=WAIT_TIMEOUT)goto done;
        error=0;goto finish_clients;
    }
    if(error)goto done;
    if(WaitForSingleObject(probe.seen,5000)!=WAIT_OBJECT_0) {
        printf("post-release-output-missing bytes=%lu\n",probe.used);goto done;
    }
    error=ntkvm_conpty_write(console,"x",1,&delivered);
    if(error || delivered!=1 || WaitForSingleObject(fast.hProcess,5000)!=WAIT_OBJECT_0 ||
        !GetExitCodeProcess(fast.hProcess,&code) || code!=37 ||
        WaitForSingleObject(hold.hProcess,0)!=WAIT_TIMEOUT)goto done;
    if(retain) {released=ntkvm_conpty_release(console);if(released)goto done;}
finish_clients:
    SetEvent(finish);
    if(WaitForSingleObject(hold.hProcess,5000)!=WAIT_OBJECT_0 ||
        !GetExitCodeProcess(hold.hProcess,&code) || code!=23 ||
        WaitForSingleObject(ntkvm_conpty_ended(console),5000)!=WAIT_OBJECT_0 || ntkvm_conpty_error(console))goto done;
    result=0;
done:
    if(finish)SetEvent(finish);
    if(hold.hProcess && WaitForSingleObject(hold.hProcess,1000)==WAIT_TIMEOUT)TerminateProcess(hold.hProcess,99);
    if(fast.hProcess && WaitForSingleObject(fast.hProcess,1000)==WAIT_TIMEOUT)TerminateProcess(fast.hProcess,99);
    ntkvm_conpty_close(console);
    if(probe.seen)CloseHandle(probe.seen);
    if(hold.hProcess)CloseHandle(hold.hProcess);if(hold.hThread)CloseHandle(hold.hThread);
    if(fast.hProcess)CloseHandle(fast.hProcess);if(fast.hThread)CloseHandle(fast.hThread);
    if(ready)CloseHandle(ready);if(finish)CloseHandle(finish);
    printf("CONPTY-RELEASE-ADMISSION %s mode=%s error=%lu exit=%lu\n",
        result ? "FAIL" : "PASS",retain ? "retained-real-io" : "released-refusal",error,code);
    return result;
}
