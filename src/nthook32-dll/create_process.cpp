#include "intercept.h"
#include "detours/detours.h"
#include "common/rpc/local_binding.h"
#include "common/rpc/management.h"
#include "ntsrv-exe/transport/rpc_security.h"
#include <wchar.h>
static decltype(&CreateProcessW) create_w=CreateProcessW;
static decltype(&CreateProcessA) create_a=CreateProcessA;
static __declspec(thread) unsigned entering;
#define NTHOOK_COMMAND_CAPACITY 32768u
/* Called only from the successful creation transaction, never DllMain.
 * Stateless finite publication: a missing/slow broker is diagnostic loss,
 * not a failed launch. No registration, retry, helper or inherited token. */
static void observe_created(HANDLE child,DWORD flags)
{
    broker_rpc_scope scope={0};common_rpc_management client={0};
    WCHAR endpoint[128];uint64_t identity=0;
    DWORD saved=GetLastError();
    if(!broker_rpc_capture_scope(&scope))goto done;
    swprintf_s(endpoint,ARRAYSIZE(endpoint),L"ntvdm-basesrv-%lu-%08lx-%08lx",scope.session,
        (ULONG)scope.logon.HighPart,(ULONG)scope.logon.LowPart);
    if(common_rpc_bind_local(endpoint,&client.binding)!=RPC_S_OK)goto done;
    client.process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    if(client.process)(void)common_rpc_observe_native_creation_bounded(&client,child,flags,&identity);
done:
    if(client.process)CloseHandle(client.process);
    if(client.binding)RpcBindingFree(&client.binding);
    SetLastError(saved);
}
/* Only a newly created, unreturned child belongs to this rollback. */
static BOOL finish(BOOL created,DWORD flags,PROCESS_INFORMATION *child,BOOL launcher)
{
    if(!created)return FALSE;
    DWORD saved=GetLastError(),error=ERROR_SUCCESS;
    nthook_context context=nthook_process_context;
    /* One redirected request, not a persistent descendant policy. */
    context.creation_flags=launcher ? flags & CREATE_SEPARATE_WOW_VDM : 0;
    {
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
    }
    if(!error)observe_created(child->hProcess,flags);
    if(!error && !(flags&CREATE_SUSPENDED) && ResumeThread(child->hThread)==(DWORD)-1)
        error=GetLastError();
    if(error) {
        TerminateProcess(child->hProcess,error);
        CloseHandle(child->hThread);CloseHandle(child->hProcess);
        ZeroMemory(child,sizeof(*child));SetLastError(error);return FALSE;
    }
    SetLastError(saved);return TRUE;
}
static BOOL has_extension(PCWSTR image)
{
    PCWSTR dot=wcsrchr(image,L'.'),slash=wcsrchr(image,L'\\');
    PCWSTR forward=wcsrchr(image,L'/');
    if(forward && (!slash || forward>slash))slash=forward;
    return dot && (!slash || dot>slash);
}
/* This is deliberately not common_resolve_application().  Run16 receives
 * one user CLI field and owns its CWD/PATH COM/EXE/BAT/PIF discovery.  A Hook
 * receives CreateProcess's two fields and must select only enough to decide
 * whether its caller requested a legacy image. */
