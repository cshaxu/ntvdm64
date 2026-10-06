#include "frontend_scope.h"
#include "run16-exe/frontend_bootstrap.h"
#include "run16-exe/native_request_client.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "common/protocol/console_io.h"
#include "nthook32-dll/hook.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <errno.h>

/* A local handle locator, never a broker identity or authorization token. */
#define FRONTEND_ENV "NTVDM_FRONTEND_CAPABILITY"
#define EXECUTION_ENV "NTVDM_EXECUTION_CONSOLE"
static DWORD inherited_capability(const char *name,HANDLE *capability)
{
    char text[32],*end;
    DWORD count;
    unsigned long long value;
    *capability=NULL;
    count=GetEnvironmentVariableA(name,text,sizeof(text));
    if (!count) return GetLastError()==ERROR_ENVVAR_NOT_FOUND ? ERROR_SUCCESS : ERROR_INVALID_DATA;
    if (count>=sizeof(text)) return ERROR_INVALID_DATA;
    errno=0;
    value=_strtoui64(text,&end,16);
    if (!value || end==text || *end || errno==ERANGE ||
        text[0]=='-' || value>(uint64_t)UINTPTR_MAX) return ERROR_INVALID_DATA;
    *capability=(HANDLE)(ULONG_PTR)value;
    return ERROR_SUCCESS;
}
struct run16_frontend_scope {
    HANDLE capability,restored,root,receipt;
    BOOL owns_environment,has_execution;
    DWORD console_mask,native_request;
};
void run16_frontend_scope_end(run16_frontend_scope *scope)
{
    if(!scope)return;
    if(scope->owns_environment)SetEnvironmentVariableA(FRONTEND_ENV,NULL);
    if(scope->capability)CloseHandle(scope->capability);
    if(scope->restored)CloseHandle(scope->restored);
    if(scope->root)CloseHandle(scope->root);
    if(scope->receipt)CloseHandle(scope->receipt);
    HeapFree(GetProcessHeap(),0,scope);
}
static DWORD scope_begin(run16_frontend_scope **output,BOOL lease,BOOL console_owned,BOOL acquire_frontend)
{
    run16_frontend_scope *scope;
    char text[32];
    DWORD error=ERROR_SUCCESS,generation;
    HANDLE inherited_frontend=NULL,inherited_execution=NULL;
    nthook_context seed;BOOL seeded=FALSE;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output=NULL;
    error=nthook_context_read(&seed,&seeded);
    if(!error && seeded) {
        if(seed.mode!=NATIVE_HOOK_LAUNCHER)error=ERROR_INVALID_DATA;
        else { inherited_frontend=seed.frontend;inherited_execution=seed.execution; }
    } else if(!error) {
        error=inherited_capability(FRONTEND_ENV,&inherited_frontend);
        if (!error) error=inherited_capability(EXECUTION_ENV,&inherited_execution);
    }
    if (error) return error;
    /* An orphan execution locator cannot promote this launcher to root. */
    if (inherited_execution && !inherited_frontend) return ERROR_INVALID_DATA;
    scope=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*scope));
    if (!scope) {
        if(seeded) {
            if(inherited_frontend)CloseHandle(inherited_frontend);
            if(inherited_execution)CloseHandle(inherited_execution);
        }
        return ERROR_NOT_ENOUGH_MEMORY;
    }
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
    } else if(acquire_frontend) {
        frontend_connection connection={0};
        uint64_t console_window=(uint64_t)(UINT_PTR)GetConsoleWindow();
        error=OpenNtBaseClientStartFrontend(console_window,lease && !console_owned,
            &connection.process,&connection.capability,&connection.restored);
        if(error)goto fail;
        scope->capability=connection.capability;connection.capability=NULL;
        scope->restored=connection.restored;connection.restored=NULL;
        scope->root=connection.process;connection.process=NULL;
        frontend_bootstrap_release(&connection);
        sprintf_s(text,sizeof(text),"%llx",(unsigned long long)(uintptr_t)scope->capability);
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
    if(seeded) {
        if(inherited_frontend)CloseHandle(inherited_frontend);
        if(inherited_execution)CloseHandle(inherited_execution);
    }
    *output=scope;
    return ERROR_SUCCESS;
fail:
    if(seeded) {
        if(inherited_frontend)CloseHandle(inherited_frontend);
        if(inherited_execution)CloseHandle(inherited_execution);
    }
    run16_frontend_scope_end(scope);
    return error;
}
DWORD run16_frontend_scope_begin(run16_frontend_scope **output)
{ return scope_begin(output,FALSE,FALSE,TRUE); }
DWORD run16_frontend_scope_begin_lease(run16_frontend_scope **output,BOOL console_owned)
{ return scope_begin(output,TRUE,console_owned,TRUE); }
DWORD run16_frontend_scope_begin_gui(run16_frontend_scope **output)
{ return scope_begin(output,FALSE,FALSE,FALSE); }

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
    HANDLE waits[3];DWORD count=0,worker_index=MAXDWORD,root_index=MAXDWORD,wait,remaining;
    ULONGLONG now=GetTickCount64();
    if(now>=deadline)return ERROR_TIMEOUT;
    remaining=(DWORD)(deadline-now);
    if(worker){worker_index=count;waits[count++]=worker;}
    if(root){root_index=count;waits[count++]=root;}
    waits[count++]=changed;
    wait=WaitForMultipleObjects(count,waits,FALSE,remaining);
    if(wait==WAIT_TIMEOUT)return ERROR_TIMEOUT;
    if(wait==WAIT_FAILED)return GetLastError();
    if(worker && wait==WAIT_OBJECT_0+worker_index)return ERROR_PROCESS_ABORTED;
    if(root && wait==WAIT_OBJECT_0+root_index)return ERROR_PIPE_NOT_CONNECTED;
    return wait==WAIT_OBJECT_0+count-1 ? ERROR_SUCCESS : ERROR_INVALID_STATE;
}

