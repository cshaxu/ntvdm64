#ifndef OPENNT_BASE_NATIVE_LAUNCH_TYPE_H
#define OPENNT_BASE_NATIVE_LAUNCH_TYPE_H

#include <windows.h>

/* Run16 obtains this from the existing SEC_IMAGE classification boundary.
 * It is copied through the native-launch reservation only; DOS and Win16
 * retain their original BaseCheckVDM BinaryType contract. */
typedef enum OPENNT_BASE_NATIVE_LAUNCH_TYPE {
    OPENNT_BASE_NATIVE_LAUNCH_NONE = 0,
    OPENNT_BASE_NATIVE_LAUNCH_WIN32_CUI,
    OPENNT_BASE_NATIVE_LAUNCH_WIN32_GUI,
    OPENNT_BASE_NATIVE_LAUNCH_WIN64_CUI,
    OPENNT_BASE_NATIVE_LAUNCH_WIN64_GUI
} OPENNT_BASE_NATIVE_LAUNCH_TYPE;

static __inline BOOL OpenNtBaseNativeLaunchTypeIsValid(DWORD type)
{
    return type>=OPENNT_BASE_NATIVE_LAUNCH_WIN32_CUI &&
        type<=OPENNT_BASE_NATIVE_LAUNCH_WIN64_GUI;
}

static __inline BOOL OpenNtBaseNativeLaunchTypeIsText(DWORD type)
{
    return type==OPENNT_BASE_NATIVE_LAUNCH_WIN32_CUI ||
        type==OPENNT_BASE_NATIVE_LAUNCH_WIN64_CUI;
}

static __inline DWORD OpenNtBaseNativeLaunchTypeFromImage(DWORD machine,DWORD subsystem)
{
    if(machine==IMAGE_FILE_MACHINE_I386)
        return subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI ? OPENNT_BASE_NATIVE_LAUNCH_WIN32_CUI :
            subsystem==IMAGE_SUBSYSTEM_WINDOWS_GUI ? OPENNT_BASE_NATIVE_LAUNCH_WIN32_GUI : 0;
    if(machine==IMAGE_FILE_MACHINE_AMD64)
        return subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI ? OPENNT_BASE_NATIVE_LAUNCH_WIN64_CUI :
            subsystem==IMAGE_SUBSYSTEM_WINDOWS_GUI ? OPENNT_BASE_NATIVE_LAUNCH_WIN64_GUI : 0;
    return 0;
}

#endif
