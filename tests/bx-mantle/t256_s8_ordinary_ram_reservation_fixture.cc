#include "bochs.h"
#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"
#include "adapter-softpc/bx_ntvdm_machine_stage_v1.h"
#include "adapter-softpc/bx_ntvdm_ordinary_ram_reservation_v1.h"

#include <string.h>

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
  (void) event;
  (void) outcome;
  return 0;
}

static int begin_stage(uint64_t base, uint64_t bytes)
{
  static const Bit8u halt[] = { 0xf4u };
  struct bx_ntvdm_machine_stage_v1_request request;
  bx_ntvdm_machine_stage_v1_request_clear(&request);
  request.guest_memory_bytes = UINT64_C(0x200000);
  request.reserved_memory_base = base;
  request.reserved_memory_bytes = bytes;
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

static int allocate(uint32_t bytes, uint32_t alignment,
  struct bx_ntvdm_ordinary_ram_reservation_v1 *action)
{
  bx_ntvdm_ordinary_ram_reservation_v1_clear(action);
  action->kind = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_ALLOCATE;
  action->byte_count = bytes;
  action->alignment_bytes = alignment;
  return bx_ntvdm_mantle_execute_ordinary_ram_reservation_v1(action) ==
    BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_OK;
}

int main(void)
{
  struct bx_ntvdm_machine_stage_v1_request malformed;
  struct bx_ntvdm_ordinary_ram_reservation_v1 first, second, release, reused;
  Bit8u value = 0x5au, observed = 0u;

  bx_ntvdm_ordinary_ram_reservation_v1_clear(&first);
  first.kind = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_ALLOCATE;
  first.byte_count = 0x1000u;
  first.alignment_bytes = 0x1000u;
  if (bx_ntvdm_mantle_execute_ordinary_ram_reservation_v1(&first) !=
      BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_REJECTED_LIFECYCLE) return 1;

  bx_ntvdm_machine_stage_v1_request_clear(&malformed);
  malformed.guest_memory_bytes = UINT64_C(0x200000);
  malformed.reserved_memory_base = UINT64_C(0x101000);
  malformed.reserved_memory_bytes = UINT64_C(0x10000);
  if (bx_ntvdm_machine_stage_v1_request_valid(&malformed)) return 2;

  if (!begin_stage(0u, 0u)) return 3;
  if (allocate(0x1000u, 0x1000u, &first) ||
      first.status != BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_REJECTED_CONFIGURATION) return 4;
  if (bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK) return 5;

  if (!begin_stage(UINT64_C(0x100000), UINT64_C(0x100000))) return 6;
  if (!allocate(0x10000u, 0x10000u, &first) || first.address != 0x100000u ||
      first.opaque_id == 0u) return 7;
  if (!allocate(0x10000u, 0x10000u, &second) || second.address != 0x110000u ||
      second.opaque_id == first.opaque_id) return 8;
  if (!bx_ntvdm_mantle_checked_ram_write_v1(first.address, &value, sizeof(value)) ||
      !bx_ntvdm_mantle_checked_ram_read_v1(first.address, &observed, sizeof(observed)) ||
      observed != value) return 9;
  bx_ntvdm_ordinary_ram_reservation_v1_clear(&release);
  release.kind = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_RELEASE;
  release.opaque_id = first.opaque_id;
  if (bx_ntvdm_mantle_execute_ordinary_ram_reservation_v1(&release) !=
      BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_OK) return 10;
  if (!allocate(0x10000u, 0x10000u, &reused) || reused.address != 0x100000u) return 11;
  if (bx_ntvdm_machine_stage_v1_reset() != BX_NTVDM_MACHINE_STAGE_V1_OK) return 12;
  bx_ntvdm_ordinary_ram_reservation_v1_clear(&reused);
  reused.kind = BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_ALLOCATE;
  reused.byte_count = 0x1000u;
  reused.alignment_bytes = 0x1000u;
  if (bx_ntvdm_mantle_execute_ordinary_ram_reservation_v1(&reused) !=
      BX_NTVDM_ORDINARY_RAM_RESERVATION_V1_REJECTED_LIFECYCLE) return 13;
  return 0;
}
