#include "bochs.h"
#include "iodev/iodev.h"
#include "bx_ntvdm_port_action_v1.h"
static uint32_t bx_ntvdm_port_action_v1_lifecycle_active;
extern "C" void bx_ntvdm_port_action_v1_initialize(struct bx_ntvdm_port_action_v1 *action)
{ if (action == 0) return; memset(action, 0, sizeof(*action)); action->magic = BX_NTVDM_PORT_ACTION_V1_MAGIC; action->abi_version = BX_NTVDM_PORT_ACTION_V1_VERSION; action->struct_bytes = sizeof(*action); }
extern "C" int bx_ntvdm_port_action_v1_valid(const struct bx_ntvdm_port_action_v1 *action)
{ return action != 0 && action->magic == BX_NTVDM_PORT_ACTION_V1_MAGIC && action->abi_version == BX_NTVDM_PORT_ACTION_V1_VERSION && action->struct_bytes == sizeof(*action) && (action->kind == BX_NTVDM_PORT_ACTION_V1_READ8 || action->kind == BX_NTVDM_PORT_ACTION_V1_WRITE8) && action->reserved0 == 0u; }
extern "C" void bx_ntvdm_port_action_v1_set_lifecycle_active(uint32_t active)
{ bx_ntvdm_port_action_v1_lifecycle_active = active == 1u ? 1u : 0u; }
static int bx_ntvdm_port_action_v1_registered(const struct bx_ntvdm_port_action_v1 *action)
{
  return bx_devices.native_port_is_registered(action->port,
    action->kind == BX_NTVDM_PORT_ACTION_V1_WRITE8);
}
extern "C" int bx_ntvdm_mantle_execute_port_action_v1(struct bx_ntvdm_port_action_v1 *action)
{ if (!bx_ntvdm_port_action_v1_lifecycle_active || !bx_ntvdm_port_action_v1_valid(action) || !bx_ntvdm_port_action_v1_registered(action)) return 0; if (action->kind == BX_NTVDM_PORT_ACTION_V1_READ8) action->value = (uint8_t)bx_devices.inp(action->port, 1u); else bx_devices.outp(action->port, action->value, 1u); return 1; }
