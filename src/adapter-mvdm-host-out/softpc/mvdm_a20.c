#include "mvdm_a20.h"

#include "adapter-bochs/machine_facade.h"

void sas_enable_20_bit_wrapping(void)
{
    (void)machine_facade_set_a20(0u);
}

void sas_disable_20_bit_wrapping(void)
{
    (void)machine_facade_set_a20(1u);
}

int sas_twenty_bit_wrapping_enabled(void)
{
    uint32_t enabled = 0u;

    return machine_facade_get_a20(&enabled) && enabled == 0u;
}
