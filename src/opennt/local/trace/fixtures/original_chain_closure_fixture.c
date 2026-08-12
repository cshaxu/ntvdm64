/*
 * R1 link-only retention fixture. It observes original symbol addresses only;
 * it must not call into BIOS, nt_bop, DEM or CCPU execution.
 */
#include <stdint.h>

typedef void (*ntdos64_bop_handler)(void);

extern ntdos64_bop_handler BIOS[];
extern void MS_bop_0(void);
extern void DemDispatch(uint8_t command);

int main(void)
{
    volatile uintptr_t bios_dos = (uintptr_t)BIOS[0x50u];
    volatile uintptr_t bop_dos = (uintptr_t)MS_bop_0;
    volatile uintptr_t dispatch = (uintptr_t)DemDispatch;

    return bios_dos == 0u || bop_dos == 0u || dispatch == 0u ? 1 : 0;
}
