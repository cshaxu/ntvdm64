#include "bochs.h"
#include "bx-mantle/bx_ntvdm_generic_ud_bridge.h"
#include "bx-mantle/bx_ntvdm_machine_stage_v1.h"
#include "bop/shim/dpmi_xmem_record_adapter.h"

#include <string.h>

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
  (void)event; (void)outcome; return 0;
}

static int begin_stage(void)
{
  static const Bit8u halt[] = { 0xf4u };
  struct bx_ntvdm_machine_stage_v1_request request;
  bx_ntvdm_machine_stage_v1_request_clear(&request);
  request.guest_memory_bytes = UINT64_C(0x200000);
  request.reserved_memory_base = UINT64_C(0x100000);
  request.reserved_memory_bytes = UINT64_C(0x100000);
  bx_ntvdm_mechanical_action_v1_clear(&request.startup_action);
  request.startup_action.action_id = 1u;
  request.startup_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = sizeof(halt);
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = sizeof(halt);
  memcpy(request.startup_action.payload, halt, sizeof(halt));
  request.preserved_state_address = 0x600u;
  request.preserved_state_bytes = 1u;
  return bx_ntvdm_machine_stage_v1_request_valid(&request) &&
    bx_ntvdm_machine_stage_v1_begin(&request) == BX_NTVDM_MACHINE_STAGE_V1_OK;
}

int main(void)
{
  struct bx_ntvdm_dpmi_xmem_record_adapter adapter;
  uint32_t first_address, first_id, second_address, second_id;
  bx_ntvdm_dpmi_xmem_record_adapter_clear(&adapter);
  if (!bx_ntvdm_dpmi_xmem_record_adapter_valid(&adapter)) return 1;
  if (bx_ntvdm_dpmi_xmem_record_adapter_allocate(&adapter, 0x10u, 0x10000u,
      &first_address, &first_id) != BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_MANTLE_FAILURE)
    return 2;
  if (!begin_stage()) return 3;
  if (bx_ntvdm_dpmi_xmem_record_adapter_allocate(&adapter, 0x10u, 0x10000u,
      &first_address, &first_id) != BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK ||
      first_address != 0x100000u || first_id == 0u) return 4;
  if (bx_ntvdm_dpmi_xmem_record_adapter_allocate(&adapter, 0x20u, 0x10000u,
      &second_address, &second_id) != BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK ||
      second_address != 0x110000u || second_id == first_id) return 5;
  if (bx_ntvdm_dpmi_xmem_record_adapter_release(&adapter, first_id + 100u) !=
      BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_REJECTED_ID || adapter.record_count != 2u) return 6;
  if (bx_ntvdm_dpmi_xmem_record_adapter_release_owner(&adapter, 0x10u) !=
      BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK || adapter.record_count != 1u) return 7;
  if (bx_ntvdm_dpmi_xmem_record_adapter_reset(&adapter) !=
      BX_NTVDM_DPMI_XMEM_RECORD_ADAPTER_OK || adapter.record_count != 0u ||
      adapter.next_record_id != 1u) return 8;
  if (bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK) return 9;
  return 0;
}
