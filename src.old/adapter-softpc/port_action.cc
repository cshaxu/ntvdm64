#include "bochs.h"
#include "iodev/iodev.h"
#include "port_action.h"
static uint32_t runtime_port_action_lifecycle_active;
extern "C" void runtime_port_action_initialize(struct runtime_port_action *action)
{ if (action == 0) return; memset(action, 0, sizeof(*action)); action->magic = RUNTIME_PORT_ACTION_MAGIC; action->abi_version = RUNTIME_PORT_ACTION_VERSION; action->struct_bytes = sizeof(*action); }
extern "C" int runtime_port_action_valid(const struct runtime_port_action *action)
{ return action != 0 && action->magic == RUNTIME_PORT_ACTION_MAGIC && action->abi_version == RUNTIME_PORT_ACTION_VERSION && action->struct_bytes == sizeof(*action) && (action->kind == RUNTIME_PORT_ACTION_READ8 || action->kind == RUNTIME_PORT_ACTION_WRITE8) && action->reserved0 == 0u; }
extern "C" void runtime_port_action_set_lifecycle_active(uint32_t active)
{ runtime_port_action_lifecycle_active = active == 1u ? 1u : 0u; }
static int runtime_port_action_registered(const struct runtime_port_action *action)
{
  return bx_devices.native_port_is_registered(action->port,
    action->kind == RUNTIME_PORT_ACTION_WRITE8);
}
extern "C" int runtime_machine_execute_port_action(struct runtime_port_action *action)
{ if (!runtime_port_action_lifecycle_active || !runtime_port_action_valid(action) || !runtime_port_action_registered(action)) return 0; if (action->kind == RUNTIME_PORT_ACTION_READ8) action->value = (uint8_t)bx_devices.inp(action->port, 1u); else bx_devices.outp(action->port, action->value, 1u); return 1; }
