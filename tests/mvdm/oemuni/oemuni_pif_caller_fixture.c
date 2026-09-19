/* Link the formal original cmdpif.obj; model only its incoming task/policy data.
 * Real OEMUNI/Win32 path functions operate on private fixture files. */
#include "cmd.h"
#include <ctype.h>
#include <softpc.h>
#include <oemuni.h>
#include "nt_pif.h"
#include <stdint.h>
#include <wchar.h>

PIF_DATA pfdata;
ULONG DosSessionId;
BOOL IsFirstVDM, DontCheckDosBinaryType;
static int message, writes;
static FILE *report;
int RcMessageBox(USHORT id, CHAR *one, CHAR *two, ULONG options)
{ (void)one; (void)two; (void)options; message = id; return 0; }
BOOL GetPIFData(PIF_DATA *data, char *name)
{ (void)data; (void)name; ExitProcess(92); return FALSE; }
DWORD cmdExpandEnvironmentStrings(PVDMENVBLK env, PCHAR input, PCHAR output, DWORD size)
{
    size_t bytes = strlen(input) + 1;
    (void)env;
    if (bytes > size) return (DWORD)bytes;
    memcpy(output, input, bytes);
    return (DWORD)bytes;
}
int mvdm_command_guest_state_write_is_dos_binary(uint8_t value)
{ if (value) ExitProcess(93); ++writes; return 1; }
#define CHECK(x) do { if (!(x)) { fprintf(report, "S37_PIF_CALLER_FAIL line=%d error=%lu\n", __LINE__, GetLastError()); fclose(report); return 1; } } while (0)
int main(void)
{
    WCHAR original[MAX_PATH], actual[MAX_PATH], expected[MAX_PATH], shortName[MAX_PATH];
    CHAR app[MAX_PATH+1], command[128];
    char files[][8] = { "R\x9c.COM", "R\x9c.TXT" };
    VDMINFO info;
    HANDLE file;
    unsigned int row;
    report = fopen("pif-caller-result.txt", "w");
    if (!report) return 90;
    CHECK(GetOEMCP() == 437 && GetCurrentDirectoryW(MAX_PATH, original));
    CHECK(CreateDirectoryW(L"O37PIF", NULL));
    CHECK(CreateDirectoryW(L"O37PIF\\D\x00a3", NULL));
    CHECK(GetFullPathNameW(L"O37PIF\\D\x00a3", MAX_PATH, expected, NULL));
    CHECK(SetCurrentDirectoryW(expected));
    for (row = 0; row < 2; ++row) {
        file = CreateFileOem(files[row], GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
        CHECK(file != INVALID_HANDLE_VALUE && CloseHandle(file));
    }
    CHECK(GetShortPathNameW(L"R\x00a3.COM", shortName, MAX_PATH));
    CHECK(SetCurrentDirectoryW(original));
    for (row = 0; row < 5; ++row) {
        memset(&pfdata, 0, sizeof(pfdata)); memset(&info, 0, sizeof(info));
        strcpy_s(app, sizeof(app), row == 1 ? "KEEP.COM" : "TASK.PIF"); command[0] = 0;
        info.AppName = app; info.CmdLine = command;
        /* GetPIFData supplies expandable MAX_PATH+1 allocations, not strdup. */
        pfdata.StartDir = calloc(MAX_PATH+1, 1);
        pfdata.StartFile = calloc(MAX_PATH+1, 1);
        pfdata.CmdLine = calloc(MAX_PATH+1, 1);
        CHECK(pfdata.StartDir && pfdata.StartFile && pfdata.CmdLine);
        strcpy_s(pfdata.StartDir, MAX_PATH+1, row == 2 ? "O37PIF\\ABSENT" : "O37PIF\\D\x9c");
        strcpy_s(pfdata.StartFile, MAX_PATH+1, row == 3 ? "P37NONE.COM" : files[row == 4]);
        strcpy_s(pfdata.CmdLine, MAX_PATH+1, "argument");
        pfdata.SubSysId = SUBSYS_DOS; IsFirstVDM = TRUE;
        DosSessionId = row == 1 ? 0 : 1; message = writes = 0;
        cmdCheckForPIF(&info);
        fprintf(report, "row=%u message=%d writes=%d appLen=%u\n", row, message, writes, info.AppLen);
        fflush(report);
        CHECK(!pfdata.StartDir && !pfdata.StartFile && !pfdata.CmdLine && !pfdata.WinTitle);
        CHECK(GetCurrentDirectoryW(MAX_PATH, actual));
        if (row == 0) {
            WCHAR selected[MAX_PATH], expectedFile[MAX_PATH];
            CHECK(!message && writes == 1 && info.AppLen == strlen(app)+1);
            CHECK(!strcmp(command, "argument\r\n") && DontCheckDosBinaryType);
            CHECK(!wcscmp(actual, expected));
            CHECK(MultiByteToWideChar(CP_OEMCP, 0, app, -1, selected, MAX_PATH));
            CHECK(GetFullPathNameW(shortName, MAX_PATH, expectedFile, NULL));
            CHECK(!_wcsicmp(selected, expectedFile));
        } else if (row == 1) {
            CHECK(!message && !writes && !strcmp(app, "KEEP.COM") && !command[0]);
            CHECK(!wcscmp(actual, original));
        } else {
            CHECK(message == (row == 2 ? EG_PIF_STARTDIR_ERR :
                row == 3 ? EG_PIF_STARTFILE_ERR : EG_DOS_PROG_EXTENSION));
            CHECK(!writes);
        }
        CHECK(SetCurrentDirectoryW(original));
    }
    CHECK(SetCurrentDirectoryW(expected));
    CHECK(DeleteFileOem(files[0]) && DeleteFileOem(files[1]));
    CHECK(SetCurrentDirectoryW(original));
    CHECK(RemoveDirectoryW(expected) && RemoveDirectoryW(L"O37PIF"));
    puts("S37_ORIGINAL_PIF_CALLER_OEM_PATH_ERRORS_CLEANUP_OK");
    fprintf(report, "S37_ORIGINAL_PIF_CALLER_OEM_PATH_ERRORS_CLEANUP_OK\n");
    fclose(report);
    return 0;
}
