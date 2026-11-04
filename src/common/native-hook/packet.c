#include "common/native-hook/context.h"
#include <wchar.h>
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
DWORD nthook_context_parse(nthook_context *context,const void *packet,DWORD bytes,DWORD machine)
{
    DWORD error;native_hook_packet header;const BYTE *data=(const BYTE *)packet;
    if(!context || !data)return ERROR_INVALID_PARAMETER;
    ZeroMemory(context,sizeof(*context));
    if(bytes<sizeof(header) || bytes>NATIVE_HOOK_MAX_BYTES)return ERROR_INVALID_DATA;
    memcpy(&header,data,sizeof(header));
    if(header.bytes!=bytes || header.version!=NATIVE_HOOK_VERSION ||
       (header.mode!=NATIVE_HOOK_INTERCEPT && header.mode!=NATIVE_HOOK_LAUNCHER) ||
       header.machine!=machine ||
       (header.flags & ~NATIVE_HOOK_SEPARATE_WOW) ||
       (header.flags && header.mode!=NATIVE_HOOK_LAUNCHER) ||
       header.reserved ||
       !!header.frontend!=!!header.execution ||
       header.frontend>(uint64_t)(ULONG_PTR)-1 || header.execution>(uint64_t)(ULONG_PTR)-1 ||
       header.launcher_offset!=sizeof(header) ||
       (uint64_t)header.hook_offset!=(uint64_t)header.launcher_offset+header.launcher_bytes ||
       (uint64_t)header.hook64_offset!=(uint64_t)header.hook_offset+header.hook_bytes ||
       header.hook64_offset>bytes || header.hook64_bytes!=bytes-header.hook64_offset)return ERROR_INVALID_DATA;
    error=path_read(data,bytes,header.launcher_offset,header.launcher_bytes,context->launcher);
    if(!error)error=path_read(data,bytes,header.hook_offset,header.hook_bytes,context->hook);
    if(!error)error=path_read(data,bytes,header.hook64_offset,header.hook64_bytes,context->hook64);
    if(error)return error;
    context->mode=header.mode;
    context->creation_flags=header.flags;
    context->frontend=(HANDLE)(ULONG_PTR)header.frontend;
    context->execution=(HANDLE)(ULONG_PTR)header.execution;
    return ERROR_SUCCESS;
}
