#include "softpc_mouse_vector_shim.h"

#include "bx-mantle/bx_ntvdm_machine_stage_v1.h"

#include <string.h>

struct bx_ntvdm_softpc_mouse_vector_call {
    uint16_t cs;
    uint16_t bx;
    uint32_t active;
    uint32_t failed;
};

static __declspec(thread) struct bx_ntvdm_softpc_mouse_vector_call g_call;

static int real_address(uint16_t segment, uint16_t offset, uint32_t bytes,
    uint32_t *address)
{
    uint32_t base = (uint32_t)segment << 4;
    if (address == 0 || bytes == 0u || offset > UINT16_MAX - (bytes - 1u) ||
        base > UINT32_MAX - (uint32_t)offset) return 0;
    *address = base + (uint32_t)offset;
    return 1;
}

int bx_ntvdm_softpc_mouse_vector_begin(uint16_t cs, uint16_t bx)
{
    if (g_call.active != 0u) return 0;
    memset(&g_call, 0, sizeof(g_call));
    g_call.cs = cs;
    g_call.bx = bx;
    g_call.active = 1u;
    return 1;
}

int bx_ntvdm_softpc_mouse_vector_end(void)
{
    int ok = g_call.active != 0u && g_call.failed == 0u;
    memset(&g_call, 0, sizeof(g_call));
    return ok;
}

int bx_ntvdm_softpc_mouse_vector_load_table_word(uint16_t byte_offset,
    bx_ntvdm_softpc_mouse_word *value)
{
    uint8_t bytes[2];
    uint16_t offset;
    uint32_t address;
    if (value == 0 || g_call.active == 0u ||
        g_call.bx > UINT16_MAX - byte_offset ||
        !real_address(g_call.cs, (uint16_t)(g_call.bx + byte_offset),
            sizeof(bytes), &address) ||
        !bx_ntvdm_mantle_checked_ram_read_v1(address, bytes, sizeof(bytes))) {
        g_call.failed = 1u;
        return 0;
    }
    offset = (uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8);
    *value = offset;
    return 1;
}

int bx_ntvdm_softpc_mouse_vector_store_int33(
    bx_ntvdm_softpc_mouse_word offset,
    bx_ntvdm_softpc_mouse_word segment)
{
    uint8_t vector[4];
    if (g_call.active == 0u) return 0;
    vector[0] = (uint8_t)offset;
    vector[1] = (uint8_t)(offset >> 8);
    vector[2] = (uint8_t)segment;
    vector[3] = (uint8_t)(segment >> 8);
    if (!bx_ntvdm_mantle_checked_ram_write_v1(0xccu, vector,
            sizeof(vector))) {
        g_call.failed = 1u;
        return 0;
    }
    return 1;
}