static BOOL current_directory_image(PCWSTR image,BOOL append_exe,PWSTR output,DWORD capacity)
{
    WCHAR *candidate=NULL;DWORD count,attributes;size_t length=wcsnlen_s(image,NTHOOK_COMMAND_CAPACITY);
    PCWSTR suffix=append_exe && !has_extension(image) ? L".exe" : L"";
    size_t suffix_length=wcslen(suffix);
    PCWSTR source=image;
    if(!capacity || length==NTHOOK_COMMAND_CAPACITY || !length ||
       length+suffix_length>=capacity) {SetLastError(ERROR_FILENAME_EXCED_RANGE);return FALSE;}
    if(suffix_length) {
        candidate=(WCHAR *)HeapAlloc(GetProcessHeap(),0,(length+suffix_length+1u)*sizeof(WCHAR));
        if(!candidate){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return FALSE;}
        memcpy(candidate,image,length*sizeof(WCHAR));
        memcpy(candidate+length,suffix,(suffix_length+1u)*sizeof(WCHAR));
        source=candidate;
    }
    count=GetFullPathNameW(source,capacity,output,NULL);
    if(candidate)HeapFree(GetProcessHeap(),0,candidate);
    if(!count || count>=capacity)return FALSE;
    attributes=GetFileAttributesW(output);
    return attributes!=INVALID_FILE_ATTRIBUTES && !(attributes&FILE_ATTRIBUTE_DIRECTORY);
}
static BOOL selected_image(PCWSTR image,PWSTR output,DWORD capacity)
{
    DWORD count,attributes;
    if(current_directory_image(image,TRUE,output,capacity))return TRUE;
    /* A bare module name is resolved by Windows search.  An explicit
     * application suppresses command-line candidate parsing, not that OS
     * file-selection contract. */
    if(wcschr(image,L'\\') || wcschr(image,L'/') || wcschr(image,L':'))return FALSE;
    count=SearchPathW(NULL,image,has_extension(image) ? NULL : L".exe",
        capacity,output,NULL);
    if(!count || count>=capacity)return FALSE;
    attributes=GetFileAttributesW(output);
    return attributes!=INVALID_FILE_ATTRIBUTES && !(attributes&FILE_ATTRIBUTE_DIRECTORY);
}
static BOOL select_command_range(PCWSTR begin,PCWSTR end,PWSTR output,DWORD capacity)
{
    WCHAR *image;BOOL result;size_t length;
    if(!begin || !end || end<=begin)return FALSE;
    length=(size_t)(end-begin);
    if(length>=capacity){SetLastError(ERROR_FILENAME_EXCED_RANGE);return FALSE;}
    image=(WCHAR *)HeapAlloc(GetProcessHeap(),0,(length+1u)*sizeof(WCHAR));
    if(!image){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return FALSE;}
    memcpy(image,begin,length*sizeof(WCHAR));image[length]=0;
    result=selected_image(image,output,capacity);
    HeapFree(GetProcessHeap(),0,image);return result;
}
/* Bounded selection for the only shape where Windows has no separate
 * application field.  Each whitespace boundary is a documented unquoted
 * CreateProcess module candidate; a quoted first field stays one candidate. */
static BOOL select_command_image(PCWSTR command,PWSTR output,DWORD capacity)
{
    PCWSTR begin,end,limit;
    if(!command)return FALSE;
    limit=command+wcsnlen_s(command,NTHOOK_COMMAND_CAPACITY);
    if(limit==command+NTHOOK_COMMAND_CAPACITY) {SetLastError(ERROR_FILENAME_EXCED_RANGE);return FALSE;}
    begin=command;while(*begin==L' ' || *begin==L'\t')++begin;
    if(!*begin)return FALSE;
    if(*begin==L'"') {
        end=wcschr(begin+1,L'"');
        return end && end<limit && select_command_range(begin+1,end,output,capacity);
    }
    end=begin;
    for(;;) {
        while(*end && *end!=L' ' && *end!=L'\t')++end;
        if(select_command_range(begin,end,output,capacity))return TRUE;
        if(!*end)return FALSE;
        ++end;
    }
}
/* Exact tail selection shape retained from original OpenNT BaseCheckVDM:
 * src/opennt-host/base/win32/client/vdm.c.  The Hook emits a normal Run16
 * CLI; Run16 then calls the original implementation with the same selected
 * application/tail relationship. */
