#include "bochs.h"
#include "adapter-softpc/finite_run.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"
#include "adapter-softpc/instruction_history.h"

#ifndef RUNTIME_ENABLE_MACHINE_INSTRUCTION_HISTORY
#define RUNTIME_ENABLE_MACHINE_INSTRUCTION_HISTORY 0
#endif

#include <string.h>

extern "C" int runtime_machine_generic_ud_bridge(
  const struct runtime_generic_ud_event *event,
  struct runtime_generic_ud_outcome *outcome)
{
  if (!event || !outcome || event->vector != 6u) return 0;
  outcome->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
  outcome->disposition = RUNTIME_GENERIC_UD_STOP;
  return 1;
}

int main(void)
{
  static const Bit8u code[] = { 0x90u, 0x90u, 0x0fu, 0x0bu };
  runtime_finite_run_request request;
  runtime_instruction_history_record record;
  runtime_finite_run_status status;
  uint32_t count;

  if (!runtime_machine_instruction_history_configure(0u) ||
      runtime_machine_instruction_history_count() != 0u) return 1;
  memset(&request, 0, sizeof(request));
  request.request_version = RUNTIME_FINITE_RUN_REQUEST_VERSION;
  memcpy(request.entry_bytes, code, sizeof(code));
  request.entry_byte_count = sizeof(code);
  request.entry_physical_address = 0xe000u;
  request.entry_cs = 0x0e00u;
  request.instruction_tick_budget = 64u;
  request.ips = 1u;
  if (!runtime_machine_instruction_history_configure(
      RUNTIME_INSTRUCTION_HISTORY_CAPACITY_MAX)) return 2;
  status = runtime_run_finite_bare_bytes(&request);
  count = runtime_machine_instruction_history_count();
#if RUNTIME_ENABLE_MACHINE_INSTRUCTION_HISTORY
  if (status != RUNTIME_FINITE_RUN_COMPLETED_UD_STOP || count < 3u ||
      !runtime_machine_instruction_history_get(count - 1u, &record) ||
      record.cs != 0x0e00u || record.rip != 2u) return 3;
#else
  if (status != RUNTIME_FINITE_RUN_COMPLETED_UD_STOP || count != 0u) return 4;
#endif
  return runtime_machine_instruction_history_configure(0u) ? 0 : 5;
}
