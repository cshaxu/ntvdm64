#include <stdint.h>
#include <stdio.h>

#include "historical_bios_bridge_v1.h"
#include "historical_dem_byob_profile_v1.h"
#include "oem_facade_v1.h"

typedef void (*runner_bios_entry)(void);

extern runner_bios_entry BIOS[];
extern int DemInit(int argc, char *argv[]);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern void setCS(uint16_t value);
extern void setIP(uint16_t value);
extern void setDI(uint16_t value);
extern uint16_t getIP(void);

int main(int argc, char *argv[])
{
    uint8_t *ram;
    int result = 0;

    if (argc != 2) return 1;
    fprintf(stderr, "demload fixture: bridge init\n");
    if (!runner_historical_bios_bridge_v1_initialize()) return 2;
    fprintf(stderr, "demload fixture: original DemInit\n");
    if (!runner_oem_facade_v1_configure_resource_root(argv[1]) ||
        !DemInit(argc, argv) ||
        !runner_historical_dem_byob_profile_v1_replace_directory(argv[1])) {
        result = 4;
        goto cleanup;
    }
    fprintf(stderr, "demload fixture: historical state\n");
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL) {
        result = 8;
        goto cleanup;
    }
    setCS(0x0070u);
    setIP(0x0478u);
    setDI(0x08b0u);
    ram[0x0b78u] = 0x11u;
    fprintf(stderr, "demload fixture: BIOS selector 50\n");
    BIOS[0x50u]();
    fprintf(stderr, "demload fixture: BIOS selector 50 returned\n");
    if (getIP() != 0x0479u || ram[0x8b00u] != 0xe9u ||
        ram[0x8b01u] != 0xa9u || ram[0x8b02u] != 0x51u) {
        result = 16;
    }

cleanup:
    runner_oem_facade_v1_reset();
    runner_historical_bios_bridge_v1_terminate();
    return result;
}
