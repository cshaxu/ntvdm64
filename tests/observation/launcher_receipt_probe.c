/* Diagnostic-only launcher composition: original production entry and real
 * providers. Wrap only the client's outcomes, never supply a fake receipt.
 * Failure-only writes preserve success-path timing and Win32 last-error. */
#include <nt.h>
#include "run16-exe/frontend_scope.h"
#include "ntsrv-exe/opennt/include/base_client.h"
#include <stdio.h>
static void receipt_failure(const char *stage,DWORD error,DWORD code,DWORD completed)
{
    DWORD saved=GetLastError(),length,written;HANDLE file;WCHAR path[MAX_PATH];char line[180];
    if(!error && !code)return;
    length=GetEnvironmentVariableW(L"RUN16_RECEIPT_PROBE_LOG",path,MAX_PATH);
    if(length && length<MAX_PATH) {
        length=(DWORD)sprintf_s(line,sizeof(line),"pid=%lu stage=%s error=%lu code=%lu completed=%lu\r\n",
            GetCurrentProcessId(),stage,error,code,completed);
        file=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,0,NULL);
        if(file!=INVALID_HANDLE_VALUE){WriteFile(file,line,length,&written,NULL);CloseHandle(file);}
    }
    SetLastError(saved);
}
static DWORD probe_wait(run16_frontend_scope *scope,DWORD *code,DWORD *completed)
{
    DWORD error=run16_frontend_scope_wait_native(scope,code,completed);
    receipt_failure("receipt",error,*code,*completed);return error;
}
static DWORD probe_launch(run16_frontend_scope *scope,const run16_native_start *start)
{
    DWORD error=run16_frontend_scope_launch_win32_text(scope,start);
    receipt_failure("startup",error,0,0);return error;
}
static DWORD probe_retire(run16_frontend_scope *scope)
{
    DWORD error=run16_frontend_scope_retire(scope);
    receipt_failure("retire",error,0,0);return error;
}
static DWORD probe_restore(run16_frontend_scope *scope)
{
    DWORD error=run16_frontend_scope_restore_parent(scope);
    receipt_failure("restore",error,0,0);return error;
}
static DWORD probe_resume(run16_frontend_scope *scope)
{
    DWORD error=run16_frontend_scope_resume_parent(scope);
    receipt_failure("resume",error,0,0);return error;
}
static BOOL probe_dos_result(HANDLE wait,LPDWORD code)
{
    BOOL completed=BaseCheckForVDM(wait,code);
    DWORD saved=GetLastError(),error=completed ? 0 : saved;
    receipt_failure("dos-result",error,completed ? *code : 0,completed);
    SetLastError(saved);return completed;
}
#define run16_frontend_scope_wait_native probe_wait
#define run16_frontend_scope_launch_win32_text probe_launch
#define run16_frontend_scope_retire probe_retire
#define run16_frontend_scope_restore_parent probe_restore
#define run16_frontend_scope_resume_parent probe_resume
#define BaseCheckForVDM probe_dos_result
#include "../../src/run16-exe/main.c"
