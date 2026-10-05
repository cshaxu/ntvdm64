/* Project-owned extraction of run16's existing original-shaped SEC_IMAGE
 * query. Native ABI declarations stay in the finite Base classifier facade. */
#include <nt.h>
#include <base_classifier.h>
#include "image_classification.h"
/* Native metadata uses the host RTL export, not the guest/NT4 RTL adapter. */
#undef RtlNtStatusToDosError

DWORD common_classify_native_image(PCWSTR application,DWORD *machine,DWORD *subsystem,
    PWSTR final_path,DWORD final_capacity)
{
    HANDLE file,section=NULL;
    SECTION_IMAGE_INFORMATION information={0};
    NTSTATUS status;DWORD error=ERROR_SUCCESS;
    if(!machine || !subsystem || (!!final_path!=!!final_capacity))return ERROR_INVALID_PARAMETER;
    *machine=*subsystem=0;
    if(final_path)final_path[0]=0;
    if(!application || !*application)return ERROR_INVALID_PARAMETER;
    file=CreateFileW(application,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,
        NULL,OPEN_EXISTING,0,NULL);
    if(file==INVALID_HANDLE_VALUE)return GetLastError();
    status=NtCreateSection(&section,SECTION_QUERY,NULL,NULL,PAGE_READONLY,SEC_IMAGE,file);
    if(NT_SUCCESS(status)) {
        status=NtQuerySection(section,SectionImageInformation,&information,sizeof(information),NULL);
        CloseHandle(section);
    }
    if(!NT_SUCCESS(status))error=RtlNtStatusToDosError(status);
    else if(information.ImageCharacteristics & IMAGE_FILE_DLL)error=ERROR_BAD_EXE_FORMAT;
    else if((information.Machine!=IMAGE_FILE_MACHINE_I386 && information.Machine!=IMAGE_FILE_MACHINE_AMD64) ||
        (information.SubSystemType!=IMAGE_SUBSYSTEM_WINDOWS_GUI &&
         information.SubSystemType!=IMAGE_SUBSYSTEM_WINDOWS_CUI))error=ERROR_NOT_SUPPORTED;
    if(!error && final_path) {
        DWORD count=GetFinalPathNameByHandleW(file,final_path,final_capacity,FILE_NAME_NORMALIZED|VOLUME_NAME_DOS);
        if(!count || count>=final_capacity)error=count ? ERROR_FILENAME_EXCED_RANGE : GetLastError();
    }
    CloseHandle(file);
    if(error){if(final_path)final_path[0]=0;return error;}
    *machine=information.Machine;*subsystem=information.SubSystemType;
    return ERROR_SUCCESS;
}

DWORD common_process_machine(HANDLE process,DWORD *machine)
{
    typedef BOOL (WINAPI *query_machine)(HANDLE,USHORT *,USHORT *);
    query_machine query=(query_machine)GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"IsWow64Process2");
    USHORT guest=0,host=0;
    if(!process || !machine)return ERROR_INVALID_PARAMETER;
    *machine=0;
    if(!query)return ERROR_CALL_NOT_IMPLEMENTED;
    if(!query(process,&guest,&host))return GetLastError();
    *machine=guest ? guest : host;
    return *machine==IMAGE_FILE_MACHINE_I386 || *machine==IMAGE_FILE_MACHINE_AMD64 ?
        ERROR_SUCCESS : ERROR_NOT_SUPPORTED;
}

DWORD common_process_image_path(HANDLE process,PWSTR path,DWORD capacity)
{
    static const WCHAR prefix[]=L"\\\\?\\GLOBALROOT";
    DWORD count;
    if(!path || !capacity)return ERROR_INVALID_PARAMETER;
    path[0]=0;
    if(!process)return ERROR_INVALID_PARAMETER;
    if(capacity<=ARRAYSIZE(prefix))return ERROR_FILENAME_EXCED_RANGE;
    count=capacity-ARRAYSIZE(prefix)+1;
    CopyMemory(path,prefix,(ARRAYSIZE(prefix)-1)*sizeof(WCHAR));
    /* A DOS System32 name can reopen the opposite image through WOW64.
     * Read the actual child in the native namespace; do not disable caller
     * redirection or introduce another image parser/search policy. */
    if(!QueryFullProcessImageNameW(process,PROCESS_NAME_NATIVE,
        path+ARRAYSIZE(prefix)-1,&count)) {
        DWORD error=GetLastError();path[0]=0;return error;
    }
    return ERROR_SUCCESS;
}

DWORD common_classify_native_process(HANDLE process,DWORD *machine,DWORD *subsystem)
{
    WCHAR path[MAX_PATH+32];DWORD error;
    if(!machine || !subsystem)return ERROR_INVALID_PARAMETER;
    *machine=*subsystem=0;
    error=common_process_image_path(process,path,ARRAYSIZE(path));
    if(error)return error;
    return common_classify_native_image(path,machine,subsystem,NULL,0);
}
