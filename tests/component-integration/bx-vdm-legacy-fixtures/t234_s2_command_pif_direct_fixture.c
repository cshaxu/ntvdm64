#include "opennt-bop/command/opennt_pif_composition.h"
#include "adapter-win32/facade/opennt_error_dialog_facade.h"

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
    CHAR config_path[MAX_PATH + 1u], autoexec_path[MAX_PATH + 1u];
    CHAR selected_config[MAX_PATH + 13u], selected_autoexec[MAX_PATH + 13u];
    STDPIF standard_pif;
    PIFEXTHDR extension_header;
    PIFEXTHDR nt_extension_header;
    WNTPIF31 nt_extension;
    HANDLE file;
    PIF_DATA parsed;
    const CHAR target_bytes[] = { 0x90, 0xc3 };
    DWORD directory_bytes;

    runtime_opennt_error_dialog_fixture_suppress(TRUE);
    if (runtime_command_pif_parser_message_box(0x2468u, "", "",
            RMB_ABORT | RMB_RETRY | RMB_IGNORE | RMB_ICON_STOP) != RMB_ABORT ||
        runtime_opennt_error_dialog_fixture_last_error() != 0x2468u) return 6;
    runtime_opennt_error_dialog_fixture_suppress(FALSE);

    directory_bytes = GetTempPathA((DWORD)sizeof(directory), directory);
    if (directory_bytes == 0u || directory_bytes >= sizeof(directory) ||
        sprintf_s(pif_path, sizeof(pif_path), "%sbx-ntvdm-command-fixture.pif", directory) < 0 ||
        sprintf_s(target_path, sizeof(target_path), "%sbx-ntvdm-target.com", directory) < 0 ||
        sprintf_s(config_path, sizeof(config_path), "%sbx-ntvdm-config.nt", directory) < 0 ||
        sprintf_s(autoexec_path, sizeof(autoexec_path), "%sbx-ntvdm-autoexec.nt", directory) < 0) return 1;
    DeleteFileA(pif_path);
    DeleteFileA(target_path);
    DeleteFileA(config_path);
    DeleteFileA(autoexec_path);
    file = CreateFileA(target_path, GENERIC_WRITE, 0u, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !write_exact(file, target_bytes, sizeof(target_bytes))) return 2;
    CloseHandle(file);

    memset(&standard_pif, 0, sizeof(standard_pif));
    memset(&extension_header, 0, sizeof(extension_header));
    memset(&nt_extension_header, 0, sizeof(nt_extension_header));
    memset(&nt_extension, 0, sizeof(nt_extension));
    strcpy_s(standard_pif.appname, sizeof(standard_pif.appname), "OpenNT PIF fixture");
    strcpy_s(standard_pif.startfile, sizeof(standard_pif.startfile), target_path);
    strcpy_s(standard_pif.defpath, sizeof(standard_pif.defpath), directory);
    strcpy_s(standard_pif.params, sizeof(standard_pif.params), "-from-pif");
    standard_pif.MSflags = 0x10u;
    strcpy_s(extension_header.extsig, sizeof(extension_header.extsig), "MICROSOFT PIFEX");
    extension_header.extnxthdrfloff = (WORD)(sizeof(standard_pif) + sizeof(extension_header));
    strcpy_s(nt_extension_header.extsig, sizeof(nt_extension_header.extsig), WNTHDRSIG31);
    nt_extension_header.extnxthdrfloff = 0xffffu;
    nt_extension_header.extfileoffset = (WORD)(sizeof(standard_pif) +
        sizeof(extension_header) + sizeof(nt_extension_header));
    nt_extension_header.extsizebytes = (WORD)sizeof(nt_extension);
    strcpy_s(nt_extension.nt31Prop.achConfigFile,
        sizeof(nt_extension.nt31Prop.achConfigFile), config_path);
    strcpy_s(nt_extension.nt31Prop.achAutoexecFile,
        sizeof(nt_extension.nt31Prop.achAutoexecFile), autoexec_path);
    file = CreateFileA(pif_path, GENERIC_WRITE, 0u, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !write_exact(file, &standard_pif, sizeof(standard_pif)) ||
        !write_exact(file, &extension_header, sizeof(extension_header)) ||
        !write_exact(file, &nt_extension_header, sizeof(nt_extension_header)) ||
        !write_exact(file, &nt_extension, sizeof(nt_extension))) return 3;
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
    if (!runtime_command_pif_select_config_files(pif_path, selected_config,
            selected_autoexec) || strcmp(selected_config, config_path) != 0 ||
        strcmp(selected_autoexec, autoexec_path) != 0) return 5;
    DeleteFileA(pif_path);
    DeleteFileA(target_path);
    puts("T234 S2 original OpenNT PIF parser: title, command, directory, target, close-on-exit, and early CONFIG/AUTOEXEC selection verified");
    return 0;
}
