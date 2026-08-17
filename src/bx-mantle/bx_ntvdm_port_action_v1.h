#ifndef BX_NTVDM_PORT_ACTION_V1_H
#define BX_NTVDM_PORT_ACTION_V1_H
#include <stdint.h>
#include <string.h>
#define BX_NTVDM_PORT_ACTION_V1_MAGIC 0x42585041u
#define BX_NTVDM_PORT_ACTION_V1_VERSION 1u
enum bx_ntvdm_port_action_v1_kind { BX_NTVDM_PORT_ACTION_V1_READ8 = 1u, BX_NTVDM_PORT_ACTION_V1_WRITE8 = 2u };
struct bx_ntvdm_port_action_v1 { uint32_t magic, abi_version, struct_bytes, kind; uint16_t port; uint8_t value, reserved0; };
#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_port_action_v1_initialize(struct bx_ntvdm_port_action_v1 *action);
int bx_ntvdm_port_action_v1_valid(const struct bx_ntvdm_port_action_v1 *action);
int bx_ntvdm_mantle_execute_port_action_v1(struct bx_ntvdm_port_action_v1 *action);
void bx_ntvdm_port_action_v1_set_lifecycle_active(uint32_t active);
#ifdef __cplusplus
}
#endif
#endif