#ifndef ADAPTER_WIN32_OPENNT_VDM_API_FACADE_H
#define ADAPTER_WIN32_OPENNT_VDM_API_FACADE_H

#include <stdint.h>
#include <windows.h>

#include "session/session_input.h"

typedef uint8_t OPENNT_VDM_BOOLEAN;

/* True source-shaped subset of OpenNT public/internal/base/inc/vdmapi.h.
 * The historical public ABI belongs at the old Win32 boundary, not in an
 * OpenNT BOP mirror. */
typedef struct _OPENNT_VDMINFO {
    ULONG iTask, dwCreationFlags, ErrorCode, CodePage;
    HANDLE StdIn, StdOut, StdErr;
    LPVOID CmdLine, AppName, PifFile, CurDirectory, Enviornment;
    ULONG EnviornmentSize;
    STARTUPINFOA StartupInfo;
    LPVOID Desktop; ULONG DesktopLen; LPVOID Title; ULONG TitleLen;
    LPVOID Reserved; ULONG ReservedLen;
    USHORT CmdSize, AppLen, PifLen, CurDirectoryLen, VDMState, CurDrive;
    OPENNT_VDM_BOOLEAN fComingFromBat;
} OPENNT_VDMINFO, *POPENNT_VDMINFO;

/* Preserve the original public spellings for imported OpenNT callers. */
typedef OPENNT_VDMINFO VDMINFO;
typedef POPENNT_VDMINFO PVDMINFO;

#define ASKING_FOR_FIRST_COMMAND 0x0001u
#define ASKING_FOR_DOS_BINARY    0x0004u
#define ASKING_FOR_SECOND_TIME   0x0008u
#define INCREMENT_REENTER_COUNT  0x0010u
#define DECREMENT_REENTER_COUNT  0x0020u
#define NO_PARENT_TO_WAKE        0x0040u
#define RETURN_ON_NO_COMMAND     0x0080u
#define ASKING_FOR_ENVIRONMENT   0x0400u

/* Bounded call scopes bind their neutral copied payload here. The facade is
 * thread-local and never retains a guest pointer or native handle. */
int opennt_vdm_api_bind_input(session_input *input);
void opennt_vdm_api_unbind_input(session_input *input);

BOOL WINAPI GetNextVDMCommand(PVDMINFO vdm_info);
BOOL WINAPI SetVDMCurrentDirectories(ULONG bytes, LPSTR directories);

#endif
