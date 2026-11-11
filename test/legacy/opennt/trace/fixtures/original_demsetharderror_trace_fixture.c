#include <stdint.h>
#include <stdio.h>

#include "historical_bios_bridge_v1.h"

typedef void (*runner_bios_entry)(void);

extern runner_bios_entry BIOS[];
extern int DemInit(int argc, char *argv[]);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern void setBX(uint16_t value);
extern void setDX(uint16_t value);
extern void setCS(uint16_t value);
extern void setDS(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getIP(void);
extern void *pHardErrPacket;
extern void *pDeviceChain;

enum {
    runner_data_segment = 0x0100u,
    runner_data_base = runner_data_segment << 4,
    runner_vhe_offset = 0x0010u,
    runner_device_offset = 0x0040u
};

int main(int argc, char *argv[])
{
    uint8_t *ram;
    int result = 0;

    if (!runner_historical_bios_bridge_v1_initialize()) return 1;
    if (!DemInit(argc, argv)) {
        result = 2;
        goto cleanup;
    }
    ram = runner_ccpu_sm0_ram();
    if (ram == NULL) {
        result = 4;
        goto cleanup;
    }

    setDX(runner_vhe_offset);
    setBX(runner_device_offset);
    setDS(runner_data_segment);
    setCS(0x0070u);
    setIP(0x0478u);
    ram[0x0b78u] = 0x32u;
    BIOS[0x50u]();

    fprintf(stderr, "demsetharderror fixture: ip=%04x vhe=%p device=%p\n",
            getIP(), pHardErrPacket, pDeviceChain);
    if (getIP() != 0x0479u ||
        pHardErrPacket != ram + runner_data_base + runner_vhe_offset ||
        pDeviceChain != ram + runner_data_base + runner_device_offset) {
        result = 8;
    }

cleanup:
    runner_historical_bios_bridge_v1_terminate();
    return result;
}
