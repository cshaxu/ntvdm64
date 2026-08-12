#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "historical_bios_bridge_v1.h"
#include "historical_dem_byob_profile_v1.h"
#include "oem_facade_v1.h"

extern int DemInit(int argc, char *argv[]);
extern int IsDebuggee(void);
extern void demLoadDos(void);
extern uint8_t *ntdos64_ccpu_sm0_ram(void);
extern void setDI(uint16_t value);
extern uint8_t *c_GetLinAdd(uint32_t address);
extern HANDLE WINAPI CreateFileOem(LPCSTR path, DWORD access, DWORD share,
                                   LPSECURITY_ATTRIBUTES security,
                                   DWORD disposition, DWORD flags,
                                   HANDLE template_file);

int main(int argc, char *argv[])
{
    uint8_t *ram;
    char ntdos_path[MAX_PATH + 16];
    HANDLE probe;
    uint8_t *load_address;
    DWORD bytes_read;
    int result = 0;

    if (argc != 2) return 1;
    fprintf(stderr, "direct demload fixture: bridge init\n");
    if (!ntdos64_historical_bios_bridge_v1_initialize()) return 2;
    fprintf(stderr, "direct demload fixture: original DemInit\n");
    if (!ntdos64_oem_facade_v1_configure_resource_root(argv[1]) ||
        !DemInit(argc, argv) ||
        !ntdos64_historical_dem_byob_profile_v1_replace_directory(argv[1])) {
        result = 4;
        goto cleanup;
    }
    ram = ntdos64_ccpu_sm0_ram();
    if (ram == NULL) {
        result = 8;
        goto cleanup;
    }
    setDI(0x08b0u);
    fprintf(stderr, "direct demload fixture: original IsDebuggee=%d\n", IsDebuggee());
    if (strlen(argv[1]) + sizeof("\\ntdos.sys") > sizeof(ntdos_path)) {
        result = 12;
        goto cleanup;
    }
    strcpy(ntdos_path, argv[1]);
    strcat(ntdos_path, "\\ntdos.sys");
    probe = CreateFileOem(ntdos_path, GENERIC_READ, FILE_SHARE_READ, NULL,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    fprintf(stderr, "direct demload fixture: facade probe=%p error=%lu\n",
            probe, GetLastError());
    if (probe == INVALID_HANDLE_VALUE) {
        result = 13;
        goto cleanup;
    }
    load_address = c_GetLinAdd(0x8b00u);
    fprintf(stderr, "direct demload fixture: sas target=%p\n", load_address);
    if (load_address == NULL ||
        !ReadFile(probe, load_address, 3u, &bytes_read, NULL) ||
        bytes_read != 3u || load_address[0] != 0xe9u ||
        load_address[1] != 0xa9u || load_address[2] != 0x51u) {
        result = 14;
        CloseHandle(probe);
        goto cleanup;
    }
    CloseHandle(probe);
    fprintf(stderr, "direct demload fixture: original demLoadDos\n");
    demLoadDos();
    fprintf(stderr, "direct demload fixture: original demLoadDos returned\n");
    if (ram[0x8b00u] != 0xe9u || ram[0x8b01u] != 0xa9u ||
        ram[0x8b02u] != 0x51u) {
        result = 16;
    }

cleanup:
    ntdos64_oem_facade_v1_reset();
    ntdos64_historical_bios_bridge_v1_terminate();
    return result;
}
