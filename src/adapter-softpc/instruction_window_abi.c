#include "instruction_window_abi.h"

#include <string.h>

void runtime_instruction_window_v1_capture(
    runtime_instruction_window_v1 *window, const uint8_t *bytes,
    uint32_t valid_bytes)
{
    if (window == 0) return;
    memset(window, 0, sizeof(*window));
    window->magic = RUNTIME_INSTRUCTION_WINDOW_ABI_MAGIC;
    window->abi_version = RUNTIME_INSTRUCTION_WINDOW_ABI_VERSION;
    window->struct_bytes = sizeof(*window);
    if (bytes == 0 || valid_bytes > RUNTIME_INSTRUCTION_WINDOW_V1_MAX_BYTES)
        return;
    window->valid_bytes = valid_bytes;
    if (valid_bytes != 0u) memcpy(window->bytes, bytes, valid_bytes);
}

int runtime_instruction_window_v1_valid(
    const runtime_instruction_window_v1 *window)
{
    return window != 0 &&
        window->magic == RUNTIME_INSTRUCTION_WINDOW_ABI_MAGIC &&
        window->abi_version == RUNTIME_INSTRUCTION_WINDOW_ABI_VERSION &&
        window->struct_bytes == sizeof(*window) &&
        window->valid_bytes <= RUNTIME_INSTRUCTION_WINDOW_V1_MAX_BYTES &&
        window->reserved0 == 0u;
}
