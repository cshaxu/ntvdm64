#include "machine_binding.h"

static runtime_machine_binding_a20_get_v1 runtime_machine_binding_get_a20;
static runtime_machine_binding_a20_set_v1 runtime_machine_binding_set_a20;

int runtime_machine_binding_v1_bind_a20(
    runtime_machine_binding_a20_get_v1 get_a20,
    runtime_machine_binding_a20_set_v1 set_a20)
{
    if (get_a20 == 0 || set_a20 == 0 ||
        runtime_machine_binding_get_a20 != 0 ||
        runtime_machine_binding_set_a20 != 0) return 0;
    runtime_machine_binding_get_a20 = get_a20;
    runtime_machine_binding_set_a20 = set_a20;
    return 1;
}

void runtime_machine_binding_v1_unbind_a20(void)
{
    runtime_machine_binding_get_a20 = 0;
    runtime_machine_binding_set_a20 = 0;
}

int runtime_machine_binding_v1_get_a20(uint32_t *enabled)
{
    return runtime_machine_binding_get_a20 != 0 &&
        runtime_machine_binding_get_a20(enabled);
}

int runtime_machine_binding_v1_set_a20(uint32_t enabled)
{
    return runtime_machine_binding_set_a20 != 0 &&
        runtime_machine_binding_set_a20(enabled);
}
