#include "common/native-hook/context.h"
#include "lib-win32/process-hook/staging.h"
#include "lib-win32/process-hook/payload.h"
#include <wchar.h>
static DWORD consume_packet(const void *packet,DWORD bytes,DWORD machine,void *context)
{return nthook_context_parse((nthook_context *)context,packet,bytes,machine);}
static DWORD consume_direct(const void *packet,DWORD bytes,void *context)
{
    DWORD machine=sizeof(void *)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386;
    return nthook_context_parse((nthook_context *)context,packet,bytes,machine);
}
DWORD nthook_context_read(nthook_context *context,BOOL *found)
{
    DWORD error;
    if(!context || !found)return ERROR_INVALID_PARAMETER;
    ZeroMemory(context,sizeof(*context));*found=FALSE;
    error=process_hook_stage_read(consume_packet,context,found);
    if(error)return error;
    if(*found)return process_hook_stage_restore();
    return process_hook_payload_take(consume_direct,context,found);
}
DWORD nthook_context_paths(nthook_context *context)
{
    WCHAR image[MAX_PATH],*slash;DWORD count;
    if(!context)return ERROR_INVALID_PARAMETER;
    ZeroMemory(context,sizeof(*context));
    count=GetModuleFileNameW(NULL,image,MAX_PATH);
    if(!count || count>=MAX_PATH)return ERROR_FILENAME_EXCED_RANGE;
    slash=wcsrchr(image,L'\\');if(!slash)return ERROR_INVALID_NAME;
    slash[1]=0;
    if(swprintf_s(context->launcher,MAX_PATH,L"%lsrun16.exe",image)<0 ||
       swprintf_s(context->hook,MAX_PATH,L"%lsnthook32.dll",image)<0 ||
       swprintf_s(context->hook64,MAX_PATH,L"%lsnthook64.dll",image)<0)return ERROR_FILENAME_EXCED_RANGE;
    return ERROR_SUCCESS;
}