static PCWSTR opennt_vdm_tail(PCWSTR application,PCWSTR command)
{
    PCWSTR cursor;size_t length;
    if(!command || !*command)return L"";
    length=wcslen(application);
    if(command[0]==L'"') {
        cursor=command+1;while(*cursor && *cursor++!=L'"');
    } else if(length<wcslen(command) && !_wcsnicmp(application,command,length)) {
        cursor=command+length;
    } else {
        cursor=command;
        while(*cursor==L' ' || *cursor==L'\t')++cursor;
        if(*cursor==L'"') {++cursor;while(*cursor && *cursor++!=L'"');}
        else while(*cursor && *cursor!=L' ' && *cursor!=L'\t')++cursor;
    }
    while(*cursor==L' ' || *cursor==L'\t')++cursor;
    return cursor;
}
static BOOL legacy_application(PCWSTR application,PCWSTR command,DWORD flags,
    PWSTR resolved,DWORD capacity)
{
    DWORD type;
    resolved[0]=0;
    if(application) {
        if(!selected_image(application,resolved,capacity))return FALSE;
    } else if(!select_command_image(command,resolved,capacity))return FALSE;
    if(flags&(CREATE_NEW_CONSOLE|DETACHED_PROCESS|CREATE_NO_WINDOW))return FALSE;
    return !nthook_legacy_type(resolved,&type);
}
static WCHAR *redirect_command(PCWSTR application,PCWSTR command)
{
    WCHAR *text;PCWSTR tail=opennt_vdm_tail(application,command);size_t count;
    count=wcslen(nthook_process_context.launcher)+wcslen(application)+wcslen(tail)+7;
    if(count>32767){SetLastError(ERROR_FILENAME_EXCED_RANGE);return NULL;}
    text=(WCHAR *)HeapAlloc(GetProcessHeap(),0,count*sizeof(WCHAR));
    if(text)swprintf_s(text,count,L"\"%ls\" \"%ls\" %ls",nthook_process_context.launcher,application,tail);
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
    WCHAR *resolved=(WCHAR *)HeapAlloc(GetProcessHeap(),0,NTHOOK_COMMAND_CAPACITY*sizeof(WCHAR));
    BOOL redirect=resolved && legacy_application(application,command,flags,resolved,NTHOOK_COMMAND_CAPACITY);
    BOOL launcher=redirect;
    WCHAR *copy=redirect ? redirect_command(resolved,command) : NULL;
    BOOL result=FALSE;DWORD error;
    if(!redirect || copy) {
        /* Native CreateProcess receives the caller's original fields.  An
         * affirmative legacy redirect alone receives the selected image and
         * OpenNT-compatible tail encoded as Run16's ordinary CLI. */
        result=create_w(redirect ? nthook_process_context.launcher : application,
            redirect ? copy : command,process_attributes,thread_attributes,inherit,
            flags|CREATE_SUSPENDED,environment,directory,startup,process);
        result=finish(result,flags,process,launcher);
    }
    error=GetLastError();if(copy)HeapFree(GetProcessHeap(),0,copy);
    if(resolved)HeapFree(GetProcessHeap(),0,resolved);
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
    WCHAR *resolved=NULL;
    char launcher[MAX_PATH*2],*redirect=NULL;
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
    if(!error) {
        resolved=(WCHAR *)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,NTHOOK_COMMAND_CAPACITY*sizeof(WCHAR));
    }
    if(!error && resolved)is_legacy=legacy_application(wide_application,wide_command,flags,resolved,NTHOOK_COMMAND_CAPACITY);
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
    if(!error) {
        /* Actual creation remains ANSI, including the caller's environment,
         * directory and STARTUPINFO. Only an owned redirect line is encoded. */
        result=create_a(is_legacy ? launcher : application,
            is_legacy ? redirect : command,
            process_attributes,thread_attributes,inherit,flags|CREATE_SUSPENDED,
            environment,directory,startup,process);
        result=finish(result,flags,process,is_legacy);
        error=GetLastError();
    }
    if(redirect)HeapFree(GetProcessHeap(),0,redirect);
    if(wide_redirect)HeapFree(GetProcessHeap(),0,wide_redirect);
    if(resolved)HeapFree(GetProcessHeap(),0,resolved);
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
    if(!error)error=nthook_attach_titles();
    if(error){DetourTransactionAbort();return error;}
    return DetourTransactionCommit();
}
