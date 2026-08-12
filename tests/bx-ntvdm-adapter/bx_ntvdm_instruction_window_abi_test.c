#include <stdint.h>

#include "bx_ntvdm_instruction_window_abi.h"

int main(void)
{
    static const uint8_t sample[] = { 0x11u, 0x22u, 0x33u };
    bx_ntvdm_instruction_window_v1 window;

    bx_ntvdm_instruction_window_v1_capture(&window, sample, 3u);
    if (sizeof(window) != 32u || !bx_ntvdm_instruction_window_v1_valid(&window)
        || window.valid_bytes != 3u || window.bytes[0] != 0x11u ||
        window.bytes[2] != 0x33u) return 1;
    bx_ntvdm_instruction_window_v1_capture(&window, sample, 16u);
    if (!bx_ntvdm_instruction_window_v1_valid(&window) ||
        window.valid_bytes != 0u) return 2;
    bx_ntvdm_instruction_window_v1_capture(&window, 0, 1u);
    if (!bx_ntvdm_instruction_window_v1_valid(&window) ||
        window.valid_bytes != 0u) return 3;
    window.reserved0 = 1u;
    if (bx_ntvdm_instruction_window_v1_valid(&window)) return 4;
    return bx_ntvdm_instruction_window_v1_valid(0) ? 5 : 0;
}
