#include "bochs.h"
#include "adapter-softpc/a20_capability.h"
#include "adapter-softpc/machine_binding.h"
#include "adapter-bochs/minimal_machine.h"
#include "adapter-bochs/machine_facade.h"

static int call(unsigned operation, unsigned value, unsigned status,
  unsigned enabled)
{
  runtime_a20_capability_request_v1 request = {
    RUNTIME_A20_CAPABILITY_V1_VERSION, operation, value };
  runtime_a20_capability_result_v1 result;
  runtime_a20_capability_v1_dispatch(&request, &result);
  return result.status == status && result.enabled == enabled;
}

int main()
{
  runtime_a20_capability_request_v1 request = {
    RUNTIME_A20_CAPABILITY_V1_VERSION, RUNTIME_A20_CAPABILITY_QUERY, 0 };
  runtime_a20_capability_result_v1 result;
  runtime_a20_capability_v1_dispatch(&request, &result);
  if (result.status != RUNTIME_A20_CAPABILITY_REJECTED_LIFECYCLE) return 1;
  bx_mantle_minimal_machine_c machine;
  if (machine.initialize(0x100000, 0x100000) != BX_MANTLE_MINIMAL_MACHINE_OK)
    return 2;
  runtime_a20_capability_v1_set_lifecycle_active(1u);
  if (!runtime_machine_binding_v1_bind_a20(machine_facade_v1_get_a20,
      machine_facade_v1_set_a20)) return 9;
  if (!call(RUNTIME_A20_CAPABILITY_QUERY, 0,
      RUNTIME_A20_CAPABILITY_OK, 1)) return 3;
  if (!call(RUNTIME_A20_CAPABILITY_SET, 0,
      RUNTIME_A20_CAPABILITY_OK, 0)) return 4;
  if (!call(RUNTIME_A20_CAPABILITY_SET, 2,
      RUNTIME_A20_CAPABILITY_REJECTED_VALUE, 0)) return 5;
  if (!call(RUNTIME_A20_CAPABILITY_SET, 1,
      RUNTIME_A20_CAPABILITY_OK, 1)) return 6;
  runtime_machine_binding_v1_unbind_a20();
  runtime_a20_capability_v1_set_lifecycle_active(0u);
  if (machine.cleanup() != BX_MANTLE_MINIMAL_MACHINE_OK) return 7;
  runtime_a20_capability_v1_dispatch(&request, &result);
  return result.status == RUNTIME_A20_CAPABILITY_REJECTED_LIFECYCLE ? 0 : 8;
}
