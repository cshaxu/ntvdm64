#include "wow_native_process.h"
#include "wow_user_task_lifecycle.h"

DWORD WINAPI NtvdmInstallChildStyle(HANDLE,DWORD);

BOOL WINAPI wow_native_create_process_a(LPCSTR application,LPSTR command,
    LPSECURITY_ATTRIBUTES process_attributes,LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit,DWORD flags,LPVOID environment,LPCSTR directory,
    LPSTARTUPINFOA startup,LPPROCESS_INFORMATION process)
{
    wow_user_runtime_thread *binding=wow_user_runtime_current();
    DWORD style,error,saved;
    if(!process || !startup){SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    if(!binding || !binding->runtime || !binding->runtime->lifecycle ||
       !wow_user_task_lifecycle_style(binding->runtime->lifecycle,&style)) {
        SetLastError(ERROR_INVALID_STATE);return FALSE;
    }
    if(!CreateProcessA(application,command,process_attributes,thread_attributes,
        inherit,flags|CREATE_SUSPENDED,environment,directory,startup,process))return FALSE;
    saved=GetLastError();
    error=NtvdmInstallChildStyle(process->hProcess,style);
    if(!error && !(flags&CREATE_SUSPENDED) && ResumeThread(process->hThread)==(DWORD)-1)
        error=GetLastError();
    if(error) {
        TerminateProcess(process->hProcess,error);
        WaitForSingleObject(process->hProcess,INFINITE);
        CloseHandle(process->hThread);CloseHandle(process->hProcess);
        ZeroMemory(process,sizeof(*process));SetLastError(error);return FALSE;
    }
    SetLastError(saved);return TRUE;
}
