#include "softpc_emm_unavailable_shim.h"

struct bx_ntvdm_softpc_emm_unavailable_call {
    uint16_t ds;
    uint16_t dx;
    uint16_t bx;
    uint32_t active;
    uint32_t failed;
};

static __declspec(thread) struct bx_ntvdm_softpc_emm_unavailable_call g_call;

int bx_ntvdm_softpc_emm_unavailable_begin(uint16_t ds, uint16_t dx,
    uint16_t *result_bx)
{
    if (result_bx == 0 || g_call.active != 0u) return 0;
    g_call.ds = ds;
    g_call.dx = dx;
    g_call.bx = 0u;
    g_call.failed = 0u;
    g_call.active = 1u;
    return 1;
}

int bx_ntvdm_softpc_emm_unavailable_end(uint16_t *result_bx)
{
    if (result_bx == 0 || g_call.active == 0u) return 0;
    *result_bx = g_call.bx;
    g_call.active = 0u;
    return g_call.failed == 0u;
}

word bx_ntvdm_softpc_emm_get_ds(void)
{
    if (g_call.active == 0u) { g_call.failed = 1u; return 0u; }
    return g_call.ds;
}

word bx_ntvdm_softpc_emm_get_dx(void)
{
    if (g_call.active == 0u) { g_call.failed = 1u; return 0u; }
    return g_call.dx;
}

short bx_ntvdm_softpc_emm_get_total_pages(void)
{
    if (g_call.active == 0u) { g_call.failed = 1u; return 0; }
    return 0;
}

void bx_ntvdm_softpc_emm_set_bx(word value)
{
    if (g_call.active == 0u) { g_call.failed = 1u; return; }
    g_call.bx = value;
}

void bx_ntvdm_softpc_emm_dma_lim_setup(void)
{
    /* Unreachable in the admitted zero-page profile.  It stays a no-op only
       to retain the source body's conditional ordering. */
    if (g_call.active == 0u) g_call.failed = 1u;
}
