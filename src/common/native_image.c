/* Extracted from accepted run16 image_classification.c: original-shaped
 * SEC_IMAGE/SectionImageInformation query, not a replacement classifier.
 * Native process path uses the documented PROCESS_NAME_NATIVE namespace. */
#include <nt.h>
#include <base_classifier.h>
#include "native_image.h"
#include <wchar.h>

DWORD common_native_image(PCWSTR application,DWORD *machine,DWORD *subsystem)
{
    HANDLE file,section=NULL;
    SECTION_IMAGE_INFORMATION information={0};
    NTSTATUS status;
    if(machine)*machine=0;
    if(subsystem)*subsystem=0;
    if(!machine || !subsystem || !application || !*application)
        return ERROR_INVALID_PARAMETER;
    file=CreateFileW(application,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,
        NULL,OPEN_EXISTING,0,NULL);
    if(file==INVALID_HANDLE_VALUE)return GetLastError();
    status=NtCreateSection(&section,SECTION_QUERY,NULL,NULL,PAGE_READONLY,SEC_IMAGE,file);
    CloseHandle(file);
    if(NT_SUCCESS(status)) {
        status=NtQuerySection(section,SectionImageInformation,&information,sizeof(information),NULL);
        CloseHandle(section);
    }
    if(!NT_SUCCESS(status))return RtlNtStatusToDosError(status);
    if(information.ImageCharacteristics & IMAGE_FILE_DLL)return ERROR_BAD_EXE_FORMAT;
    if((information.Machine!=IMAGE_FILE_MACHINE_I386 && information.Machine!=IMAGE_FILE_MACHINE_AMD64) ||
       (information.SubSystemType!=IMAGE_SUBSYSTEM_WINDOWS_GUI &&
        information.SubSystemType!=IMAGE_SUBSYSTEM_WINDOWS_CUI))return ERROR_NOT_SUPPORTED;
    *machine=information.Machine;*subsystem=information.SubSystemType;
    return ERROR_SUCCESS;
}
DWORD common_native_process_path(HANDLE process,PWSTR path,DWORD capacity)
{
    static const WCHAR prefix[]=L"\\\\?\\GLOBALROOT";
    DWORD prefix_count=(DWORD)(sizeof(prefix)/sizeof(prefix[0])-1),count,error;
    if(!path || !capacity)return ERROR_INVALID_PARAMETER;
    path[0]=0;
    if(!process)return ERROR_INVALID_HANDLE;
    if(capacity<=prefix_count+1)return ERROR_INSUFFICIENT_BUFFER;
    memcpy(path,prefix,prefix_count*sizeof(WCHAR));count=capacity-prefix_count;
    if(!QueryFullProcessImageNameW(process,PROCESS_NAME_NATIVE,path+prefix_count,&count)) {
        error=GetLastError();path[0]=0;return error;
    }
    if(!count || path[prefix_count]!=L'\\') {path[0]=0;return ERROR_INVALID_DATA;}
    return ERROR_SUCCESS;
}
DWORD common_native_process_image(HANDLE process,DWORD *machine,DWORD *subsystem)
{
    WCHAR path[MAX_PATH+32];DWORD error;
    if(machine)*machine=0;
    if(subsystem)*subsystem=0;
    if(!machine || !subsystem)return ERROR_INVALID_PARAMETER;
    error=common_native_process_path(process,path,(DWORD)(sizeof(path)/sizeof(path[0])));
    return error ? error : common_native_image(path,machine,subsystem);
}
