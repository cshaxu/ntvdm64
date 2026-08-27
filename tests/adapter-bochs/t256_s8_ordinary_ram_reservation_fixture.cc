#include "bochs.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"
#include "adapter-mvdm-host-out/softpc/machine_stage.h"
#include "adapter-mvdm-host-out/softpc/ordinary_ram_reservation.h"

#include <string.h>

extern "C" int runtime_machine_generic_ud_bridge(
  const struct runtime_generic_ud_event *event,
  struct runtime_generic_ud_outcome *outcome)
{
  (void) event;
  (void) outcome;
  return 0;
}

static int begin_stage(uint64_t base, uint64_t bytes)
{
  static const Bit8u halt[] = { 0xf4u };
  struct runtime_machine_stage_request request;
  runtime_machine_stage_request_clear(&request);
  request.guest_memory_bytes = UINT64_C(0x200000);
  request.reserved_memory_base = base;
  request.reserved_memory_bytes = bytes;
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

static int allocate(uint32_t bytes, uint32_t alignment,
  struct runtime_ordinary_ram_reservation *action)
{
  runtime_ordinary_ram_reservation_clear(action);
  action->kind = RUNTIME_ORDINARY_RAM_RESERVATION_ALLOCATE;
  action->byte_count = bytes;
  action->alignment_bytes = alignment;
  return runtime_machine_execute_ordinary_ram_reservation(action) ==
    RUNTIME_ORDINARY_RAM_RESERVATION_OK;
}

int main(void)
{
  struct runtime_machine_stage_request malformed;
  struct runtime_ordinary_ram_reservation first, second, release, reused;
  Bit8u value = 0x5au, observed = 0u;

  runtime_ordinary_ram_reservation_clear(&first);
  first.kind = RUNTIME_ORDINARY_RAM_RESERVATION_ALLOCATE;
  first.byte_count = 0x1000u;
  first.alignment_bytes = 0x1000u;
  if (runtime_machine_execute_ordinary_ram_reservation(&first) !=
      RUNTIME_ORDINARY_RAM_RESERVATION_REJECTED_LIFECYCLE) return 1;

  runtime_machine_stage_request_clear(&malformed);
  malformed.guest_memory_bytes = UINT64_C(0x200000);
  malformed.reserved_memory_base = UINT64_C(0x101000);
  malformed.reserved_memory_bytes = UINT64_C(0x10000);
  if (runtime_machine_stage_request_valid(&malformed)) return 2;

  if (!begin_stage(0u, 0u)) return 3;
  if (allocate(0x1000u, 0x1000u, &first) ||
      first.status != RUNTIME_ORDINARY_RAM_RESERVATION_REJECTED_CONFIGURATION) return 4;
  if (runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK) return 5;

  if (!begin_stage(UINT64_C(0x100000), UINT64_C(0x100000))) return 6;
  if (!allocate(0x10000u, 0x10000u, &first) || first.address != 0x100000u ||
      first.opaque_id == 0u) return 7;
  if (!allocate(0x10000u, 0x10000u, &second) || second.address != 0x110000u ||
      second.opaque_id == first.opaque_id) return 8;
  if (!runtime_machine_checked_ram_write(first.address, &value, sizeof(value)) ||
      !runtime_machine_checked_ram_read(first.address, &observed, sizeof(observed)) ||
      observed != value) return 9;
  runtime_ordinary_ram_reservation_clear(&release);
  release.kind = RUNTIME_ORDINARY_RAM_RESERVATION_RELEASE;
  release.opaque_id = first.opaque_id;
  if (runtime_machine_execute_ordinary_ram_reservation(&release) !=
      RUNTIME_ORDINARY_RAM_RESERVATION_OK) return 10;
  if (!allocate(0x10000u, 0x10000u, &reused) || reused.address != 0x100000u) return 11;
  if (runtime_machine_stage_reset() != RUNTIME_MACHINE_STAGE_OK) return 12;
  runtime_ordinary_ram_reservation_clear(&reused);
  reused.kind = RUNTIME_ORDINARY_RAM_RESERVATION_ALLOCATE;
  reused.byte_count = 0x1000u;
  reused.alignment_bytes = 0x1000u;
  if (runtime_machine_execute_ordinary_ram_reservation(&reused) !=
      RUNTIME_ORDINARY_RAM_RESERVATION_REJECTED_LIFECYCLE) return 13;
  return 0;
}
