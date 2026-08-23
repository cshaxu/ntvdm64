#include "softpc_printer_openclose_shim.h"

#include <string.h>

#define BX_NTVDM_SOFTPC_PRINTER_PORTS 3u

struct bx_ntvdm_softpc_printer_call {
    uint16_t si;
    uint16_t dx;
    uint32_t active;
    uint32_t unsupported;
};

static __declspec(thread) struct bx_ntvdm_softpc_printer_call g_call;
static __declspec(thread) uint32_t g_dos_open[BX_NTVDM_SOFTPC_PRINTER_PORTS];

int bx_ntvdm_softpc_printer_openclose_begin(uint16_t si, uint16_t dx)
{
    if (g_call.active != 0u) return 0;
    memset(&g_call, 0, sizeof(g_call));
    g_call.si = si;
    g_call.dx = dx;
    g_call.active = 1u;
    return 1;
}

int bx_ntvdm_softpc_printer_openclose_end(void)
{
    int ok = g_call.active != 0u && g_call.unsupported == 0u;
    memset(&g_call, 0, sizeof(g_call));
    return ok;
}

uint16_t bx_ntvdm_softpc_printer_get_si(void) { return g_call.si; }
uint16_t bx_ntvdm_softpc_printer_get_dx(void) { return g_call.dx; }

void bx_ntvdm_softpc_printer_host_lpt_dos_open(int adapter)
{
    if (g_call.active == 0u || adapter < 0 ||
        (uint32_t)adapter >= BX_NTVDM_SOFTPC_PRINTER_PORTS) {
        g_call.unsupported = 1u;
        return;
    }
    /* Source parity: nt_lpt.c sets only `dos_opened` here.  Endpoint open,
     * HANDLE allocation and output remain later printer packages. */
    g_dos_open[adapter] = 1u;
}

void bx_ntvdm_softpc_printer_host_lpt_dos_close(int adapter)
{
    if (g_call.active == 0u || adapter < 0 ||
        (uint32_t)adapter >= BX_NTVDM_SOFTPC_PRINTER_PORTS) {
        g_call.unsupported = 1u;
        return;
    }
    /* The original first closes an active LPT endpoint.  This profile never
     * owns one, then preserves the following `dos_opened = FALSE` update. */
    g_dos_open[adapter] = 0u;
}

void bx_ntvdm_softpc_printer_unsupported(void)
{
    if (g_call.active != 0u) g_call.unsupported = 1u;
}

void bx_ntvdm_softpc_printer_openclose_reset(void)
{
    memset(&g_call, 0, sizeof(g_call));
    memset(g_dos_open, 0, sizeof(g_dos_open));
}

int bx_ntvdm_softpc_printer_dos_open_copy(uint16_t adapter, uint32_t *opened)
{
    if (opened == 0 || adapter >= BX_NTVDM_SOFTPC_PRINTER_PORTS) return 0;
    *opened = g_dos_open[adapter];
    return 1;
}
