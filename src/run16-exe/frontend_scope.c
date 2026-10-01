#include "frontend_scope.h"
#include "worker_launch.h"
#include "ntkvm-exe/bootstrap.h"
#include "ntkvm-exe/native_request_client.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "interface/console_io.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

/* A local handle locator, never a broker identity or authorization token. */
#define FRONTEND_ENV "NTVDM_FRONTEND_CAPABILITY"
#define EXECUTION_ENV "NTVDM_EXECUTION_CONSOLE"
static DWORD inherited_capability(const char *name,HANDLE *capability)
{
    char text[32],*end;
    DWORD count;
    ULONG_PTR value;
    *capability=NULL;
    count=GetEnvironmentVariableA(name,text,sizeof(text));
    if (!count) return GetLastError()==ERROR_ENVVAR_NOT_FOUND ? ERROR_SUCCESS : ERROR_INVALID_DATA;
    if (count>=sizeof(text)) return ERROR_INVALID_DATA;
    value=(ULONG_PTR)strtoul(text,&end,16);
    if (!value || end==text || *end) return ERROR_INVALID_DATA;
    *capability=(HANDLE)value;
    return ERROR_SUCCESS;
}
struct run16_frontend_scope {
    HANDLE capability,retire,restored,root,receipt,completion,worker;
    BOOL owns_environment,has_execution;
    DWORD console_mask;
};
void run16_frontend_scope_end(run16_frontend_scope *scope)
{
    if(!scope)return;
    if(scope->owns_environment)SetEnvironmentVariableA(FRONTEND_ENV,NULL);
    if(scope->capability)CloseHandle(scope->capability);
    if(scope->retire)CloseHandle(scope->retire);
    if(scope->restored)CloseHandle(scope->restored);
    if(scope->root)CloseHandle(scope->root);
    if(scope->receipt)CloseHandle(scope->receipt);
    if(scope->completion)CloseHandle(scope->completion);
    if(scope->worker)CloseHandle(scope->worker);
    HeapFree(GetProcessHeap(),0,scope);
}
DWORD run16_frontend_scope_begin(run16_frontend_scope **output)
{
    run16_frontend_scope *scope;
    char text[32];
    DWORD error=ERROR_SUCCESS,generation;
    HANDLE inherited_frontend=NULL,inherited_execution=NULL;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output=NULL;
    error=inherited_capability(FRONTEND_ENV,&inherited_frontend);
    if (!error) error=inherited_capability(EXECUTION_ENV,&inherited_execution);
    if (error) return error;
    /* An orphan execution locator cannot promote this launcher to root. */
    if (inherited_execution && !inherited_frontend) return ERROR_INVALID_DATA;
    scope=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*scope));
    if (!scope) return ERROR_NOT_ENOUGH_MEMORY;
    if (inherited_frontend) {
        /* The broker first authenticates a worker-local execution context,
         * when present, then verifies that its owner is the retained root. */
        if (inherited_execution) {
            error=OpenNtBaseClientBindConsoleContext(inherited_execution);
            if (error) goto fail;
            scope->has_execution=TRUE;
        }
        error=OpenNtBaseClientRetainFrontendRoot(inherited_frontend,&scope->root,&generation);
        if (error) goto fail;
        if (!DuplicateHandle(GetCurrentProcess(),inherited_frontend,GetCurrentProcess(),
            &scope->capability,SYNCHRONIZE,FALSE,0)) { error=GetLastError();goto fail; }
    } else {
        WCHAR image[MAX_PATH],*slash;
        DWORD length=GetModuleFileNameW(NULL,image,ARRAYSIZE(image));
        frontend_connection connection={0};
        if(!length || length>=ARRAYSIZE(image) || !(slash=wcsrchr(image,L'\\'))){error=ERROR_BAD_PATHNAME;goto fail;}
        if(wcscpy_s(slash+1,ARRAYSIZE(image)-(size_t)(slash+1-image),L"ntkvm.exe")){error=ERROR_FILENAME_EXCED_RANGE;goto fail;}
        error=frontend_bootstrap_start(image,&connection);if(error)goto fail;
        scope->capability=connection.capability;connection.capability=NULL;
        scope->retire=connection.retire;connection.retire=NULL;
        scope->restored=connection.restored;connection.restored=NULL;
        scope->root=connection.process;connection.process=NULL;
        frontend_bootstrap_release(&connection);
        sprintf_s(text,sizeof(text),"%lx",(unsigned long)(uintptr_t)scope->capability);
        if (!SetEnvironmentVariableA(FRONTEND_ENV,text)) { error=GetLastError();goto fail; }
        scope->owns_environment=TRUE;
    }
    /* This describes streams, never grants membership. Only an authenticated
     * execution context can supply worker-local endpoints. Do not turn an
     * actual redirected file/pipe into Console I/O, and consume the one-hop
     * metadata before constructing any target environment. */
    {
        DWORD count=GetEnvironmentVariableA(CONSOLE_COMMAND_STREAMS_ENV,text,sizeof(text)),i,flags;
        if(count) {
            if(!SetEnvironmentVariableA(CONSOLE_COMMAND_STREAMS_ENV,NULL)){error=GetLastError();goto fail;}
            if(count!=1 || text[0]<'0' || text[0]>'7' || !scope->has_execution){error=ERROR_INVALID_DATA;goto fail;}
            scope->console_mask=(DWORD)(text[0]-'0');
            for(i=0;i<3;++i)if(scope->console_mask&(1u<<i)) {
                HANDLE stream=GetStdHandle(i==0 ? STD_INPUT_HANDLE : i==1 ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
                if(!GetHandleInformation(stream,&flags) || GetFileType(stream)!=FILE_TYPE_UNKNOWN){error=ERROR_INVALID_HANDLE;goto fail;}
            }
        }else if(GetLastError()!=ERROR_ENVVAR_NOT_FOUND){error=ERROR_INVALID_DATA;goto fail;}
    }
    *output=scope;
    return ERROR_SUCCESS;
fail:
    run16_frontend_scope_end(scope);
    return error;
}

