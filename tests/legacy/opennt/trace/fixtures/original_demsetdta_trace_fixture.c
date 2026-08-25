#include <stdint.h>
#include <stdio.h>

#include "historical_bios_bridge_v1.h"

typedef void (*runner_bios_entry)(void);

extern runner_bios_entry BIOS[];
extern int DemInit(int argc, char *argv[]);
extern uint8_t *runner_ccpu_sm0_ram(void);
extern void setAX(uint16_t value);
extern void setCX(uint16_t value);
extern void setDX(uint16_t value);
extern void setSI(uint16_t value);
extern void setCS(uint16_t value);
extern void setDS(uint16_t value);
extern void setIP(uint16_t value);
extern uint16_t getIP(void);
extern void *pulDTALocation;
extern void *pusCurrentPDB;
extern void *pExtendedError;
extern void *pSFTHead;

enum {
    runner_data_segment = 0x0100u,
    runner_data_base = runner_data_segment << 4,
    runner_dta_offset = 0x0010u,
    runner_pdb_offset = 0x0020u,
    runner_error_offset = 0x0030u,
    runner_wow_offset = 0x0040u,
    runner_sft_offset = 0x0050u,
    runner_wow_sft_offset = 8u * sizeof(uint32_t)
};

static void runner_store_u32(uint8_t *destination, uint32_t value)
{
    destination[0] = (uint8_t)value;
    destination[1] = (uint8_t)(value >> 8);
    destination[2] = (uint8_t)(value >> 16);
    destination[3] = (uint8_t)(value >> 24);
}

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

    /* DOSWOWDATA::lpSftAddr is a far pointer; demSetDTALocation consumes
     * its low word with the same DS that NTDOS supplied. */
    runner_store_u32(ram + runner_data_base + runner_wow_offset +
                       runner_wow_sft_offset,
                       ((uint32_t)runner_data_segment << 16) |
                       runner_sft_offset);

    setAX(runner_dta_offset);
    setDX(runner_pdb_offset);
    setCX(runner_error_offset);
    setSI(runner_wow_offset);
    setDS(runner_data_segment);
    setCS(0x0070u);
    setIP(0x0478u);
    ram[0x0b78u] = 0x1bu;
    BIOS[0x50u]();

    fprintf(stderr,
            "demsetdta fixture: ip=%04x dta=%p pdb=%p error=%p sft=%p\n",
            getIP(), pulDTALocation, pusCurrentPDB, pExtendedError, pSFTHead);
    if (getIP() != 0x0479u ||
        pulDTALocation != ram + runner_data_base + runner_dta_offset ||
        pusCurrentPDB != ram + runner_data_base + runner_pdb_offset ||
        pExtendedError != ram + runner_data_base + runner_error_offset ||
        pSFTHead != ram + runner_data_base + runner_sft_offset) {
        result = 8;
    }

cleanup:
    runner_historical_bios_bridge_v1_terminate();
    return result;
}
