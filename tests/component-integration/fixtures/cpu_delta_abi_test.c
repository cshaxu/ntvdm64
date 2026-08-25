#include "cpu_delta_abi.h"

#include <stdio.h>

int main(void)
{
    runtime_cpu_delta delta;
    runtime_cpu_delta_initialize(&delta);
    if (!runtime_cpu_delta_valid(&delta) || delta.gpr16_write_mask != 0u)
        return 1;
    if (!runtime_cpu_delta_set_gpr16(&delta, 0u, 0x027fu) ||
        delta.gpr16_write_mask != 1u || delta.gpr16_values[0] != 0x027fu)
        return 2;
    if (runtime_cpu_delta_set_gpr16(&delta, 8u, 0u)) return 3;
    delta.gpr16_write_mask = 0x100u;
    if (runtime_cpu_delta_valid(&delta)) return 4;
    puts("runtime CPU delta ABI: narrow register-mask checks verified");
    return 0;
}
