/* Project boundary recovered from run16/main.c's image-section query.
 * OpenNT base/win32/client/vdm.c queries SectionImageInformation for its
 * native classification. Keep the OS metadata contract, not a PE parser.
 * DOS/WOW discovery and their original execution owners remain unchanged. */
#include <nt.h>
#include <base_classifier.h>
#include "image_classification.h"

DWORD run16_classify_native_image(PCWSTR application,DWORD *subsystem)
{
    HANDLE file,section=NULL;
    SECTION_IMAGE_INFORMATION information={0};
    NTSTATUS status;
    if(!subsystem)return ERROR_INVALID_PARAMETER;
    *subsystem=0;
    if(!application || !*application)return ERROR_INVALID_PARAMETER;
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
    if(information.SubSystemType!=IMAGE_SUBSYSTEM_WINDOWS_GUI &&
        information.SubSystemType!=IMAGE_SUBSYSTEM_WINDOWS_CUI)return ERROR_NOT_SUPPORTED;
    *subsystem=information.SubSystemType;
    return ERROR_SUCCESS;
}
