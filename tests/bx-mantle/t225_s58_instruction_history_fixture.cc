#include "bochs.h"
#include "adapter-softpc/bx_ntvdm_finite_run.h"
#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"
#include "adapter-softpc/bx_ntvdm_instruction_history.h"

#ifndef BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
#define BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY 0
#endif

#include <string.h>

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
  if (!event || !outcome || event->vector != 6u) return 0;
  outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
  outcome->disposition = BX_NTVDM_GENERIC_UD_STOP;
  return 1;
}

int main(void)
{
  static const Bit8u code[] = { 0x90u, 0x90u, 0x0fu, 0x0bu };
  bx_ntvdm_finite_run_request request;
  bx_ntvdm_instruction_history_record_v1 record;
  bx_ntvdm_finite_run_status status;
  uint32_t count;

  if (!bx_ntvdm_mantle_instruction_history_v1_configure(0u) ||
      bx_ntvdm_mantle_instruction_history_v1_count() != 0u) return 1;
  memset(&request, 0, sizeof(request));
  request.request_version = BX_NTVDM_FINITE_RUN_REQUEST_VERSION;
  memcpy(request.entry_bytes, code, sizeof(code));
  request.entry_byte_count = sizeof(code);
  request.entry_physical_address = 0xe000u;
  request.entry_cs = 0x0e00u;
  request.instruction_tick_budget = 64u;
  request.ips = 1u;
  request.stop_on_ud_fixture = 1u;
  if (!bx_ntvdm_mantle_instruction_history_v1_configure(
      BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX)) return 2;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  count = bx_ntvdm_mantle_instruction_history_v1_count();
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP || count < 3u ||
      !bx_ntvdm_mantle_instruction_history_v1_get(count - 1u, &record) ||
      record.cs != 0x0e00u || record.rip != 2u) return 3;
#else
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP || count != 0u) return 4;
#endif
  return bx_ntvdm_mantle_instruction_history_v1_configure(0u) ? 0 : 5;
}
