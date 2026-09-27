#include "frontend_scope.h"
#include "ntkvm-exe/bootstrap.h"
#include "ntkvm-exe/native_request_client.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "product-abi/console_io.h"
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
    HANDLE capability,root,receipt;
    BOOL owns_environment,has_execution;
    DWORD console_mask;
};
void run16_frontend_scope_end(run16_frontend_scope *scope)
{
    if(!scope)return;
    if(scope->owns_environment)SetEnvironmentVariableA(FRONTEND_ENV,NULL);
    if(scope->capability)CloseHandle(scope->capability);
    if(scope->root)CloseHandle(scope->root);
    if(scope->receipt)CloseHandle(scope->receipt);
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
        /* Inherited locator is untrusted until the broker matches the object. */
        error=OpenNtBaseClientRetainFrontendRoot(inherited_frontend,&scope->root,&generation);
        if (error) goto fail;
        if (inherited_execution) {
            error=OpenNtBaseClientBindConsoleContext(inherited_execution);
            if (error) goto fail;
            scope->has_execution=TRUE;
        }
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

DWORD run16_frontend_scope_launch_native(run16_frontend_scope *scope,const run16_native_start *start,HANDLE *target)
{
    if(!scope)return ERROR_INVALID_PARAMETER;
    if(scope->receipt)return ERROR_BUSY;
    return run16_native_request_submit_receipt(scope->root,scope->capability,start,target,&scope->receipt);
}
DWORD run16_frontend_scope_wait_native(run16_frontend_scope *scope,HANDLE target,DWORD *result)
{
    if(!scope || !target || !result)return ERROR_INVALID_PARAMETER;
    if(WaitForSingleObject(target,INFINITE)!=WAIT_OBJECT_0 || !GetExitCodeProcess(target,result))return GetLastError();
    if(scope->receipt){
        HANDLE waits[2]={scope->receipt,scope->root};
        /* Presentation acknowledgment, not session retirement or target status. */
        WaitForMultipleObjects(2,waits,FALSE,2000);
        CloseHandle(scope->receipt);scope->receipt=NULL;
    }
    return ERROR_SUCCESS;
}
