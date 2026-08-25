#include "bochs.h"
#include "adapter-softpc/a20_capability.h"
#include "adapter-bochs/minimal_machine.h"

static int call(unsigned operation, unsigned value, unsigned status,
  unsigned enabled)
{
  runtime_a20_capability_request request = {
    RUNTIME_A20_CAPABILITY_VERSION, operation, value };
  runtime_a20_capability_result result;
  runtime_a20_capability_dispatch(&request, &result);
  return result.status == status && result.enabled == enabled;
}

int main()
{
  runtime_a20_capability_request request = {
    RUNTIME_A20_CAPABILITY_VERSION, RUNTIME_A20_CAPABILITY_QUERY, 0 };
  runtime_a20_capability_result result;
  runtime_a20_capability_dispatch(&request, &result);
  if (result.status != RUNTIME_A20_CAPABILITY_REJECTED_LIFECYCLE) return 1;
  adapter_bochs_minimal_machine_c machine;
  if (machine.initialize(0x100000, 0x100000) != BX_MACHINE_MINIMAL_MACHINE_OK)
    return 2;
  runtime_a20_capability_set_lifecycle_active(1u);
  if (!call(RUNTIME_A20_CAPABILITY_QUERY, 0,
      RUNTIME_A20_CAPABILITY_OK, 1)) return 3;
  if (!call(RUNTIME_A20_CAPABILITY_SET, 0,
      RUNTIME_A20_CAPABILITY_OK, 0)) return 4;
  if (!call(RUNTIME_A20_CAPABILITY_SET, 2,
      RUNTIME_A20_CAPABILITY_REJECTED_VALUE, 0)) return 5;
  if (!call(RUNTIME_A20_CAPABILITY_SET, 1,
      RUNTIME_A20_CAPABILITY_OK, 1)) return 6;
  runtime_a20_capability_set_lifecycle_active(0u);
  if (machine.cleanup() != BX_MACHINE_MINIMAL_MACHINE_OK) return 7;
  runtime_a20_capability_dispatch(&request, &result);
  return result.status == RUNTIME_A20_CAPABILITY_REJECTED_LIFECYCLE ? 0 : 8;
}
