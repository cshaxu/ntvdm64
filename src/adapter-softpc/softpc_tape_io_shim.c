#include "softpc_tape_io_shim.h"

struct bx_ntvdm_softpc_tape_io_call {
    uint16_t input_ax;
    uint16_t result_ax;
    uint32_t active;
};

static __declspec(thread) struct bx_ntvdm_softpc_tape_io_call g_call;

int bx_ntvdm_softpc_tape_io_begin(uint16_t input_ax, uint16_t *result_ax)
{
    if (result_ax == 0 || g_call.active != 0u) return 0;
    g_call.input_ax = input_ax;
    g_call.result_ax = input_ax;
    g_call.active = 1u;
    return 1;
}

int bx_ntvdm_softpc_tape_io_end(uint16_t *result_ax)
{
    if (result_ax == 0 || g_call.active == 0u) return 0;
    *result_ax = g_call.result_ax;
    g_call.active = 0u;
    return 1;
}

half_word bx_ntvdm_softpc_tape_io_getAH(void)
{
    return (half_word)(g_call.input_ax >> 8);
}

void bx_ntvdm_softpc_tape_io_setAX(half_word value)
{
    if (g_call.active != 0u) g_call.result_ax = value;
}
