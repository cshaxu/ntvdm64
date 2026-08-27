#include "bochs.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"
#include "adapter-softpc/machine_stage.h"
#include "adapter-softpc/mechanical_action.h"
#include "support/dpmi_xmem_record_adapter.h"

#include <string.h>

/* This source-derived record has no production consumer.  Keep its historic
 * allocation/lifetime exercise test-local rather than leave a DPMI-named
 * service record in the selector-blind adapter library. */
#include "support/dpmi_xmem_record_adapter.c"

extern "C" int runtime_machine_generic_ud_bridge(
  const struct runtime_generic_ud_event *event,
  struct runtime_generic_ud_outcome *outcome)
{
  (void)event; (void)outcome; return 0;
}

static int begin_stage(void)
{
  static const Bit8u halt[] = { 0xf4u };
  struct runtime_machine_stage_request request;
  runtime_machine_stage_request_clear(&request);
  request.guest_memory_bytes = UINT64_C(0x200000);
  request.reserved_memory_base = UINT64_C(0x100000);
  request.reserved_memory_bytes = UINT64_C(0x100000);
  runtime_mechanical_action_clear(&request.startup_action);
  request.startup_action.action_id = 1u;
  request.startup_action.kind = RUNTIME_MECHANICAL_ACTION_WRITE;
  request.startup_action.range_count = 1u;
  request.startup_action.payload_bytes = sizeof(halt);
  request.startup_action.ranges[0].physical_address = 0x700u;
  request.startup_action.ranges[0].byte_count = sizeof(halt);
  memcpy(request.startup_action.payload, halt, sizeof(halt));
  request.preserved_state_address = 0x600u;
  request.preserved_state_bytes = 1u;
  return runtime_machine_stage_request_valid(&request) &&
    runtime_machine_stage_begin(&request) == RUNTIME_MACHINE_STAGE_OK;
}

int main(void)
{
  struct runtime_dpmi_xmem_record_adapter adapter;
  uint32_t first_address, first_id, second_address, second_id;
  uint32_t replacement_address;
  Bit8u written[] = { 0x13u, 0x57u, 0x9bu, 0xdfu };
  Bit8u observed[sizeof(written)] = { 0 };
  runtime_dpmi_xmem_record_adapter_clear(&adapter);
  if (!runtime_dpmi_xmem_record_adapter_valid(&adapter)) return 1;
  if (runtime_dpmi_xmem_record_adapter_allocate(&adapter, 0x10u, 0x10000u,
      &first_address, &first_id) != RUNTIME_DPMI_XMEM_RECORD_ADAPTER_MACHINE_FAILURE)
    return 2;
  if (!begin_stage()) return 3;
  if (runtime_dpmi_xmem_record_adapter_allocate(&adapter, 0x10u, 0x10000u,
      &first_address, &first_id) != RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK ||
      first_address != 0x100000u || first_id == 0u) return 4;
  if (!runtime_machine_checked_ram_write(first_address, written,
      sizeof(written))) return 5;
  if (runtime_dpmi_xmem_record_adapter_reallocate(&adapter, first_id, 0x20000u,
      &replacement_address) != RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK ||
      replacement_address != 0x110000u || adapter.records[0].record_id != first_id ||
      !runtime_machine_checked_ram_read(replacement_address, observed,
        sizeof(observed)) || memcmp(written, observed, sizeof(written)) != 0) return 6;
  if (runtime_dpmi_xmem_record_adapter_reallocate(&adapter, first_id, 0x1f000u,
      &replacement_address) != RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK ||
      replacement_address != 0x110000u) return 7;
  if (runtime_dpmi_xmem_record_adapter_allocate(&adapter, 0x20u, 0x10000u,
      &second_address, &second_id) != RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK ||
      second_address != 0x100000u || second_id == first_id) return 8;
  if (runtime_dpmi_xmem_record_adapter_release(&adapter, first_id + 100u) !=
      RUNTIME_DPMI_XMEM_RECORD_ADAPTER_REJECTED_ID || adapter.record_count != 2u) return 9;
  if (runtime_dpmi_xmem_record_adapter_release_owner(&adapter, 0x10u) !=
      RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK || adapter.record_count != 1u) return 10;
  if (runtime_dpmi_xmem_record_adapter_reset(&adapter) !=
      RUNTIME_DPMI_XMEM_RECORD_ADAPTER_OK || adapter.record_count != 0u ||
      adapter.next_record_id != 1u) return 11;
  if (runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK) return 12;
  return 0;
}
