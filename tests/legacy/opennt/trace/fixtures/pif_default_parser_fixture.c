/* Bounded original nt_pif.c default-path fixture using an owned temp file. */
#include <windows.h>
#include <string.h>

#include "pif.h"
#include "nt_pif.h"

extern unsigned long ntdos64_pif_fixture_unexpected_callbacks(void);

ULONG DosSessionId = 0;

int main(void) {
    char temporary_directory[MAX_PATH];
    char file_name[MAX_PATH];
    STDPIF standard_pif;
    PIFEXTHDR extension_header;
    PIF_DATA data;
    HANDLE file;
    DWORD written;
    int result = 1;

    if (GetTempPathA(sizeof(temporary_directory), temporary_directory) == 0 ||
        GetTempFileNameA(temporary_directory, "npf", 0, file_name) == 0) {
        return 1;
    }

    file = CreateFileA(file_name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_TEMPORARY, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        DeleteFileA(file_name);
        return 2;
    }

    memset(&standard_pif, 0, sizeof(standard_pif));
    memset(&extension_header, 0, sizeof(extension_header));
    extension_header.extnxthdrfloff = LASTHEADER;
    if (!WriteFile(file, &standard_pif, sizeof(standard_pif), &written, NULL) ||
        written != sizeof(standard_pif) ||
        !WriteFile(file, &extension_header, sizeof(extension_header), &written,
                   NULL) || written != sizeof(extension_header) ||
        !CloseHandle(file)) {
        DeleteFileA(file_name);
        return 3;
    }

    memset(&data, 0, sizeof(data));
    if (!GetPIFData(&data, file_name)) {
        result = 4;
    } else if (data.emsdes != DEFAULTEMSLMT ||
               data.xmsdes != DEFAULTXMSLMT ||
               data.idledetect != DEFAULTIDLETM || data.reskey != 0 ||
               data.ShortMod != 0 || data.ShortScan != 0 ||
               ntdos64_pif_fixture_unexpected_callbacks() != 0) {
        result = 5;
    } else {
        result = 0;
    }

    DeleteFileA(file_name);
    return result;
}
