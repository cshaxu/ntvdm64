#include "intercept.h"
#include "detours/detours.h"
#include "common/application_search.h"
#include <wchar.h>
static decltype(&CreateProcessW) create_w=CreateProcessW;
static decltype(&CreateProcessA) create_a=CreateProcessA;
static __declspec(thread) unsigned entering;
/* Only a newly created, unreturned child belongs to this rollback. */
static BOOL finish(BOOL created,DWORD flags,PROCESS_INFORMATION *child,BOOL launcher)
{
    if(!created)return FALSE;
    DWORD saved=GetLastError(),error=ERROR_SUCCESS;
    nthook_context context=nthook_process_context;
    if(nthook_target32(child->hProcess)) {
        BOOL actual_launcher=FALSE;
        error=nthook_launcher_target(child->hProcess,&context,&actual_launcher);
        launcher=launcher || actual_launcher;
        DWORD subsystem=0;
        if(!launcher && (nthook_native_subsystem(child->hProcess,&subsystem) ||
            subsystem!=IMAGE_SUBSYSTEM_WINDOWS_CUI ||
            flags&(CREATE_NEW_CONSOLE|DETACHED_PROCESS|CREATE_NO_WINDOW)))
            context.frontend=context.execution=NULL;
        if(!error)error=nthook_install(child->hProcess,&context,
            launcher ? NATIVE_HOOK_LAUNCHER : NATIVE_HOOK_INTERCEPT);
    } else if(launcher)error=ERROR_NOT_SUPPORTED;
    if(!error && !(flags&CREATE_SUSPENDED) && ResumeThread(child->hThread)==(DWORD)-1)
        error=GetLastError();
    if(error) {
        TerminateProcess(child->hProcess,error);
        CloseHandle(child->hThread);CloseHandle(child->hProcess);
        ZeroMemory(child,sizeof(*child));SetLastError(error);return FALSE;
    }
    SetLastError(saved);return TRUE;
}
static PCWSTR command_tail(PCWSTR command,PWSTR token)
{
    PCWSTR begin=command,end;size_t count;BOOL quoted;
    while(*begin==L' ' || *begin==L'\t')++begin;
    quoted=*begin==L'"';
    if(quoted){++begin;end=wcschr(begin,L'"');}
    else {end=begin;while(*end && *end!=L' ' && *end!=L'\t')++end;}
    count=end ? end-begin : 0;
    if(!count || count>=MAX_PATH)return NULL;
    memcpy(token,begin,count*sizeof(WCHAR));token[count]=0;
    return quoted ? end+1 : end;
}
/* Owner-selected launch discovery is shared with run16: CWD, then each
 * PATH directory, with COM/EXE/BAT/PIF precedence inside each directory.
 * Explicit application paths remain exact; argv[0] is not an identity check. */
