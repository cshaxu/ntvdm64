#include "bochs.h"
#include "adapter-softpc/bx_ntvdm_a20_capability_v1.h"
#include "bx-mantle/minimal_machine.h"

static int call(unsigned operation, unsigned value, unsigned status,
  unsigned enabled)
{
  bx_ntvdm_a20_capability_request_v1 request = {
    BX_NTVDM_A20_CAPABILITY_V1_VERSION, operation, value };
  bx_ntvdm_a20_capability_result_v1 result;
  bx_ntvdm_a20_capability_v1_dispatch(&request, &result);
  return result.status == status && result.enabled == enabled;
}

int main()
{
  bx_ntvdm_a20_capability_request_v1 request = {
    BX_NTVDM_A20_CAPABILITY_V1_VERSION, BX_NTVDM_A20_CAPABILITY_QUERY, 0 };
  bx_ntvdm_a20_capability_result_v1 result;
  bx_ntvdm_a20_capability_v1_dispatch(&request, &result);
  if (result.status != BX_NTVDM_A20_CAPABILITY_REJECTED_LIFECYCLE) return 1;
  bx_mantle_minimal_machine_c machine;
  if (machine.initialize(0x100000, 0x100000) != BX_MANTLE_MINIMAL_MACHINE_OK)
    return 2;
  if (!call(BX_NTVDM_A20_CAPABILITY_QUERY, 0,
      BX_NTVDM_A20_CAPABILITY_OK, 1)) return 3;
  if (!call(BX_NTVDM_A20_CAPABILITY_SET, 0,
      BX_NTVDM_A20_CAPABILITY_OK, 0)) return 4;
  if (!call(BX_NTVDM_A20_CAPABILITY_SET, 2,
      BX_NTVDM_A20_CAPABILITY_REJECTED_VALUE, 0)) return 5;
  if (!call(BX_NTVDM_A20_CAPABILITY_SET, 1,
      BX_NTVDM_A20_CAPABILITY_OK, 1)) return 6;
  if (machine.cleanup() != BX_MANTLE_MINIMAL_MACHINE_OK) return 7;
  bx_ntvdm_a20_capability_v1_dispatch(&request, &result);
  return result.status == BX_NTVDM_A20_CAPABILITY_REJECTED_LIFECYCLE ? 0 : 8;
}