HANDLE run16_frontend_scope_capability(run16_frontend_scope *scope)
{
    return scope ? scope->capability : NULL;
}

BOOL run16_frontend_scope_has_execution(run16_frontend_scope *scope)
{
    return scope && scope->has_execution;
}

DWORD run16_frontend_scope_console_mask(run16_frontend_scope *scope)
{
    return scope ? scope->console_mask : 0;
}

static DWORD wait_worker_change(HANDLE changed,HANDLE worker,HANDLE root,ULONGLONG deadline)
{
    HANDLE waits[3];DWORD count=0,worker_index=MAXDWORD,root_index,wait,remaining;
    ULONGLONG now=GetTickCount64();
    if(now>=deadline)return ERROR_TIMEOUT;
    remaining=(DWORD)(deadline-now);
    if(worker){worker_index=count;waits[count++]=worker;}
    root_index=count;waits[count++]=root;
    waits[count++]=changed;
    wait=WaitForMultipleObjects(count,waits,FALSE,remaining);
    if(wait==WAIT_TIMEOUT)return ERROR_TIMEOUT;
    if(wait==WAIT_FAILED)return GetLastError();
    if(worker && wait==WAIT_OBJECT_0+worker_index)return ERROR_PROCESS_ABORTED;
    if(wait==WAIT_OBJECT_0+root_index)return ERROR_PIPE_NOT_CONNECTED;
    return wait==WAIT_OBJECT_0+count-1 ? ERROR_SUCCESS : ERROR_INVALID_STATE;
}

DWORD run16_frontend_scope_launch_native(run16_frontend_scope *scope,const run16_native_start *start,HANDLE *target)
{
    HANDLE worker=NULL,changed=NULL;DWORD error;
    ULONGLONG deadline=GetTickCount64()+10000;
    if(!scope || !start || !target)return ERROR_INVALID_PARAMETER;
    if(scope->receipt)return ERROR_BUSY;
    *target=NULL;
    error=OpenNtBaseClientWorkerStateChanged(&changed);
    if(error)return error;
    for(;;) {
        uint64_t reservation=0;
        error=OpenNtBaseClientSelectNativeWorker(&worker);
        if(error!=ERROR_NOT_FOUND)break;
        error=OpenNtBaseClientReserveNativeWorker(&reservation);
        if(error==ERROR_ALREADY_EXISTS) {
            /* Each launcher has its own auto-reset state event; NTKVM's
             * retirement event is never shared with this admission wait. */
            error=wait_worker_change(changed,NULL,scope->root,deadline);
            if(!error)continue;
            break;
        }
        if(!error) {
            WCHAR image[MAX_PATH],command[MAX_PATH+3],*slash;
            STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};
            DWORD length=GetModuleFileNameW(NULL,image,ARRAYSIZE(image));
            if(!length || length>=ARRAYSIZE(image) || !(slash=wcsrchr(image,L'\\')))error=ERROR_BAD_PATHNAME;
            else if(wcscpy_s(slash+1,ARRAYSIZE(image)-(size_t)(slash+1-image),L"ntcon.exe"))error=ERROR_FILENAME_EXCED_RANGE;
            else if(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\"",image)<0)error=ERROR_FILENAME_EXCED_RANGE;
            else {
                startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
                error=run16_worker_prepare(reservation,image,command,NULL,CREATE_NEW_CONSOLE,&startup,&process);
                if(!error && ResumeThread(process.hThread)==(DWORD)-1) {
                    error=GetLastError();TerminateProcess(process.hProcess,error);
                    WaitForSingleObject(process.hProcess,INFINITE);
                }
            }
            if(process.hThread)CloseHandle(process.hThread);
            if(!error){worker=process.hProcess;process.hProcess=NULL;}
            if(process.hProcess)CloseHandle(process.hProcess);
            if(error)(void)OpenNtBaseClientReleaseWorker(reservation);
        }
        break;
    }
    if(error)goto done;
    for(;;) {
        /* Both worker kinds acquire the same authenticated presentation route.
         * A reused route is not a new frontend or an input activation. */
        error=OpenNtBaseClientRequestFrontend(scope->capability);
        if(error==ERROR_ALREADY_EXISTS)error=ERROR_SUCCESS;
        if(!error)error=run16_native_worker_request_begin(worker,scope->capability,start,target,&scope->receipt,&scope->completion);
        if(error!=ERROR_NOT_READY)break;
        /* No request was accepted. Wait only for initial registration; never
         * replay a submitted request or restart a failed worker. */
        error=wait_worker_change(changed,worker,scope->root,deadline);
        if(error)break;
    }
    if(!error)scope->worker=worker;
    else CloseHandle(worker);
