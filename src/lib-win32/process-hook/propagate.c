#include "propagate.h"
#include "unwind.h"
static process_hook_create_w create_w;
typedef BOOL (WINAPI *process_hook_create_a)(LPCSTR,LPSTR,LPSECURITY_ATTRIBUTES,
    LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCSTR,LPSTARTUPINFOA,LPPROCESS_INFORMATION);
static process_hook_create_a create_a;
static process_hook_propagation_ops ops;
static _Thread_local unsigned entering;
static void leave(void *unused){(void)unused;--entering;}
static BOOL finish(BOOL created,DWORD flags,PROCESS_INFORMATION *child,DWORD original_error)
{
    if(!created){SetLastError(original_error);return FALSE;}
    DWORD error=ops.install(child->hProcess,create_w,ops.context);
    if(!error && !(flags&CREATE_SUSPENDED)){
        DWORD previous=ResumeThread(child->hThread);
        if(previous!=1)error=previous==(DWORD)-1?GetLastError():ERROR_INVALID_STATE;
    }
    if(error){
        TerminateProcess(child->hProcess,error);WaitForSingleObject(child->hProcess,5000);
        CloseHandle(child->hThread);CloseHandle(child->hProcess);ZeroMemory(child,sizeof(*child));
        SetLastError(error);return FALSE;
    }
    SetLastError(original_error);return TRUE;
}
static PROCESS_HOOK_UNWIND_FUNCTION BOOL WINAPI hook_w(LPCWSTR application,LPWSTR command,LPSECURITY_ATTRIBUTES pa,
    LPSECURITY_ATTRIBUTES ta,BOOL inherit,DWORD flags,LPVOID environment,LPCWSTR directory,
    LPSTARTUPINFOW startup,LPPROCESS_INFORMATION child)
{
    if(entering || !startup || !child || !ops.active(ops.context) || !ops.accept_w(application,command,ops.context))
        return create_w(application,command,pa,ta,inherit,flags,environment,directory,startup,child);
    BOOL result=FALSE;DWORD error=ERROR_SUCCESS;process_hook_unwind_scope scope;++entering;
    PROCESS_HOOK_UNWIND_BEGIN(scope,leave,NULL);
    {
        result=create_w(application,command,pa,ta,inherit,flags|CREATE_SUSPENDED,environment,directory,startup,child);
        error=GetLastError();result=finish(result,flags,child,error);error=GetLastError();
    }
    PROCESS_HOOK_UNWIND_END(scope);SetLastError(error);return result;
}
static PROCESS_HOOK_UNWIND_FUNCTION BOOL WINAPI hook_a(LPCSTR application,LPSTR command,LPSECURITY_ATTRIBUTES pa,
    LPSECURITY_ATTRIBUTES ta,BOOL inherit,DWORD flags,LPVOID environment,LPCSTR directory,
    LPSTARTUPINFOA startup,LPPROCESS_INFORMATION child)
{
    if(entering || !startup || !child || !ops.active(ops.context) || !ops.accept_a(application,command,ops.context))
        return create_a(application,command,pa,ta,inherit,flags,environment,directory,startup,child);
    BOOL result=FALSE;DWORD error=ERROR_SUCCESS;process_hook_unwind_scope scope;++entering;
    PROCESS_HOOK_UNWIND_BEGIN(scope,leave,NULL);
    {
        result=create_a(application,command,pa,ta,inherit,flags|CREATE_SUSPENDED,environment,directory,startup,child);
        error=GetLastError();result=finish(result,flags,child,error);error=GetLastError();
    }
    PROCESS_HOOK_UNWIND_END(scope);SetLastError(error);return result;
}
DWORD process_hook_propagation_attach(DWORD (*attach)(void *,void *),const process_hook_propagation_ops *value)
{
    if(!attach || !value || !value->active || !value->accept_w || !value->accept_a || !value->install)return ERROR_INVALID_PARAMETER;
    ops=*value;create_w=CreateProcessW;create_a=CreateProcessA;
    DWORD error=attach(&create_w,(void *)hook_w);
    if(!error)error=attach(&create_a,(void *)hook_a);
    return error;
}
