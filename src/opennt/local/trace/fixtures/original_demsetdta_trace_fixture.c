#include <stdint.h>
#include <stdio.h>

#include "historical_bios_bridge_v1.h"

typedef void (*ntdos64_bios_entry)(void);

extern ntdos64_bios_entry BIOS[];
extern int DemInit(int argc, char *argv[]);
extern uint8_t *ntdos64_ccpu_sm0_ram(void);
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
    ntdos64_data_segment = 0x0100u,
    ntdos64_data_base = ntdos64_data_segment << 4,
    ntdos64_dta_offset = 0x0010u,
    ntdos64_pdb_offset = 0x0020u,
    ntdos64_error_offset = 0x0030u,
    ntdos64_wow_offset = 0x0040u,
    ntdos64_sft_offset = 0x0050u,
    ntdos64_wow_sft_offset = 8u * sizeof(uint32_t)
};

static void ntdos64_store_u32(uint8_t *destination, uint32_t value)
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

    /* DOSWOWDATA::lpSftAddr is a far pointer; demSetDTALocation consumes
     * its low word with the same DS that NTDOS supplied. */
    ntdos64_store_u32(ram + ntdos64_data_base + ntdos64_wow_offset +
                       ntdos64_wow_sft_offset,
                       ((uint32_t)ntdos64_data_segment << 16) |
                       ntdos64_sft_offset);

    setAX(ntdos64_dta_offset);
    setDX(ntdos64_pdb_offset);
    setCX(ntdos64_error_offset);
    setSI(ntdos64_wow_offset);
    setDS(ntdos64_data_segment);
    setCS(0x0070u);
    setIP(0x0478u);
    ram[0x0b78u] = 0x1bu;
    BIOS[0x50u]();

    fprintf(stderr,
            "demsetdta fixture: ip=%04x dta=%p pdb=%p error=%p sft=%p\n",
            getIP(), pulDTALocation, pusCurrentPDB, pExtendedError, pSFTHead);
    if (getIP() != 0x0479u ||
        pulDTALocation != ram + ntdos64_data_base + ntdos64_dta_offset ||
        pusCurrentPDB != ram + ntdos64_data_base + ntdos64_pdb_offset ||
        pExtendedError != ram + ntdos64_data_base + ntdos64_error_offset ||
        pSFTHead != ram + ntdos64_data_base + ntdos64_sft_offset) {
        result = 8;
    }

cleanup:
    ntdos64_historical_bios_bridge_v1_terminate();
    return result;
}
