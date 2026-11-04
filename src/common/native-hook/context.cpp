#include "common/native-hook/context.h"
#include "lib-win32/process-hook/detours/detours.h"
#include <wchar.h>
/* Private copied-payload identity, not a discoverable authorization token. */
extern const GUID nthook_payload_guid={0x62430757,0xd5a2,0x46e8,{0x81,0xbe,0xb3,0x72,0x4f,0x63,0x89,0x11}};
DWORD nthook_context_read(nthook_context *context,BOOL *found)
{
    DWORD bytes=0;const BYTE *data;
    if(!context || !found)return ERROR_INVALID_PARAMETER;
    ZeroMemory(context,sizeof(*context));*found=FALSE;
    data=(const BYTE *)DetourFindPayloadEx(nthook_payload_guid,&bytes);
    if(!data)return ERROR_SUCCESS;
    *found=TRUE;
    return nthook_context_parse(context,data,bytes,
        sizeof(void *)==8 ? IMAGE_FILE_MACHINE_AMD64 : IMAGE_FILE_MACHINE_I386);
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
