#include "bochs.h"
#include "adapter-mvdm-host-out/softpc/cpu_state_abi.h"
#include "adapter-mvdm-host-out/softpc/port_action.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"
#include "app/bop/route.h"
#include "adapter-bochs/minimal_machine.h"

static void initialize_event(struct runtime_generic_ud_event *event,
    uint8_t selector)
{
  memset(event, 0, sizeof(*event));
  event->magic = RUNTIME_GENERIC_UD_EVENT_MAGIC;
  event->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
  event->struct_bytes = sizeof(*event);
  event->vector = 6u;
  event->execution_mode = RUNTIME_CPU_EXECUTION_REAL;
  event->fault_rip = 0x100u;
  event->window_bytes = 3u;
  event->window[0] = 0xc4u;
  event->window[1] = 0xc4u;
  event->window[2] = selector;
}

int main(void)
{
    if (!runtime_bop_ingress_bind(runtime_opennt_bop_route_dispatch, 0)) return 90;
  adapter_bochs_minimal_machine_c machine;
  struct runtime_generic_ud_event event;
  struct runtime_generic_ud_outcome outcome;
  Bit8u frame[4] = {0x34u, 0x12u, 0x70u, 0u};
  Bit8u value = 0u;

  if (machine.initialize(0x200000u, 0x200000u) != BX_MACHINE_MINIMAL_MACHINE_OK)
    return 1;
  runtime_port_action_set_lifecycle_active(1u);
  if (!bx_mem.copy_to_ordinary_ram(0xb79u, sizeof(frame), frame)) return 2;

  initialize_event(&event, 0x06u);
  event.ss = 0x70u;
  event.esp = 0x479u;
  memset(&outcome, 0, sizeof(outcome));
  if (!runtime_machine_generic_ud_bridge(&event, &outcome) ||
      outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
      outcome.resume_rip != 0x103u) return 3;
  if (!bx_mem.copy_from_ordinary_ram(0xb79u, 1u, &value) || value != 0x35u)
    return 4;
  if (!bx_mem.copy_from_ordinary_ram(0x46bu, 1u, &value) || value != 0xffu)
    return 5;

  initialize_event(&event, 0x02u);
  memset(&outcome, 0, sizeof(outcome));
  if (!runtime_machine_generic_ud_bridge(&event, &outcome) ||
      outcome.disposition != RUNTIME_GENERIC_UD_RESUME ||
      outcome.resume_rip != 0x103u) return 6;
  if (!bx_mem.copy_from_ordinary_ram(0x46bu, 1u, &value) || value != 0xffu)
    return 7;

  /* The source operation is accepted but checked preflight failure is a
   * controlled machine stop, never a fall-through into CPU exception code. */
  initialize_event(&event, 0x06u);
  event.ss = 0xffffu;
  event.esp = 0x200000u;
  memset(&outcome, 0, sizeof(outcome));
  if (!runtime_machine_generic_ud_bridge(&event, &outcome) ||
      outcome.disposition != RUNTIME_GENERIC_UD_STOP) return 8;

  initialize_event(&event, 0x07u);
  memset(&outcome, 0, sizeof(outcome));
  if (runtime_machine_generic_ud_bridge(&event, &outcome)) return 9;
  runtime_port_action_set_lifecycle_active(0u);
  return machine.cleanup() == BX_MACHINE_MINIMAL_MACHINE_OK ? 0 : 10;
}
