#include "run16-exe/native_request_client.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "common/native_image.h"
DWORD run16_native_request_submit(HANDLE root_capability,const run16_native_start *start,HANDLE *target,HANDLE *receipt,DWORD *request)
{
    run16_native_start local={0};
    BYTE *payload=NULL;DWORD error,bytes=0,native_request=0;
    WCHAR application[32767];
    if(!target || !receipt || !request)return ERROR_INVALID_PARAMETER;
    *target=NULL;*receipt=NULL;local.capabilities[0]=local.capabilities[1]=NULL;
    *request=0;
    if(start) {
        local=*start;local.capabilities[0]=local.capabilities[1]=NULL;
        /* The caller owns file discovery, including its own WOW64 view.
         * Preserve that selected file across a different-width worker;
         * the original command/argv text and creation contract stay intact. */
        if(local.application && *local.application) {
            error=common_native_application_path(local.application,application,ARRAYSIZE(application));
            if(error)return error;
            local.application=application;
        }
        error=run16_native_launch_pack(&local,&payload,&bytes);if(error)return error;
    }
    error=OpenNtBaseClientSubmitNativeRequest(root_capability,bytes,payload,target,receipt,&native_request);
    if(!error)*request=native_request;
    HeapFree(GetProcessHeap(),0,payload);
    return error;
}
DWORD run16_native_request_finish(DWORD request,DWORD *exit_code,DWORD *target_completed)
{
    if(!exit_code || !target_completed || !request)return ERROR_INVALID_PARAMETER;
    *exit_code=*target_completed=0;
    return OpenNtBaseClientFinishNativeRequest(request,exit_code,target_completed);
}
DWORD run16_native_request_resume(HANDLE capability)
{
    HANDLE target=NULL,receipt=NULL;DWORD request=0;
    return run16_native_request_submit(capability,NULL,&target,&receipt,&request);
}
