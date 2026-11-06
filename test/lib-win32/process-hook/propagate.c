#include "lib-win32/process-hook/propagate.h"
#include "lib-win32/process-hook/staging.h"
#include "lib-win32/process-hook/startup_hooks.h"
#include <stdio.h>
#include <wchar.h>
#include <string.h>
static BOOL enabled=TRUE,accepted=TRUE,fail_install=FALSE;static unsigned installations;static HANDLE failed_child;
static BOOL reenter,recursive_ok;
static DWORD launch(const WCHAR *,DWORD,BOOL);
static const BYTE payload[]={0x51,0x26,0x37,0xa5};
static BOOL active(void *context){(void)context;return enabled;}
static BOOL accept_w(LPCWSTR a,LPWSTR c,void *context){(void)a;(void)c;(void)context;return accepted;}
static BOOL accept_a(LPCSTR a,LPSTR c,void *context){(void)a;(void)c;(void)context;return accepted;}
static DWORD install(HANDLE process,process_hook_create_w create,void *context)
{
    (void)create;(void)context;++installations;
    if(reenter){reenter=FALSE;recursive_ok=launch(L"plain",0,FALSE)==73;}
    if(fail_install){
        if(!DuplicateHandle(GetCurrentProcess(),process,GetCurrentProcess(),&failed_child,0,FALSE,DUPLICATE_SAME_ACCESS))return GetLastError();
        return ERROR_BAD_EXE_FORMAT;
    }
    return process_hook_stage_payload(process,payload,sizeof(payload));
}
static DWORD consume(const void *data,DWORD bytes,DWORD machine,void *context)
{(void)machine;*(BOOL *)context=bytes==sizeof(payload) && !memcmp(data,payload,bytes);return ERROR_SUCCESS;}
static int initialize(void)
{
    const process_hook_propagation_ops ops={active,accept_w,accept_a,install,NULL};
    DWORD error=process_hook_startup_begin();if(error)return (int)error;
    error=process_hook_propagation_attach(process_hook_startup_attach,&ops);
    if(!error)error=process_hook_startup_commit();
    if(error)process_hook_startup_abort();
    return (int)error;
}
static DWORD launch(const WCHAR *mode,DWORD flags,BOOL resume)
{
    WCHAR self[MAX_PATH],command[MAX_PATH+64];STARTUPINFOW si={.cb=sizeof(si)};PROCESS_INFORMATION pi={0};
    if(!GetModuleFileNameW(NULL,self,ARRAYSIZE(self)))return 100;
    if(swprintf(command,ARRAYSIZE(command),L"\"%ls\" %ls",self,mode)<0)return 101;
    if(!CreateProcessW(self,command,NULL,NULL,FALSE,flags,NULL,NULL,&si,&pi))return GetLastError();
    if(resume && ResumeThread(pi.hThread)!=1){TerminateProcess(pi.hProcess,102);CloseHandle(pi.hThread);CloseHandle(pi.hProcess);return 102;}
    DWORD exit=103;if(WaitForSingleObject(pi.hProcess,5000)==WAIT_OBJECT_0)GetExitCodeProcess(pi.hProcess,&exit);
    else{TerminateProcess(pi.hProcess,ERROR_TIMEOUT);WaitForSingleObject(pi.hProcess,1000);}
    CloseHandle(pi.hThread);CloseHandle(pi.hProcess);return exit;
}
static DWORD launch_a(const char *mode)
{
    char self[MAX_PATH],command[MAX_PATH+64];STARTUPINFOA si={.cb=sizeof(si)};PROCESS_INFORMATION pi={0};
    if(!GetModuleFileNameA(NULL,self,ARRAYSIZE(self)))return 100;
    if(snprintf(command,sizeof(command),"\"%s\" %s",self,mode)<0)return 101;
    if(!CreateProcessA(self,command,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))return GetLastError();
    DWORD exit=103;if(WaitForSingleObject(pi.hProcess,5000)==WAIT_OBJECT_0)GetExitCodeProcess(pi.hProcess,&exit);
    else{TerminateProcess(pi.hProcess,ERROR_TIMEOUT);WaitForSingleObject(pi.hProcess,1000);}
    CloseHandle(pi.hThread);CloseHandle(pi.hProcess);return exit;
}
int wmain(int argc,WCHAR **argv)
{
    BOOL found=FALSE,valid=FALSE;
    if(GetModuleHandleW(L"cross-helper.dll") || GetModuleHandleW(L"second.dll"))return 1;
    if(argc>1 && wcscmp(argv[1],L"plain")){
        if(process_hook_stage_read(consume,&valid,&found) || !found || !valid || process_hook_stage_restore())return 2;
        if(!wcscmp(argv[1],L"leaf"))return 71;
        if(initialize() || launch(L"leaf",0,FALSE)!=71 || installations!=1)return 3;
        return 72;
    }
    if(argc>1)return process_hook_stage_read(consume,&valid,&found) || found?4:73;
    if(initialize())return 5;
    reenter=TRUE;if(launch(L"child",0,FALSE)!=72 || installations!=1 || !recursive_ok)return 6;
    if(launch(L"leaf",CREATE_SUSPENDED,TRUE)!=71 || installations!=2)return 7;
    if(launch_a("leaf")!=71 || installations!=3)return 8;
    enabled=FALSE;if(launch(L"plain",0,FALSE)!=73 || installations!=3)return 9;
    enabled=TRUE;accepted=FALSE;if(launch_a("plain")!=73 || installations!=3)return 10;
    accepted=TRUE;fail_install=TRUE;if(launch(L"leaf",0,FALSE)!=ERROR_BAD_EXE_FORMAT || installations!=4 || !failed_child)return 11;
    DWORD failed_exit=0;BOOL reaped=WaitForSingleObject(failed_child,0)==WAIT_OBJECT_0 && GetExitCodeProcess(failed_child,&failed_exit) && failed_exit==ERROR_BAD_EXE_FORMAT;
    CloseHandle(failed_child);if(!reaped)return 12;
    printf("PROCESS HOOK PROPAGATE PASS width=%u child=1 grandchild=1 aw=1 suspended=1 disabled=1 rejected=1 error193=1 no_style=1 reentrant=1\n",(unsigned)(sizeof(void*)*8));return 0;
}
