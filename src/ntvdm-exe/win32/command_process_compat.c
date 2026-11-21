#include "compat/include/mvdm_command_native_child.h"
#include "compat/include/mvdm_command_launch_request.h"
#include "ntvdm-exe/command/include/command_launch.h"
#include "compat/include/command_process.h"
#include "lib-win32/process-launch/attributes.h"
#include "vdmapi.h"
#include "console_client.h"
#include "compat/include/console_io.h"
#include "compat/include/command_services.h"
#include "compat/include/system_root.h"
#include "common/native-hook/installer.h"

#include <stdio.h>
#include <string.h>
#include <wchar.h>

#undef SetStdHandle
#undef CreateProcess

static BOOL create_policy_child(LPCSTR application,LPSTR command,
    LPSECURITY_ATTRIBUTES process_attributes,LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit,DWORD flags,LPVOID environment,LPCSTR directory,
    LPSTARTUPINFOA startup,LPPROCESS_INFORMATION process,HANDLE frontend,HANDLE execution,
    BOOL explicit_handles)
{
    nthook_context context;
    win32_process_attributes attributes={0};
    STARTUPINFOEXA extended={0};
    HANDLE handles[5];
    BOOL created;
    DWORD error=nthook_context_paths(&context);
    if(error){SetLastError(error);return FALSE;}
    context.frontend=frontend;context.execution=execution;
    if(explicit_handles){
        handles[0]=frontend;handles[1]=execution;
        handles[2]=startup->hStdInput;handles[3]=startup->hStdOutput;handles[4]=startup->hStdError;
        error=win32_process_attributes_init(&attributes,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
            handles,sizeof(handles));
        if(error){win32_process_attributes_destroy(&attributes);SetLastError(error);return FALSE;}
        extended.StartupInfo=*startup;extended.StartupInfo.cb=sizeof(extended);
        extended.lpAttributeList=attributes.list;
        startup=&extended.StartupInfo;flags|=EXTENDED_STARTUPINFO_PRESENT;
    }
    created=CreateProcessA(application,command,process_attributes,thread_attributes,
        inherit,flags|CREATE_SUSPENDED,environment,directory,startup,process);
    error=GetLastError();
    win32_process_attributes_destroy(&attributes);
    if(!created){SetLastError(error);return FALSE;}
    /* The installer recognizes the pinned Run16 image and gives it a copied
     * launcher payload only. Native images receive their matching-width Hook.
     * Neither the user command tail nor its environment carries this policy. */
    error=nthook_install(process->hProcess,&context,NATIVE_HOOK_INTERCEPT);
    if(!error && !(flags&CREATE_SUSPENDED) && ResumeThread(process->hThread)==(DWORD)-1)
        error=GetLastError();
    if(error){
        TerminateProcess(process->hProcess,error);
        WaitForSingleObject(process->hProcess,INFINITE);
        CloseHandle(process->hThread);CloseHandle(process->hProcess);
        ZeroMemory(process,sizeof(*process));SetLastError(error);return FALSE;
    }
    return TRUE;
}

/* COMMAND owns its original native target and COMSPEC-tail construction.
 * The worker binding selects Run16 only for the latter service branch; it
 * never derives that choice from a file name or shell metacharacter. */

/* The locator is added only to the native child block, never guest memory.
 * The inherited handle is a restricted duplicate of the broker-proven root. */
