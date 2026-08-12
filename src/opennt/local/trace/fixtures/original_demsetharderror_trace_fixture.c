#include <stdint.h>
#include <stdio.h>

#include "historical_bios_bridge_v1.h"

typedef void (*ntdos64_bios_entry)(void);

extern ntdos64_bios_entry BIOS[];
extern int DemInit(int argc, char *argv[]);
extern uint8_t *ntdos64_ccpu_sm0_ram(void);
extern void setBX(uint16_t value);
extern void setDX(uint16_t value);
extern void setCS(uint16_t value);
extern void setDS(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getIP(void);
extern void *pHardErrPacket;
extern void *pDeviceChain;

enum {
    ntdos64_data_segment = 0x0100u,
    ntdos64_data_base = ntdos64_data_segment << 4,
    ntdos64_vhe_offset = 0x0010u,
    ntdos64_device_offset = 0x0040u
};

int main(int argc, char *argv[])
{
    uint8_t *ram;
    int result = 0;

    if (!ntdos64_historical_bios_bridge_v1_initialize()) return 1;
    if (!DemInit(argc, argv)) {
        result = 2;
        goto cleanup;
    }
    ram = ntdos64_ccpu_sm0_ram();
    if (ram == NULL) {
        result = 4;
        goto cleanup;
    }

    setDX(ntdos64_vhe_offset);
    setBX(ntdos64_device_offset);
    setDS(ntdos64_data_segment);
    setCS(0x0070u);
    setIP(0x0478u);
    ram[0x0b78u] = 0x32u;
    BIOS[0x50u]();

    fprintf(stderr, "demsetharderror fixture: ip=%04x vhe=%p device=%p\n",
            getIP(), pHardErrPacket, pDeviceChain);
    if (getIP() != 0x0479u ||
        pHardErrPacket != ram + ntdos64_data_base + ntdos64_vhe_offset ||
        pDeviceChain != ram + ntdos64_data_base + ntdos64_device_offset) {
        result = 8;
    }

cleanup:
    ntdos64_historical_bios_bridge_v1_terminate();
    return result;
}
