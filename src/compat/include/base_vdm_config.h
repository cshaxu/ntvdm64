/* Explicit immutable VDM configuration binding; no request or registry owner. */
#ifndef OPENNT_COMPAT_BASE_VDM_CONFIG_H
#define OPENNT_COMPAT_BASE_VDM_CONFIG_H
#include <windows.h>
#define MAX_VDM_CFG_LINE 256
#define CMDLINE L"cmdline"
#define WOWCMDLINE L"wowcmdline"
#define DOSSIZE L"size"
#define WOWSIZE L"wowsize"
typedef struct {
    CHAR Dos[MAX_VDM_CFG_LINE];
    CHAR Wow[MAX_VDM_CFG_LINE];
    ULONG ImageLength;
} OPENNT_BASE_VDM_CONFIG;
BOOL OpenNtBaseInitializeVdmConfig(OPENNT_BASE_VDM_CONFIG *, PCSTR, PCSTR);
const OPENNT_BASE_VDM_CONFIG *OpenNtBaseBindVdmConfig(const OPENNT_BASE_VDM_CONFIG *);
BOOL BaseGetVDMKeyword(LPWSTR, LPSTR, LPDWORD, LPWSTR, LPDWORD);
PCHAR OpenNtBaseVdmImageEnd(PCHAR);
#endif
