#include "command_process_compat.h"
#include "vdmapi.h"
#include "console_client.h"
#include "common/protocol/console_io.h"
#include "common/system_root.h"

#include <stdio.h>
#include <string.h>
#include <wchar.h>

#undef SetStdHandle
#undef CreateProcess

typedef struct opennt_command_standard_handles {
    unsigned int initialized;
    unsigned int overridden[3];
    HANDLE values[3];
} opennt_command_standard_handles;

static __declspec(thread) opennt_command_standard_handles current_handles;

/* COMMAND's original cmdExec has already selected the full process command:
 * either its native target or `%COMSPEC% /c <tail>`.  Preserve that command
 * unchanged.  run16/NTVWM creates the selected host process and injects
 * NTHOOK before it runs; host CMD then owns built-ins, batch, quotes, pipes
 * and redirection, while Hook returns only actual DOS/Win16 children. */

/* The locator is added only to the native child block, never guest memory.
 * The inherited handle is a restricted duplicate of the broker-proven root. */
static BOOL create_frontend_child(LPCSTR application,LPSTR command,
    LPSECURITY_ATTRIBUTES process_attributes,LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit,DWORD flags,LPVOID environment,LPCSTR directory,
    LPSTARTUPINFOA startup,LPPROCESS_INFORMATION process)
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
    if (!ntvdm_console_inherit_launch_capabilities(&capabilities[0],&capabilities[1])) return FALSE;
    if (!capabilities[0]) return CreateProcessA(application,command,process_attributes,
        thread_attributes,inherit,flags,environment,directory,startup,process);
    if (!inherit) { error=ERROR_INVALID_PARAMETER;result=FALSE;goto done; }
    for (slot=0;slot<2;++slot) {
        sprintf_s(value[slot],sizeof(value[slot]),"%s%lx",names[slot],(unsigned long)(ULONG_PTR)capabilities[slot]);
    }
    for(slot=0;slot<3;++slot) {
        HANDLE stream=(startup->dwFlags&STARTF_USESTDHANDLES) ?
            (slot==0 ? startup->hStdInput : slot==1 ? startup->hStdOutput : startup->hStdError) :
            GetStdHandle(slot==0 ? STD_INPUT_HANDLE : slot==1 ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
        DWORD kind=ntvdm_console_handle_kind(stream);
        if(kind) {
            if(kind!=(slot==0 ? 1u : 2u)){error=ERROR_INVALID_HANDLE;result=FALSE;goto done;}
            if(!DuplicateHandle(GetCurrentProcess(),stream,GetCurrentProcess(),
                &endpoint_streams[slot],SYNCHRONIZE,TRUE,0)){error=GetLastError();result=FALSE;goto done;}
            stream=endpoint_streams[slot];
            console_mask|=1u<<slot;
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
    result=CreateProcessA(application,command,process_attributes,thread_attributes,
        inherit,flags,copy,directory,&child_startup,process);
    error=GetLastError();
done:
    for(slot=0;slot<3;++slot)if(endpoint_streams[slot])CloseHandle(endpoint_streams[slot]);
    if (copy) HeapFree(GetProcessHeap(),0,copy);
    if (inherited) {
        if (wide) FreeEnvironmentStringsW(inherited);else FreeEnvironmentStringsA(inherited);
    }
    CloseHandle(capabilities[0]);CloseHandle(capabilities[1]);SetLastError(error);
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
    LPPROCESS_INFORMATION process_information)
{
    char launcher[MAX_PATH];
    char child_command[MAX_PATH * 2u + MAXIMUM_VDM_COMMAND_LENGTH + 8u];
    DWORD error;
    int formatted;

    if (command_line == NULL || *command_line == '\0') {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    error = common_product_path_a(L"system32\\run16.exe", launcher, sizeof(launcher));
    if (error) {
        SetLastError(error);
        return FALSE;
    }
    formatted = snprintf(child_command, sizeof(child_command),
        "\"%s\" %s", launcher, command_line);
    if (formatted < 0 || (size_t)formatted >= sizeof(child_command)) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    return create_frontend_child(NULL, child_command, process_attributes,
        thread_attributes, inherit_handles, creation_flags, environment,
        current_directory, startup_info, process_information);
}

static int opennt_command_standard_handle_index(DWORD standard_handle)
{
    switch (standard_handle) {
    case STD_INPUT_HANDLE: return 0;
    case STD_OUTPUT_HANDLE: return 1;
    case STD_ERROR_HANDLE: return 2;
    default: return -1;
    }
}

static void opennt_command_initialize_standard_handles(void)
{
    if (current_handles.initialized != 0u) return;
    current_handles.values[0] = GetStdHandle(STD_INPUT_HANDLE);
    current_handles.values[1] = GetStdHandle(STD_OUTPUT_HANDLE);
    current_handles.values[2] = GetStdHandle(STD_ERROR_HANDLE);
    current_handles.initialized = 1u;
}

BOOL opennt_command_set_std_handle(DWORD standard_handle, HANDLE handle)
{
    int index = opennt_command_standard_handle_index(standard_handle);
    if (index < 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    opennt_command_initialize_standard_handles();
    current_handles.values[index] = handle;
    current_handles.overridden[index] = 1u;
    return TRUE;
}

BOOL opennt_command_create_process_a(
    LPCSTR application_name,
    LPSTR command_line,
    LPSECURITY_ATTRIBUTES process_attributes,
    LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit_handles,
    DWORD creation_flags,
    LPVOID environment,
    LPCSTR current_directory,
    LPSTARTUPINFOA startup_info,
    LPPROCESS_INFORMATION process_information)
{
    STARTUPINFOA local_startup;
    LPSTARTUPINFOA effective_startup;
    int use_child_streams;

    if (startup_info == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    opennt_command_initialize_standard_handles();
    use_child_streams = current_handles.overridden[0] != 0u ||
        current_handles.overridden[1] != 0u ||
        current_handles.overridden[2] != 0u;
    effective_startup = startup_info;
    if (use_child_streams) {
        local_startup = *startup_info;
        local_startup.dwFlags |= STARTF_USESTDHANDLES;
        local_startup.hStdInput = current_handles.values[0];
        local_startup.hStdOutput = current_handles.values[1];
        local_startup.hStdError = current_handles.values[2];
        effective_startup = &local_startup;
    }
    /* Original cmdCreateProcess calls CreateProcess(NULL, pCommand32); the
     * command text has already been selected by COMMAND. */
    if (application_name == NULL && command_line != NULL && *command_line) {
        return opennt_command_launch_vdm_child(command_line,
            process_attributes, thread_attributes, inherit_handles,
            creation_flags, environment, current_directory, effective_startup,
            process_information);
    }
    return create_frontend_child(application_name, command_line,
        process_attributes, thread_attributes, inherit_handles,
        creation_flags, environment, current_directory, effective_startup,
        process_information);
}
