#include "hook.h"
#include "detours/detours.h"
#include <wchar.h>
/* Private copied-payload identity, not a discoverable authorization token. */
extern const GUID nthook_payload_guid={0x62430757,0xd5a2,0x46e8,{0x81,0xbe,0xb3,0x72,0x4f,0x63,0x89,0x11}};
static DWORD path_read(const BYTE *data,DWORD bytes,DWORD offset,DWORD count,PWSTR out)
{
    const WCHAR *value;
    if(offset<sizeof(native_hook_packet) || offset%sizeof(WCHAR) ||
       count<4*sizeof(WCHAR) || count%sizeof(WCHAR) || count>MAX_PATH*sizeof(WCHAR) ||
       offset>bytes || count>bytes-offset)return ERROR_INVALID_DATA;
    value=(const WCHAR *)(data+offset);
    if(value[count/sizeof(WCHAR)-1] || wcsnlen_s(value,count/sizeof(WCHAR))!=count/sizeof(WCHAR)-1 ||
       value[1]!=L':' || (value[2]!=L'\\' && value[2]!=L'/'))return ERROR_INVALID_DATA;
    memcpy(out,value,count);return ERROR_SUCCESS;
}
DWORD nthook_context_read(nthook_context *context,BOOL *found)
{
    DWORD bytes=0,error;native_hook_packet header;const BYTE *data;
    if(!context || !found)return ERROR_INVALID_PARAMETER;
    ZeroMemory(context,sizeof(*context));*found=FALSE;
    data=(const BYTE *)DetourFindPayloadEx(nthook_payload_guid,&bytes);
    if(!data)return ERROR_SUCCESS;
    *found=TRUE;
    if(bytes<sizeof(header) || bytes>NATIVE_HOOK_MAX_BYTES)return ERROR_INVALID_DATA;
    memcpy(&header,data,sizeof(header));
    if(header.bytes!=bytes || header.version!=NATIVE_HOOK_VERSION ||
       (header.mode!=NATIVE_HOOK_INTERCEPT && header.mode!=NATIVE_HOOK_LAUNCHER) ||
       header.machine!=
#if defined(_WIN64)
           IMAGE_FILE_MACHINE_AMD64 ||
#else
           IMAGE_FILE_MACHINE_I386 || header.frontend>MAXDWORD || header.execution>MAXDWORD ||
#endif
       header.flags || header.reserved || !!header.frontend!=!!header.execution ||
       header.launcher_offset!=sizeof(header) ||
       (uint64_t)header.hook_offset!=(uint64_t)header.launcher_offset+header.launcher_bytes ||
       (uint64_t)header.hook64_offset!=(uint64_t)header.hook_offset+header.hook_bytes ||
       header.hook64_offset>bytes || header.hook64_bytes!=bytes-header.hook64_offset)return ERROR_INVALID_DATA;
    error=path_read(data,bytes,header.launcher_offset,header.launcher_bytes,context->launcher);
    if(!error)error=path_read(data,bytes,header.hook_offset,header.hook_bytes,context->hook);
    if(!error)error=path_read(data,bytes,header.hook64_offset,header.hook64_bytes,context->hook64);
    if(error)return error;
    context->mode=header.mode;
    context->frontend=(HANDLE)(ULONG_PTR)header.frontend;
    context->execution=(HANDLE)(ULONG_PTR)header.execution;
    return ERROR_SUCCESS;
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
