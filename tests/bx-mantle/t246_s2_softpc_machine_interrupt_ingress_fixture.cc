#include "bochs.h"
#include "adapter-softpc/bx_ntvdm_cpu_state_abi.h"
#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"
#include "opennt-bop/ingress/opennt_bop_route.h"
#include "bx-mantle/minimal_machine.h"

static void initialize_event(struct bx_ntvdm_generic_ud_event_v1 *event,
    uint8_t selector)
{
  memset(event, 0, sizeof(*event));
  event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
  event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
  event->struct_bytes = sizeof(*event);
  event->vector = 6u;
  event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
  event->fault_rip = 0x100u;
  event->window_bytes = 3u;
  event->window[0] = 0xc4u;
  event->window[1] = 0xc4u;
  event->window[2] = selector;
}

int main(void)
{
    if (!bx_ntvdm_bop_ingress_v1_bind(bx_ntvdm_opennt_bop_route_dispatch_v1, 0)) return 90;
  bx_mantle_minimal_machine_c machine;
  struct bx_ntvdm_generic_ud_event_v1 event;
  struct bx_ntvdm_generic_ud_outcome_v1 outcome;
  Bit8u frame[4] = {0x34u, 0x12u, 0x70u, 0u};
  Bit8u value = 0u;

  if (machine.initialize(0x200000u, 0x200000u) != BX_MANTLE_MINIMAL_MACHINE_OK)
    return 1;
  if (!bx_mem.copy_to_ordinary_ram(0xb79u, sizeof(frame), frame)) return 2;

  initialize_event(&event, 0x06u);
  event.ss = 0x70u;
  event.esp = 0x479u;
  memset(&outcome, 0, sizeof(outcome));
  if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
      outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
      outcome.resume_rip != 0x103u) return 3;
  if (!bx_mem.copy_from_ordinary_ram(0xb79u, 1u, &value) || value != 0x35u)
    return 4;
  if (!bx_mem.copy_from_ordinary_ram(0x46bu, 1u, &value) || value != 0xffu)
    return 5;

  initialize_event(&event, 0x02u);
  memset(&outcome, 0, sizeof(outcome));
  if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
      outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
      outcome.resume_rip != 0x103u) return 6;
  if (!bx_mem.copy_from_ordinary_ram(0x46bu, 1u, &value) || value != 0xffu)
    return 7;

  /* The source operation is accepted but checked preflight failure is a
   * controlled machine stop, never a fall-through into CPU exception code. */
  initialize_event(&event, 0x06u);
  event.ss = 0xffffu;
  event.esp = 0x200000u;
  memset(&outcome, 0, sizeof(outcome));
  if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
      outcome.disposition != BX_NTVDM_GENERIC_UD_STOP) return 8;

  initialize_event(&event, 0x07u);
  memset(&outcome, 0, sizeof(outcome));
  if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 9;
  return machine.cleanup() == BX_MANTLE_MINIMAL_MACHINE_OK ? 0 : 10;
}