static BOOL create_frontend_child(LPCSTR application,LPSTR command,
    LPSECURITY_ATTRIBUTES process_attributes,LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit,DWORD flags,LPVOID environment,LPCSTR directory,
    LPSTARTUPINFOA startup,LPPROCESS_INFORMATION process,const HANDLE *context,
    DWORD captured_console_mask,session *owner,const HANDLE streams[3],const DWORD stream_kinds[3])
{
    static const char *names[3]={"NTVDM_FRONTEND_CAPABILITY=","NTVDM_EXECUTION_CONSOLE=",CONSOLE_COMMAND_STREAMS_ENTRY};
    static const WCHAR *wide_names[3]={L"NTVDM_FRONTEND_CAPABILITY=",L"NTVDM_EXECUTION_CONSOLE=",CONSOLE_COMMAND_STREAMS_WENTRY};
    HANDLE capabilities[2]={NULL,NULL};
    HANDLE endpoint_streams[3]={NULL,NULL,NULL};
    STARTUPINFOA child_startup=*startup;
    BOOL wide=(flags&CREATE_UNICODE_ENVIRONMENT)!=0,result;
    void *inherited=NULL,*copy=NULL;
    const BYTE *cursor;
    BYTE *destination;
    size_t unit=wide ? sizeof(WCHAR) : 1,bytes=unit,length,entry_bytes,index,slot;
    char value[3][80];
    WCHAR wide_value[3][80];
    DWORD error,console_mask=0;
    if(context){
        for(slot=0;slot<2;++slot)
            if(!DuplicateHandle(GetCurrentProcess(),context[slot],GetCurrentProcess(),
                &capabilities[slot],SYNCHRONIZE,TRUE,0)){
                error=GetLastError();result=FALSE;goto done;
            }
    }else if (!ntvdm_console_inherit_launch_capabilities(owner,&capabilities[0],&capabilities[1])) return FALSE;
    if (!capabilities[0]) return create_policy_child(application,command,process_attributes,
        thread_attributes,inherit,flags,environment,directory,startup,process,NULL,NULL,FALSE);
    if (!inherit) { error=ERROR_INVALID_PARAMETER;result=FALSE;goto done; }
    for (slot=0;slot<2;++slot)
        sprintf_s(value[slot],sizeof(value[slot]),"%s%lx",names[slot],(unsigned long)(ULONG_PTR)capabilities[slot]);
    for(slot=0;slot<3;++slot) {
        HANDLE stream=context ?
            (slot==0 ? startup->hStdInput : slot==1 ? startup->hStdOutput : startup->hStdError) :
            streams[slot];
        DWORD kind=context ? ((captured_console_mask&(1u<<slot)) ? (slot==0 ? 1u : 2u) : 0u) :
            stream_kinds[slot];
        if(kind) {
            if(kind!=(slot==0 ? 1u : 2u)){error=ERROR_INVALID_HANDLE;result=FALSE;goto done;}
            if(!DuplicateHandle(GetCurrentProcess(),stream,GetCurrentProcess(),
                &endpoint_streams[slot],SYNCHRONIZE,TRUE,0)){error=GetLastError();result=FALSE;goto done;}
            stream=endpoint_streams[slot];
            console_mask|=1u<<slot;
        }else if(context){
            if(!DuplicateHandle(GetCurrentProcess(),stream,GetCurrentProcess(),
                &endpoint_streams[slot],0,TRUE,DUPLICATE_SAME_ACCESS)){
                error=GetLastError();result=FALSE;goto done;
            }
            stream=endpoint_streams[slot];
        }
        if(slot==0)child_startup.hStdInput=stream;
        else if(slot==1)child_startup.hStdOutput=stream;
        else child_startup.hStdError=stream;
    }
    child_startup.dwFlags|=STARTF_USESTDHANDLES;
    sprintf_s(value[2],sizeof(value[2]),"%s%lu",names[2],console_mask);
    for(slot=0;slot<3;++slot)
        for(index=0;index<=strlen(value[slot]);++index)wide_value[slot][index]=(WCHAR)value[slot][index];
    if (!environment) {
        inherited=wide ? (void *)GetEnvironmentStringsW() : (void *)GetEnvironmentStringsA();
        if (!inherited) { error=GetLastError();result=FALSE;goto done; }
        environment=inherited;
    }
    for (cursor=environment;(length=wide ? wcslen((const WCHAR *)cursor) : strlen((const char *)cursor))!=0;
         cursor+=(length+1)*unit) bytes+=(length+1)*unit;
    for (slot=0;slot<3;++slot) bytes+=(strlen(value[slot])+1)*unit;
    copy=HeapAlloc(GetProcessHeap(),0,bytes);
    if (!copy) { error=ERROR_NOT_ENOUGH_MEMORY;result=FALSE;goto done; }
    destination=copy;
    for (cursor=environment;(length=wide ? wcslen((const WCHAR *)cursor) : strlen((const char *)cursor))!=0;
         cursor+=(length+1)*unit) {
        for (slot=0;slot<3;++slot)
            if (wide ? !_wcsnicmp((const WCHAR *)cursor,wide_names[slot],strlen(names[slot])) :
                       !_strnicmp((const char *)cursor,names[slot],strlen(names[slot]))) break;
        if (slot<3) continue;
        entry_bytes=(length+1)*unit;
        memcpy(destination,cursor,entry_bytes);destination+=entry_bytes;
    }
    for (slot=0;slot<3;++slot) {
        entry_bytes=(strlen(value[slot])+1)*unit;
        memcpy(destination,wide ? (const void *)wide_value[slot] : (const void *)value[slot],entry_bytes);
        destination+=entry_bytes;
    }
    ZeroMemory(destination,unit);
    /* This is the authenticated inner launcher, not a Console frontend.
     * A detached worker must not cause Windows to allocate a new Console
     * merely because the launcher is a Console-subsystem executable. */
    if (!(flags&(CREATE_NEW_CONSOLE|CREATE_NO_WINDOW))) flags|=DETACHED_PROCESS;
    result=create_policy_child(application,command,process_attributes,thread_attributes,
        inherit,flags,copy,directory,&child_startup,process,capabilities[0],capabilities[1],context!=NULL);
    error=GetLastError();
done:
    for(slot=0;slot<3;++slot)if(endpoint_streams[slot])CloseHandle(endpoint_streams[slot]);
    if (copy) HeapFree(GetProcessHeap(),0,copy);
    if (inherited) {
        if (wide) FreeEnvironmentStringsW(inherited);else FreeEnvironmentStringsA(inherited);
    }
    for(slot=0;slot<2;++slot)if(capabilities[slot])CloseHandle(capabilities[slot]);
    SetLastError(error);
    return result;
}

