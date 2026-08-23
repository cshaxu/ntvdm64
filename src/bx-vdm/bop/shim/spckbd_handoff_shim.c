#include "spckbd_handoff_shim.h"

#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

#include <string.h>

typedef struct bx_ntvdm_spckbd_handoff_call {
    uint16_t cs, ds, si, ax;
    uint8_t screen_state, active, failed, cf;
} bx_ntvdm_spckbd_handoff_call;

static __declspec(thread) bx_ntvdm_spckbd_handoff_call g_call;

static int real_address(uint16_t segment, uint16_t offset, uint32_t bytes,
    uint32_t *address)
{
    uint32_t base = (uint32_t)segment << 4;
    if (address == 0 || bytes == 0u || offset > UINT16_MAX - (bytes - 1u) ||
        base > UINT32_MAX - (uint32_t)offset) return 0;
    *address = base + (uint32_t)offset;
    return 1;
}

int bx_ntvdm_spckbd_handoff_begin(uint16_t cs, uint16_t ds, uint16_t si,
    uint16_t ax, uint8_t screen_state)
{
    if (g_call.active != 0u || ax != 0xbeefu || screen_state != 2u) return 0;
    memset(&g_call, 0, sizeof(g_call));
    g_call.cs = cs; g_call.ds = ds; g_call.si = si; g_call.ax = ax;
    g_call.screen_state = screen_state; g_call.active = 1u;
    return 1;
}

int bx_ntvdm_spckbd_handoff_end(void)
{
    int ok = g_call.active != 0u && g_call.failed == 0u && g_call.cf == 1u;
    memset(&g_call, 0, sizeof(g_call));
    return ok;
}

void bx_ntvdm_spckbd_handoff_reset(void)
{
    memset(&g_call, 0, sizeof(g_call));
}

word getCS(void) { return g_call.cs; }
word getDS(void) { return g_call.ds; }
word getSI(void) { return g_call.si; }
word getAX(void) { return g_call.ax; }
uint8_t getNtScreenState(void) { return g_call.screen_state; }

int bx_ntvdm_spckbd_table_word(uint16_t index, word *value)
{
    uint32_t address;
    uint8_t bytes[2];
    if (g_call.active == 0u || value == 0 || index > 32u ||
        g_call.si > UINT16_MAX - index * 2u ||
        !real_address(g_call.cs, (uint16_t)(g_call.si + index * 2u),
            sizeof(bytes), &address) ||
        !bx_ntvdm_mantle_checked_ram_read_v1(address, bytes, sizeof(bytes))) {
        g_call.failed = 1u;
        return 0;
    }
    *value = (word)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
    return 1;
}

void sas_store_no_check(uint32_t address, uint8_t value)
{
    if (g_call.active == 0u || !bx_ntvdm_mantle_checked_ram_write_v1(address,
            &value, 1u)) g_call.failed = 1u;
}

void bx_ntvdm_spckbd_handoff_fail(void) { if (g_call.active != 0u) g_call.failed = 1u; }
int bx_ntvdm_spckbd_handoff_failed(void) { return g_call.failed != 0u; }
void setCF(uint16_t value) { if (g_call.active != 0u) g_call.cf = value == 1u; }
