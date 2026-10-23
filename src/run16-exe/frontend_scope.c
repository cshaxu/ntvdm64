#include "frontend_scope.h"
#include "run16-exe/frontend_bootstrap.h"
#include "run16-exe/native_request_client.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "common/protocol/console_io.h"
#include "common/protocol/frontend_lifecycle.h"
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
    HANDLE capability,receipt;
    BOOL owns_environment,has_execution;
    DWORD console_mask,native_request;
    DWORD creation_flags;
};
void run16_frontend_scope_end(run16_frontend_scope *scope)
{
    if(!scope)return;
    if(scope->owns_environment)SetEnvironmentVariableA(FRONTEND_ENV,NULL);
    if(scope->capability)CloseHandle(scope->capability);
    if(scope->receipt)CloseHandle(scope->receipt);
    HeapFree(GetProcessHeap(),0,scope);
}
static DWORD scope_begin(run16_frontend_scope **output,BOOL lease,BOOL console_owned,BOOL acquire_frontend)
{
    run16_frontend_scope *scope;
    char text[32];
    DWORD error=ERROR_SUCCESS,generation;
    HANDLE inherited_frontend=NULL,inherited_execution=NULL,root=NULL;
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
        error=OpenNtBaseClientRetainFrontendRoot(inherited_frontend,&root,&generation);
        if(root){CloseHandle(root);root=NULL;}
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
        frontend_bootstrap_release(&connection);
        sprintf_s(text,sizeof(text),"%llx",(unsigned long long)(uintptr_t)scope->capability);
        if (!SetEnvironmentVariableA(FRONTEND_ENV,text)) { error=GetLastError();goto fail; }
        scope->owns_environment=TRUE;
    }
    scope->creation_flags=seeded ? seed.creation_flags : 0;
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
    /* Retaining/creating a frontend may have succeeded before later setup
     * failed. Complete that request through the same service-owned barrier;
     * preserve the original setup error rather than inventing task success. */
    (void)run16_frontend_scope_finish(scope);
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

DWORD run16_frontend_scope_creation_flags(run16_frontend_scope *scope)
{ return scope ? scope->creation_flags : 0; }

DWORD run16_frontend_scope_launch_native(run16_frontend_scope *scope,const run16_native_start *start,DWORD native_type)
{
    HANDLE worker=NULL,target=NULL;DWORD error;
    BOOL text;
    if(!scope || !start || !OpenNtBaseNativeLaunchTypeIsValid(native_type))return ERROR_INVALID_PARAMETER;
    text=OpenNtBaseNativeLaunchTypeIsText(native_type);
    if(scope->receipt)return ERROR_BUSY;
    error=OpenNtBaseClientStartNativeWorker(native_type,&worker);
    if(worker)CloseHandle(worker);
    if(error)goto done;
    /* One submission. NTSRV owns readiness and frontend association; neither
     * a transient status nor an ambiguous reply authorizes a replay here. */
    error=run16_native_request_submit(text ? scope->capability : NULL,start,
        &target,&scope->receipt,&scope->native_request);
done:
    /* The exported reference is startup diagnostics only. Neither task
     * completion nor Console return depends on local target observation. */
    if(target)CloseHandle(target);
    return error;
}
DWORD run16_wait_direct_event(HANDLE receipt)
{
    HANDLE server=NULL,waits[2];DWORD wait,error;
    if(!receipt)return ERROR_INVALID_PARAMETER;
    error=OpenNtBaseClientBrokerProcess(&server);
    if(error)return error;
    waits[0]=server;waits[1]=receipt;
    wait=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
    error=wait==WAIT_OBJECT_0 ? ERROR_INVALID_HANDLE :
        wait==WAIT_OBJECT_0+1 ? ERROR_SUCCESS :
        wait==WAIT_FAILED ? GetLastError() : ERROR_INVALID_STATE;
    CloseHandle(server);
    return error;
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
DWORD run16_frontend_scope_finish(run16_frontend_scope *scope)
{
    if(!scope)return ERROR_SUCCESS;
    return OpenNtBaseClientWaitFrontendConsoleRestored();
}
