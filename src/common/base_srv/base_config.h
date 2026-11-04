/* Private binding for original BaseGetVdmConfigInfo: immutable app-owned
 * package paths replace the NT4 WOW registry. No executable classification. */
#ifndef OPENNT_BASE_CONFIG_H
#define OPENNT_BASE_CONFIG_H
#include <nt.h>
#include <stdio.h>
#include <string.h>
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
BOOL BaseGetVdmConfigInfo(LPCWSTR, ULONG, ULONG, PUNICODE_STRING, PULONG);
#endif
