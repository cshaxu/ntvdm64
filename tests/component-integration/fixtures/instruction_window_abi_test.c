#include <stdint.h>

#include "instruction_window_abi.h"

int main(void)
{
    static const uint8_t sample[] = { 0x11u, 0x22u, 0x33u };
    runtime_instruction_window window;

    runtime_instruction_window_capture(&window, sample, 3u);
    if (sizeof(window) != 32u || !runtime_instruction_window_valid(&window)
        || window.valid_bytes != 3u || window.bytes[0] != 0x11u ||
        window.bytes[2] != 0x33u) return 1;
    runtime_instruction_window_capture(&window, sample, 16u);
    if (!runtime_instruction_window_valid(&window) ||
        window.valid_bytes != 0u) return 2;
    runtime_instruction_window_capture(&window, 0, 1u);
    if (!runtime_instruction_window_valid(&window) ||
        window.valid_bytes != 0u) return 3;
    window.reserved0 = 1u;
    if (runtime_instruction_window_valid(&window)) return 4;
    return runtime_instruction_window_valid(0) ? 5 : 0;
}
