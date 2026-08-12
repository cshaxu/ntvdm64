#include <stdint.h>

#include "bx_ntvdm_guest_write_abi.h"

int main(void)
{
    bx_ntvdm_guest_write_v1 write;
    bx_ntvdm_guest_write_v1_initialize(&write, UINT64_C(0x8ae0), 0x20u, 0u);
    if (sizeof(write) != 40u || !bx_ntvdm_guest_write_v1_preflight(&write,
        0x100000u, 0x20u)) return 1;
    write.payload_offset = 1u;
    if (bx_ntvdm_guest_write_v1_preflight(&write, 0x100000u, 0x20u)) return 2;
    write.payload_offset = 0u;
    write.guest_physical_address = UINT64_C(0xfffff);
    if (bx_ntvdm_guest_write_v1_preflight(&write, 0x100000u, 0x20u)) return 3;
    write.guest_physical_address = UINT64_C(0x8ae0);
    write.flags = 1u;
    if (bx_ntvdm_guest_write_v1_preflight(&write, 0x100000u, 0x20u)) return 4;
    bx_ntvdm_guest_write_v1_initialize(0, 0u, 0u, 0u);
    return bx_ntvdm_guest_write_v1_preflight(0, 0x100000u, 0u) ? 5 : 0;
}
