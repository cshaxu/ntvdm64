#include "bop/shim/command_pif_shim.h"

#define WINNT 1
#include <pif.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int write_exact(HANDLE file, const void *data, DWORD bytes)
{
    DWORD written = 0u;
    return WriteFile(file, data, bytes, &written, NULL) && written == bytes;
}

int main(void)
{
    CHAR directory[MAX_PATH + 1u], pif_path[MAX_PATH + 1u], target_path[MAX_PATH + 1u];
    STDPIF standard_pif;
    PIFEXTHDR extension_header;
    HANDLE file;
    PIF_DATA parsed;
    const CHAR target_bytes[] = { 0x90, 0xc3 };
    DWORD directory_bytes;

    directory_bytes = GetTempPathA((DWORD)sizeof(directory), directory);
    if (directory_bytes == 0u || directory_bytes >= sizeof(directory) ||
        sprintf_s(pif_path, sizeof(pif_path), "%sbx-ntvdm-command-fixture.pif", directory) < 0 ||
        sprintf_s(target_path, sizeof(target_path), "%sbx-ntvdm-target.com", directory) < 0) return 1;
    DeleteFileA(pif_path);
    DeleteFileA(target_path);
    file = CreateFileA(target_path, GENERIC_WRITE, 0u, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !write_exact(file, target_bytes, sizeof(target_bytes))) return 2;
    CloseHandle(file);

    memset(&standard_pif, 0, sizeof(standard_pif));
    memset(&extension_header, 0, sizeof(extension_header));
    strcpy_s(standard_pif.appname, sizeof(standard_pif.appname), "OpenNT PIF fixture");
    strcpy_s(standard_pif.startfile, sizeof(standard_pif.startfile), target_path);
    strcpy_s(standard_pif.defpath, sizeof(standard_pif.defpath), directory);
    strcpy_s(standard_pif.params, sizeof(standard_pif.params), "-from-pif");
    standard_pif.MSflags = 0x10u;
    strcpy_s(extension_header.extsig, sizeof(extension_header.extsig), "MICROSOFT PIFEX");
    extension_header.extnxthdrfloff = 0xffffu;
    file = CreateFileA(pif_path, GENERIC_WRITE, 0u, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !write_exact(file, &standard_pif, sizeof(standard_pif)) ||
        !write_exact(file, &extension_header, sizeof(extension_header))) return 3;
    CloseHandle(file);

    memset(&pfdata, 0, sizeof(pfdata));
    memset(&parsed, 0, sizeof(parsed));
    parsed.AppHasPIFFile = 1;
    DosSessionId = 1u;
    if (!GetPIFData(&parsed, pif_path) || parsed.WinTitle == NULL || parsed.CmdLine == NULL ||
        parsed.StartDir == NULL || parsed.StartFile == NULL || parsed.CloseOnExit != 1 ||
        strcmp(parsed.WinTitle, "OpenNT PIF fixture") != 0 ||
        strcmp(parsed.CmdLine, "-from-pif") != 0 || strcmp(parsed.StartDir, directory) != 0 ||
        strcmp(parsed.StartFile, target_path) != 0) {
        fprintf(stderr, "parse title=%s cmd=%s dir=%s file=%s close=%d\n",
            parsed.WinTitle == NULL ? "<null>" : parsed.WinTitle,
            parsed.CmdLine == NULL ? "<null>" : parsed.CmdLine,
            parsed.StartDir == NULL ? "<null>" : parsed.StartDir,
            parsed.StartFile == NULL ? "<null>" : parsed.StartFile,
            parsed.CloseOnExit);
        return 4;
    }
    free(parsed.WinTitle);
    free(parsed.CmdLine);
    free(parsed.StartDir);
    free(parsed.StartFile);
    DeleteFileA(pif_path);
    DeleteFileA(target_path);
    puts("T234 S2 original OpenNT PIF parser: title, command, directory, target, and close-on-exit verified");
    return 0;
}