static BOOL legacy_application(PCWSTR application,PCWSTR command,DWORD flags,
    PWSTR resolved)
{
    WCHAR token[MAX_PATH];DWORD type;
    resolved[0]=0;
    if(!application) {
        if(!command || !command_tail(command,token))return FALSE;
        application=token;
    }
    if(common_resolve_application(application,resolved,MAX_PATH))return FALSE;
    if(flags&(CREATE_NEW_CONSOLE|DETACHED_PROCESS|CREATE_NO_WINDOW))return FALSE;
    return !nthook_legacy_type(resolved,&type);
}
static WCHAR *redirect_command(PCWSTR application,PCWSTR command)
{
    WCHAR *text,token[MAX_PATH];size_t count;
    /* Pin the already classified explicit application; preserve the complete
     * original tail. Selection/classification already used the shared search
     * and original classifier; no second argv[0] eligibility rule exists. */
    if(application) {
        PCWSTR tail=command && *command ? command_tail(command,token) : L"";
        if(!tail){SetLastError(ERROR_INVALID_PARAMETER);return NULL;}
        count=wcslen(nthook_process_context.launcher)+wcslen(application)+wcslen(tail)+7;
        if(count>32767){SetLastError(ERROR_FILENAME_EXCED_RANGE);return NULL;}
        text=(WCHAR *)HeapAlloc(GetProcessHeap(),0,count*sizeof(WCHAR));
        if(text)swprintf_s(text,count,L"\"%ls\" \"%ls\"%ls",nthook_process_context.launcher,application,tail);
    } else if(command && *command) {
        count=wcslen(nthook_process_context.launcher)+wcslen(command)+5;
        if(count>32767){SetLastError(ERROR_FILENAME_EXCED_RANGE);return NULL;}
        text=(WCHAR *)HeapAlloc(GetProcessHeap(),0,count*sizeof(WCHAR));
        if(text)swprintf_s(text,count,L"\"%ls\" %ls",nthook_process_context.launcher,command);
    } else {SetLastError(ERROR_INVALID_PARAMETER);return NULL;}
    if(!text)SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return text;
}
static BOOL WINAPI hooked_w(LPCWSTR application,LPWSTR command,
    LPSECURITY_ATTRIBUTES process_attributes,LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit,DWORD flags,LPVOID environment,LPCWSTR directory,
    LPSTARTUPINFOW startup,LPPROCESS_INFORMATION process)
{
    if(entering || !process || !startup || flags&(DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS))
        return create_w(application,command,process_attributes,thread_attributes,
            inherit,flags,environment,directory,startup,process);
    ++entering;
    WCHAR resolved[MAX_PATH];
    BOOL redirect=legacy_application(application,command,flags,resolved);
    BOOL launcher=redirect;
    WCHAR *copy=redirect ? redirect_command(resolved,command) : NULL;
    BOOL result=FALSE;DWORD error;
    if(!redirect || copy) {
        result=create_w(redirect ? nthook_process_context.launcher :
            (*resolved ? resolved : application),
            redirect ? copy : command,process_attributes,thread_attributes,inherit,
            flags|CREATE_SUSPENDED,environment,directory,startup,process);
        result=finish(result,flags,process,launcher);
    }
    error=GetLastError();if(copy)HeapFree(GetProcessHeap(),0,copy);
    --entering;SetLastError(error);return result;
}
static BOOL WINAPI hooked_a(LPCSTR application,LPSTR command,
    LPSECURITY_ATTRIBUTES process_attributes,LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit,DWORD flags,LPVOID environment,LPCSTR directory,
    LPSTARTUPINFOA startup,LPPROCESS_INFORMATION process)
{
    if(entering || !process || !startup || flags&(DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS))
        return create_a(application,command,process_attributes,thread_attributes,
            inherit,flags,environment,directory,startup,process);
    ++entering;
    WCHAR *wide_application=NULL,*wide_command=NULL,*wide_redirect=NULL;
    WCHAR resolved[MAX_PATH]={0};
    char launcher[MAX_PATH*2],selected[MAX_PATH*2],*redirect=NULL;
    BOOL lossy=FALSE,is_legacy=FALSE,result=FALSE;
    LPCSTR source[2]={application,command};WCHAR **wide[2]={&wide_application,&wide_command};
    DWORD error=ERROR_SUCCESS;
    for(unsigned i=0;i<2;++i)if(source[i]) {
        int count=MultiByteToWideChar(CP_ACP,0,source[i],-1,NULL,0);
        if(!count || count>32767){error=ERROR_INVALID_PARAMETER;break;}
        *wide[i]=(WCHAR *)HeapAlloc(GetProcessHeap(),0,count*sizeof(WCHAR));
        if(!*wide[i]){error=ERROR_NOT_ENOUGH_MEMORY;break;}
        if(!MultiByteToWideChar(CP_ACP,0,source[i],-1,*wide[i],count)){error=GetLastError();break;}
    }
    if(!error)is_legacy=legacy_application(wide_application,wide_command,flags,resolved);
    if(is_legacy) {
        wide_redirect=redirect_command(resolved,wide_command);
        if(!wide_redirect)error=GetLastError();
        if(!error && (!WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,
            nthook_process_context.launcher,-1,launcher,sizeof(launcher),NULL,&lossy) || lossy))
            error=ERROR_NO_UNICODE_TRANSLATION;
        if(!error) {
            int count=WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,wide_redirect,-1,NULL,0,NULL,&lossy);
            if(!count || lossy)error=ERROR_NO_UNICODE_TRANSLATION;
            else {
                redirect=(char *)HeapAlloc(GetProcessHeap(),0,count);
                if(!redirect)error=ERROR_NOT_ENOUGH_MEMORY;
                else if(!WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,wide_redirect,-1,
                    redirect,count,NULL,&lossy) || lossy)error=ERROR_NO_UNICODE_TRANSLATION;
            }
        }
    }
    if(!error && !is_legacy && *resolved &&
        (!WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,resolved,-1,
            selected,sizeof(selected),NULL,&lossy) || lossy))
        error=ERROR_NO_UNICODE_TRANSLATION;
    if(!error) {
        /* Actual creation remains ANSI, including the caller's environment,
         * directory and STARTUPINFO. Only an owned redirect line is encoded. */
        result=create_a(is_legacy ? launcher : (*resolved ? selected : application),
            is_legacy ? redirect : command,
            process_attributes,thread_attributes,inherit,flags|CREATE_SUSPENDED,
            environment,directory,startup,process);
        result=finish(result,flags,process,is_legacy);
        error=GetLastError();
    }
    if(redirect)HeapFree(GetProcessHeap(),0,redirect);
    if(wide_redirect)HeapFree(GetProcessHeap(),0,wide_redirect);
    if(wide_command)HeapFree(GetProcessHeap(),0,wide_command);
    if(wide_application)HeapFree(GetProcessHeap(),0,wide_application);
    --entering;SetLastError(error);return result;
}
LONG nthook_attach(void)
{
    LONG error=DetourTransactionBegin();
    if(!error)error=DetourUpdateThread(GetCurrentThread());
    if(!error)error=DetourAttach(&(PVOID &)create_w,hooked_w);
    if(!error)error=DetourAttach(&(PVOID &)create_a,hooked_a);
    if(error){DetourTransactionAbort();return error;}
    return DetourTransactionCommit();
}
