#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "mvdm_softpc_firmware.h"
#include "ntvdm-exe/session/session.h"

/* Unused co-resident media functions must not run in this conversion test. */
session *session_thread_current(void) { ExitProcess(90); return NULL; }
const char *session_firmware_root(const session *instance) { (void)instance; ExitProcess(91); return NULL; }
const char *session_mvdm_system_root(const session *instance) { (void)instance; ExitProcess(92); return NULL; }

int main(void)
{
    char path[MAX_PATH+13], expected[MAX_PATH+12];
    BOOL substituted=FALSE;
    int count;
    if (!WideCharToMultiByte(CP_ACP,0,L"D\x00a3\\P30.NT",-1,
        expected,sizeof(expected),NULL,&substituted) || substituted) return 1;
    memset(path,0x5a,sizeof(path));
    count=WideCharToMultiByte(CP_OEMCP,0,L"D\x00a3\\P30.NT",-1,
        path,MAX_PATH+12,NULL,&substituted);
    if (!count || substituted || !mvdm_softpc_config_path_to_ansi(path,MAX_PATH+12) ||
        strcmp(path,expected) || path[MAX_PATH+12]!=0x5a) return 2;
    strcpy_s(path,sizeof(path),"ascii");
    if (!mvdm_softpc_config_path_to_ansi(path,6) || strcmp(path,"ascii")) return 3;
    path[0]=0;
    if (!mvdm_softpc_config_path_to_ansi(path,1) || path[0]) return 4;
    memset(path,0x5a,sizeof(path));
    if (mvdm_softpc_config_path_to_ansi(path,MAX_PATH+12) ||
        mvdm_softpc_config_path_to_ansi(path,0) ||
        mvdm_softpc_config_path_to_ansi(path,sizeof(path)) ||
        mvdm_softpc_config_path_to_ansi(NULL,1) || path[0]!=0x5a ||
        path[MAX_PATH+12]!=0x5a) return 5;
    puts("S37_PIF_OEM_TO_ANSI_BOUNDED_PATH_OK");
    return 0;
}
