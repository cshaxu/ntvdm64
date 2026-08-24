/* Bounded ABI fixture for the direct-CLI first-PIF-query transport. */
#include <windows.h>
#include <string.h>

#include "vdmapi.h"

extern void ntdos64_direct_cli_transport_end(void);
extern BOOL ntdos64_direct_cli_transport_begin(const char *pif_path,
                                               BOOL first_vdm);

static void clear_info(VDMINFO *info, char *buffer, USHORT buffer_length) {
    memset(info, 0, sizeof(*info));
    info->VDMState = ASKING_FOR_PIF | ASKING_FOR_DOS_BINARY;
    info->PifFile = buffer;
    info->PifLen = buffer_length;
}

int main(void) {
    static const char path[] = "R:\\session\\_default.pif";
    VDMINFO info;
    char buffer[sizeof(path)];
    char snapshot[sizeof(buffer)];

    ntdos64_direct_cli_transport_end();
    if (GetNextVDMCommand(NULL)) return 1;
    if (!ntdos64_direct_cli_transport_begin(path, TRUE)) return 2;
    if (!GetNextVDMCommand(NULL)) return 3;

    memset(buffer, 0x5a, sizeof(buffer));
    clear_info(&info, buffer, sizeof(buffer) - 1);
    memcpy(snapshot, buffer, sizeof(buffer));
    if (GetNextVDMCommand(&info)) return 4;
    if (memcmp(buffer, snapshot, sizeof(buffer)) != 0) return 5;

    clear_info(&info, buffer, sizeof(buffer));
    if (!GetNextVDMCommand(&info)) return 6;
    if (strcmp(buffer, path) != 0) return 7;

    memcpy(snapshot, buffer, sizeof(buffer));
    if (GetNextVDMCommand(&info)) return 8;
    if (memcmp(buffer, snapshot, sizeof(buffer)) != 0) return 9;

    ntdos64_direct_cli_transport_end();
    if (!ntdos64_direct_cli_transport_begin(path, FALSE)) return 10;
    if (GetNextVDMCommand(NULL)) return 11;
    clear_info(&info, buffer, sizeof(buffer));
    info.VDMState = ASKING_FOR_WOW_BINARY;
    memcpy(snapshot, buffer, sizeof(buffer));
    if (GetNextVDMCommand(&info)) return 12;
    if (memcmp(buffer, snapshot, sizeof(buffer)) != 0) return 13;

    return 0;
}
