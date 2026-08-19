/* Direct table witness for src/opennt/base/mvdm/dos/dem/demdisp.c. */
#include <stdio.h>

#include "bop/shim/demdisp_shim.h"

int main(void)
{
    static const uint8_t noops[] = { 0x1fu, 0x24u, 0x26u, 0x28u,
        0x2bu, 0x40u, 0x42u, 0x43u };
    size_t index;

    if (bx_ntvdm_demdisp_service_count() != SVC_DEMLASTSVC ||
        SVC_DEMLASTSVC != 73u) return 1;
    for (index = 0; index != SVC_DEMLASTSVC; ++index)
        if (bx_ntvdm_demdisp_handler_at((uint8_t)index) == 0) return 2;
    for (index = 0; index != sizeof(noops); ++index)
        if (bx_ntvdm_demdisp_handler_at(noops[index]) != demNotYetImplemented)
            return 3;
    if (bx_ntvdm_demdisp_handler_at(0x1du) != demSetV86KernelAddr ||
        bx_ntvdm_demdisp_handler_at(SVC_DEMLASTSVC) != 0) return 4;
    puts("T230 S10 direct OpenNT demdisp table: 73 original dispatch slots verified");
    return 0;
}
