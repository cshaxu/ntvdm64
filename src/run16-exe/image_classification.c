/* Project boundary recovered from run16/main.c's image-section query.
 * OpenNT base/win32/client/vdm.c queries SectionImageInformation for its
 * native classification. Keep the OS metadata contract, not a PE parser.
 * DOS/WOW discovery and their original execution owners remain unchanged. */
#include "image_classification.h"
#include "common/native_image.h"

DWORD run16_classify_native_image(PCWSTR application,DWORD *subsystem)
{
    DWORD machine;
    return common_native_image(application,&machine,subsystem);
}

DWORD run16_classify_native_launch_type(PCWSTR application,DWORD *native_type)
{
    DWORD machine,subsystem,error;
    if(!native_type)return ERROR_INVALID_PARAMETER;
    *native_type=0;
    error=common_native_image(application,&machine,&subsystem);
    if(error)return error;
    *native_type=OpenNtBaseNativeLaunchTypeFromImage(machine,subsystem);
    return *native_type ? ERROR_SUCCESS : ERROR_BAD_EXE_FORMAT;
}