done:
    CloseHandle(changed);
    return error;
}
DWORD run16_frontend_scope_wait_native(run16_frontend_scope *scope,HANDLE target,DWORD *result)
{
    DWORD error=0,wait;
    if(!scope || !target || !result)return ERROR_INVALID_PARAMETER;
    if(scope->receipt){
        HANDLE waits[3]={target,scope->worker,scope->root};
        /* A live target does not prove that its worker can still complete the
         * request. Observe the authenticated endpoints without owning or
         * terminating the handed-off target's execution lifetime. */
        wait=WaitForMultipleObjects(3,waits,FALSE,INFINITE);
        if(wait!=WAIT_OBJECT_0)return wait==WAIT_OBJECT_0+1 ? ERROR_PROCESS_ABORTED :
            wait==WAIT_OBJECT_0+2 ? ERROR_PIPE_NOT_CONNECTED : GetLastError();
    }else if(WaitForSingleObject(target,INFINITE)!=WAIT_OBJECT_0)return GetLastError();
    if(!GetExitCodeProcess(target,result))return GetLastError();
    if(scope->receipt){
        /* Reuse the authenticated request stream: the worker reports final
         * presentation status before DOS can resume. Peer/root death cancels
         * the read; a timeout must never silently authorize handoff. */
        error=run16_native_worker_request_finish(scope->completion,scope->worker,scope->root);
        CloseHandle(scope->receipt);scope->receipt=NULL;
        CloseHandle(scope->completion);scope->completion=NULL;
        CloseHandle(scope->worker);scope->worker=NULL;
    }
    return error;
}
DWORD run16_frontend_scope_resume_parent(run16_frontend_scope *scope)
{
    DWORD capacity=16,count,index,error=0,*members=NULL;
    HANDLE worker=NULL;
    if(!scope || !scope->has_execution)return 0;
    /* Actual attachment distinguishes a native caller from a DOS-side
     * launcher. This is a routing check only; broker selection and the
     * authenticated completion receipt still grant endpoint authority. */
    for(;;) {
        members=HeapAlloc(GetProcessHeap(),0,capacity*sizeof(*members));
        if(!members)return ERROR_NOT_ENOUGH_MEMORY;
        count=GetConsoleProcessList(members,capacity);
        if(!count) {
            error=GetLastError();
            if(error==ERROR_INVALID_HANDLE)error=0;
            goto done;
        }
        if(count<=capacity)break;
        HeapFree(GetProcessHeap(),0,members);members=NULL;
        if(count>65536)return ERROR_BUFFER_OVERFLOW;
        capacity=count;
    }
    error=OpenNtBaseClientSelectNativeWorker(&worker);
    if(error==ERROR_NOT_FOUND){error=0;goto done;}
    if(error)goto done;
    for(index=0;index<count;++index)if(members[index]==GetProcessId(worker))break;
    if(index<count)error=run16_native_worker_request_resume(worker,scope->capability);
done:
    if(worker)CloseHandle(worker);
    if(members)HeapFree(GetProcessHeap(),0,members);
    return error;
}

DWORD run16_frontend_scope_restore_parent(run16_frontend_scope *scope)
{
    HANDLE waits[2];
    DWORD wait,status=ERROR_GEN_FAILURE;
    /* Only a root launcher can return an outer CMD to this Console.  An
     * inherited scope is an inner invocation and must never hold its root's
     * frontend lifetime. */
    if(!scope || !scope->owns_environment || !scope->restored)return ERROR_SUCCESS;
    waits[0]=scope->restored;waits[1]=scope->root;
    wait=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
    if(wait==WAIT_OBJECT_0)return ERROR_SUCCESS;
    if(wait==WAIT_OBJECT_0+1) {
        if(!GetExitCodeProcess(scope->root,&status))status=GetLastError();
        return status ? status : ERROR_GEN_FAILURE;
    }
    return GetLastError();
}

DWORD run16_frontend_scope_retire(run16_frontend_scope *scope)
{
    if(!scope || !scope->owns_environment || !scope->retire)return ERROR_SUCCESS;
    return SetEvent(scope->retire) ? ERROR_SUCCESS : GetLastError();
}
