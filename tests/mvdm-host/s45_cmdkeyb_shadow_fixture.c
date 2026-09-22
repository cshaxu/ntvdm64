/* Exercise original cmdkeyb.c's configured, absent and malformed branches
 * without changing the host keyboard layout or Registry.  The production PIF
 * test supplies the guest BOP route; this fixture supplies only its Console
 * layout input so each original reader outcome can be deterministic. */
#define WINNT 1
#include <windows.h>
#include <stdio.h>
#include <string.h>

BOOL bPifFastPaste;
static CHAR layout_name[KL_NAMELENGTH];
static USHORT result_dx;
static CHAR guest_bytes[512];

VOID cmdGetKbdLayout(VOID);
BOOL ntvdm_shadow_registry_initialize(VOID);
VOID ntvdm_shadow_registry_shutdown(VOID);

USHORT getDX(VOID) { return 0; }
USHORT getDS(VOID) { return 0; }
USHORT getSI(VOID) { return 0; }
USHORT getCX(VOID) { return 0; }
VOID setDX(USHORT value) { result_dx=value; }
PVOID GetVDMAddr(USHORT segment, USHORT offset)
{
    (void)segment;
    (void)offset;
    return guest_bytes;
}
UCHAR *__cdecl Sim32pGetVDMPointer(ULONG address, UCHAR protected_mode)
{
    (void)address;
    (void)protected_mode;
    return (UCHAR *)guest_bytes;
}
VOID cmdInitConsole(VOID) {}
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR name)
{
    return strcpy_s(name,KL_NAMELENGTH,layout_name)==0;
}

static BOOL write_file(LPCSTR path, LPCSTR text)
{
    HANDLE file;
    DWORD bytes,written;

    file=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE) return FALSE;
    bytes=(DWORD)lstrlenA(text);
    if (!WriteFile(file,text,bytes,&written,NULL) || written!=bytes) {
        CloseHandle(file);
        return FALSE;
    }
    CloseHandle(file);
    return TRUE;
}

static int run_case(LPCSTR module, LPCSTR name, LPCSTR file_text, USHORT expected)
{
    CHAR path[MAX_PATH],*slash;

    if (!GetModuleFileNameA(NULL,path,sizeof(path))) return 1;
    slash=strrchr(path,'\\');
    if (!slash) return 1;
    lstrcpyA(slash+1,"NTVDM.REG");
    if (!write_file(path,file_text)) return 1;
    lstrcpyA(layout_name,name);
    result_dx=0xffffu;
    bPifFastPaste=FALSE;
    if (!ntvdm_shadow_registry_initialize()) {
        DeleteFileA(path);
        return 1;
    }
    cmdGetKbdLayout();
    ntvdm_shadow_registry_shutdown();
    DeleteFileA(path);
    if (result_dx!=expected) return 1;
    printf("S45_CMDKEYB_%s_OK\n",module);
    return 0;
}

int main(VOID)
{
    static const CHAR configured[]=
        "Windows Registry Editor Version 5.00\r\n\r\n"
        "[HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Control\\Keyboard Layout\\DosKeybCodes]\r\n"
        "\"00000409\"=\"US\"\r\n";
    static const CHAR malformed[]=
        "Windows Registry Editor Version 5.00\r\n\r\n"
        "[HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Control\\Keyboard Layout\\DosKeybCodes]\r\n"
        "\"00000409\"=hex:zz\r\n";
    static const CHAR absent[]=
        "Windows Registry Editor Version 5.00\r\n\r\n"
        "[HKEY_LOCAL_MACHINE\\System\\CurrentControlSet\\Control\\Keyboard Layout\\DosKeybCodes]\r\n"
        "\"00000409\"=\"US\"\r\n";

    if (run_case("CONFIGURED","00000409",configured,0)) return 1;
    if (run_case("MALFORMED","00000409",malformed,0)) return 2;
    if (run_case("ABSENT","0000F00D",absent,0)) return 3;
    return 0;
}