static DWORD scope_launch_native(run16_frontend_scope *scope,const run16_native_start *start,BOOL text)
{
    HANDLE worker=NULL,changed=NULL,target=NULL;DWORD error;
    ULONGLONG deadline=GetTickCount64()+10000;
    if(!scope || !start)return ERROR_INVALID_PARAMETER;
    if(scope->receipt)return ERROR_BUSY;
    error=OpenNtBaseClientWorkerStateChanged(&changed);
    if(error)return error;
    for(;;) {
        error=OpenNtBaseClientStartNativeWorker(&worker);
        if(error==ERROR_ALREADY_EXISTS) {
            /* Each launcher has its own auto-reset state event; NTCON's
             * retirement event is never shared with this admission wait. */
            error=wait_worker_change(changed,NULL,scope->root,deadline);
            if(!error)continue;
            break;
        }
        break;
    }
    if(error)goto done;
    for(;;) {
        /* Both worker kinds acquire the same authenticated presentation route.
         * A reused route is not a new frontend or an input activation. */
        error=text ? OpenNtBaseClientRequestFrontend(scope->capability) : ERROR_SUCCESS;
        if(error==ERROR_ALREADY_EXISTS)error=ERROR_SUCCESS;
        if(!error)error=run16_native_request_submit(text ? scope->capability : NULL,start,&target,&scope->receipt,&scope->native_request);
        if(error!=ERROR_NOT_READY)break;
        /* No request was accepted. Wait only for initial registration; never
         * replay a submitted request or restart a failed worker. */
        error=wait_worker_change(changed,worker,scope->root,deadline);
        if(error)break;
    }
    CloseHandle(worker);
done:
    /* The exported reference is startup diagnostics only. Neither task
     * completion nor Console return depends on local target observation. */
    if(target)CloseHandle(target);
    CloseHandle(changed);
    return error;
}
DWORD run16_frontend_scope_launch_win32_text(run16_frontend_scope *scope,const run16_native_start *start)
{ return scope_launch_native(scope,start,TRUE); }
DWORD run16_frontend_scope_launch_win32_gui(run16_frontend_scope *scope,const run16_native_start *start)
{ return scope_launch_native(scope,start,FALSE); }
DWORD run16_wait_direct_event(HANDLE receipt)
{
    DWORD wait;
    if(!receipt)return ERROR_INVALID_PARAMETER;
    wait=WaitForSingleObject(receipt,INFINITE);
    if(wait==WAIT_FAILED)return GetLastError();
    if(wait!=WAIT_OBJECT_0)return ERROR_INVALID_STATE;
    return ERROR_SUCCESS;
}
DWORD run16_frontend_scope_wait_native(run16_frontend_scope *scope,DWORD *result,DWORD *target_completed)
{
    DWORD error=0;
    if(!scope || !result || !target_completed)return ERROR_INVALID_PARAMETER;
    *result=*target_completed=0;
    if(scope->receipt){
        /* NTSRV owns completion and fails outstanding receipts on worker
         * rundown. Its authenticated death watcher covers broker loss. */
        error=run16_wait_direct_event(scope->receipt);
        if(error)return error;
    }else return ERROR_INVALID_STATE;
    if(scope->receipt){
        /* NTSRV joins the real task result with the final I/O acknowledgement. */
        error=run16_native_request_finish(scope->native_request,result,target_completed);
        CloseHandle(scope->receipt);scope->receipt=NULL;
    }
    return error;
}
DWORD run16_frontend_scope_resume_parent(run16_frontend_scope *scope)
{
    if(!scope || !scope->has_execution)return 0;
    /* NTSRV validates the completed child and inherited worker origin. A DOS
     * origin keeps its original resume path; only a native parent is granted
     * an I/O reacquisition through the existing confirmation barrier. */
    return run16_native_request_resume(scope->capability);
}

DWORD run16_frontend_scope_restore_parent(run16_frontend_scope *scope)
{
    /* Only a root launcher can return an outer CMD to this Console.  An
     * inherited scope is an inner invocation and must never hold its root's
     * frontend lifetime. */
    if(!scope || !scope->owns_environment || !scope->restored)return ERROR_SUCCESS;
    return OpenNtBaseClientWaitFrontendConsoleRestored();
}

DWORD run16_frontend_scope_retire(run16_frontend_scope *scope)
{
    if(!scope || !scope->owns_environment || !scope->restored)return ERROR_SUCCESS;
    return OpenNtBaseClientReturnFrontendConsole();
}