static BOOL opennt_command_launch_vdm_child(
    const char *command_line,
    LPSECURITY_ATTRIBUTES process_attributes,
    LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit_handles,
    DWORD creation_flags,
    LPVOID environment,
    LPCSTR current_directory,
    LPSTARTUPINFOA startup_info,
    LPPROCESS_INFORMATION process_information,unsigned int service,
    const char *service_tail,const HANDLE *context,DWORD console_mask,
    session *owner,const HANDLE streams[3],const DWORD stream_kinds[3])
{
    char launcher[MAX_PATH];
    char child_command[MAX_PATH * 2u + MAXIMUM_VDM_COMMAND_LENGTH + 16u];
    DWORD error;
    int formatted;

    if (command_line == NULL || *command_line == '\0') {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    error = compat_product_path_a(L"system32\\run16.exe", launcher, sizeof(launcher));
    if (error) { SetLastError(error); return FALSE; }
    if (service == MVDM_COMMAND_SERVICE_INTERACTIVE)
        /* SVC_EXECCOMSPEC32 selects the host COMSPEC, not guest COMMAND.COM.
         * Preserve the captured executable, including spaces, through Run16. */
        formatted = snprintf(child_command, sizeof(child_command), "\"%s\" \"%s\"", launcher, command_line);
    else if (service == MVDM_COMMAND_SERVICE_EXECUTE && service_tail != NULL)
        formatted = snprintf(child_command, sizeof(child_command),
            "\"%s\" %s", launcher, service_tail);
    else
        formatted = snprintf(child_command, sizeof(child_command), "\"%s\" %s", launcher, command_line);
    if (formatted < 0 || (size_t)formatted >= sizeof(child_command)) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    return create_frontend_child(NULL, child_command, process_attributes,
        thread_attributes, inherit_handles, creation_flags, environment,
        current_directory, startup_info, process_information,context,console_mask,owner,streams,stream_kinds);
}

void ntvdm_command_start_original(session *owner, compat_command_child_request *request)
{
    if (request->host && (!owner || request->host != &owner->host_context)) {
        request->error = ERROR_INVALID_STATE;
    } else if (!request->application && request->command && *request->command) {
        request->created = opennt_command_launch_vdm_child(request->command,
            request->process_attributes,request->thread_attributes,request->inherit,
            request->flags,request->environment,request->directory,request->startup,
            request->process,request->service,request->service_tail,NULL,0,
            request->host ? owner : NULL,request->streams,request->stream_kinds);
    } else {
        request->created = create_frontend_child(request->application,request->command,
            request->process_attributes,request->thread_attributes,request->inherit,
            request->flags,request->environment,request->directory,request->startup,
            request->process,NULL,0,request->host ? owner : NULL,request->streams,request->stream_kinds);
    }
    if (!request->created && !request->error) request->error = GetLastError();
}

DWORD ntvdm_command_launch_start(ntvdm_command_launch *request,HANDLE frontend,
    HANDLE execution,DWORD flags,
    PROCESS_INFORMATION *out)
{
    STARTUPINFOA startup={sizeof(startup)};
    HANDLE context[2]={frontend,execution};
    if(out)ZeroMemory(out,sizeof(*out));
    if(!request || !out || !frontend || !execution ||
        frontend==INVALID_HANDLE_VALUE || execution==INVALID_HANDLE_VALUE ||
        (flags&~(CREATE_SUSPENDED|CREATE_DEFAULT_ERROR_MODE)))
        return ERROR_INVALID_PARAMETER;
    if(InterlockedExchange(&request->started,1))return ERROR_INVALID_STATE;
    startup.dwFlags=STARTF_USESTDHANDLES;
    startup.hStdInput=request->streams[0];startup.hStdOutput=request->streams[1];
    startup.hStdError=request->streams[2];
    return opennt_command_launch_vdm_child(request->command,NULL,NULL,TRUE,flags,
        request->environment,request->directory,&startup,out,request->service,
        request->command,context,request->console_mask,NULL,NULL,NULL) ? ERROR_SUCCESS : GetLastError();
}
