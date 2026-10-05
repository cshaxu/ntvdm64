#include "hook.h"
#include "detours/detours.h"
#include <wchar.h>
extern const GUID nthook_payload_guid;
DWORD nthook_launcher_target(HANDLE child,const nthook_context *context,BOOL *launcher)
{
    WCHAR actual[MAX_PATH];DWORD count=MAX_PATH,error=ERROR_SUCCESS;
    HANDLE files[2]={INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE};BY_HANDLE_FILE_INFORMATION info[2];
    if(!child || !context || !launcher)return ERROR_INVALID_PARAMETER;
    *launcher=FALSE;
    if(!QueryFullProcessImageNameW(child,0,actual,&count))return GetLastError();
    PCWSTR names[2]={actual,context->launcher};
    for(unsigned i=0;i<2;++i) {
        files[i]=CreateFileW(names[i],FILE_READ_ATTRIBUTES,FILE_SHARE_READ|FILE_SHARE_DELETE,
            NULL,OPEN_EXISTING,0,NULL);
        if(files[i]==INVALID_HANDLE_VALUE || !GetFileInformationByHandle(files[i],&info[i])) {
            error=GetLastError();break;
        }
    }
    if(!error)*launcher=info[0].dwVolumeSerialNumber==info[1].dwVolumeSerialNumber &&
        info[0].nFileIndexHigh==info[1].nFileIndexHigh && info[0].nFileIndexLow==info[1].nFileIndexLow;
    for(unsigned i=0;i<2;++i)if(files[i]!=INVALID_HANDLE_VALUE)CloseHandle(files[i]);
    return error;
}
BOOL nthook_target32(HANDLE child)
{
    BOOL self_wow=FALSE,child_wow=FALSE;
    if(!IsWow64Process(GetCurrentProcess(),&self_wow) || !IsWow64Process(child,&child_wow))return FALSE;
    return !self_wow || child_wow;
}
static void undo_attachment(HANDLE child,HANDLE value)
{
    HANDLE local=NULL;
    if(value && DuplicateHandle(child,value,GetCurrentProcess(),&local,0,FALSE,
        DUPLICATE_CLOSE_SOURCE|DUPLICATE_SAME_ACCESS))CloseHandle(local);
}
DWORD nthook_install(HANDLE child,const nthook_context *context,DWORD mode)
{
    BYTE data[sizeof(native_hook_packet)+2*MAX_PATH*sizeof(WCHAR)]={0};
    native_hook_packet *header=(native_hook_packet *)data;
    HANDLE attached[2]={NULL,NULL};DWORD error=ERROR_SUCCESS,flags;
    char dll[MAX_PATH*2];BOOL default_char=FALSE;
    if(!child || !context || (mode!=NATIVE_HOOK_INTERCEPT && mode!=NATIVE_HOOK_LAUNCHER) ||
        !!context->frontend!=!!context->execution)return ERROR_INVALID_PARAMETER;
    if(!nthook_target32(child))return ERROR_NOT_SUPPORTED;
    if(!context->launcher[0] || !context->hook[0] ||
       wcsnlen_s(context->launcher,MAX_PATH)==MAX_PATH || wcsnlen_s(context->hook,MAX_PATH)==MAX_PATH)
        return ERROR_INVALID_DATA;
    if(mode==NATIVE_HOOK_INTERCEPT) {
        BOOL launcher=FALSE;
        error=nthook_launcher_target(child,context,&launcher);
        if(error)return error;
        /* The actual pinned launcher receives context only, including when
         * NTVWM itself creates it. Never install its own interception DLL. */
        if(launcher)mode=NATIVE_HOOK_LAUNCHER;
    }
    if(mode==NATIVE_HOOK_INTERCEPT) {
        flags=GetFileAttributesW(context->hook);
        if(flags==INVALID_FILE_ATTRIBUTES)return GetLastError();
        if(flags&FILE_ATTRIBUTE_DIRECTORY)return ERROR_BAD_EXE_FORMAT;
        if(!WideCharToMultiByte(CP_ACP,WC_NO_BEST_FIT_CHARS,context->hook,-1,dll,sizeof(dll),NULL,&default_char) ||
            default_char)return ERROR_NO_UNICODE_TRANSLATION;
    }
    for(DWORD i=0;i<2;++i) {
        HANDLE source=i?context->execution:context->frontend;
        if(source && !DuplicateHandle(GetCurrentProcess(),source,child,&attached[i],
            0,FALSE,DUPLICATE_SAME_ACCESS)){error=GetLastError();goto failed;}
    }
    header->version=NATIVE_HOOK_VERSION;header->mode=mode;header->machine=IMAGE_FILE_MACHINE_I386;
    header->frontend=(ULONG_PTR)attached[0];header->execution=(ULONG_PTR)attached[1];
    header->launcher_offset=sizeof(*header);
    header->launcher_bytes=(DWORD)((wcslen(context->launcher)+1)*sizeof(WCHAR));
    header->hook_offset=header->launcher_offset+header->launcher_bytes;
    header->hook_bytes=(DWORD)((wcslen(context->hook)+1)*sizeof(WCHAR));
    header->bytes=header->hook_offset+header->hook_bytes;
    memcpy(data+header->launcher_offset,context->launcher,header->launcher_bytes);
    memcpy(data+header->hook_offset,context->hook,header->hook_bytes);
    if(!DetourCopyPayloadToProcess(child,nthook_payload_guid,data,header->bytes)) {error=GetLastError();goto failed;}
    if(mode==NATIVE_HOOK_INTERCEPT) {
        LPCSTR imports[1]={dll};
        /* Deliberately never call DetourCreateProcessWithDll*: its fallback
         * can spawn a cross-width helper. This update is same-width only. */
        if(!DetourUpdateProcessWithDll(child,imports,1)){error=GetLastError();goto failed;}
    }
    return ERROR_SUCCESS;
failed:
    for(DWORD i=0;i<2;++i)undo_attachment(child,attached[i]);
    return error?error:ERROR_DLL_INIT_FAILED;
}
