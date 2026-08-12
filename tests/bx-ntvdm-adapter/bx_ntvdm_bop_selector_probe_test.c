#include <stdint.h>

#include "bx_ntvdm_bop_selector_probe.h"

int main(void)
{
    static const uint8_t admitted[] = { 0xc4u, 0xc4u, 0x50u };
    static const uint8_t rejected_selector[] = { 0xc4u, 0xc4u, 0x51u };
    static const uint8_t rejected_opcode[] = { 0x0fu, 0x0bu, 0x50u };
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bios_owner_v1 owner;

    bx_ntvdm_instruction_window_v1_capture(&window, admitted, 3u);
    if (!bx_ntvdm_bop_selector_probe_v1(&window, &owner) ||
        owner != BX_NTVDM_BIOS_OWNER_OPENNT_MS_BOP_0) return 1;
    bx_ntvdm_instruction_window_v1_capture(&window, rejected_selector, 3u);
    if (bx_ntvdm_bop_selector_probe_v1(&window, &owner) ||
        owner != BX_NTVDM_BIOS_OWNER_NONE) return 2;
    bx_ntvdm_instruction_window_v1_capture(&window, rejected_opcode, 3u);
    if (bx_ntvdm_bop_selector_probe_v1(&window, &owner)) return 3;
    bx_ntvdm_instruction_window_v1_capture(&window, admitted, 2u);
    if (bx_ntvdm_bop_selector_probe_v1(&window, &owner)) return 4;
    return bx_ntvdm_bop_selector_probe_v1(&window, 0) ? 5 : 0;
}
