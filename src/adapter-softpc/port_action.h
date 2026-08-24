#ifndef RUNTIME_PORT_ACTION_V1_H
#define RUNTIME_PORT_ACTION_V1_H
#include <stdint.h>
#include <string.h>
#define RUNTIME_PORT_ACTION_V1_MAGIC 0x42585041u
#define RUNTIME_PORT_ACTION_V1_VERSION 1u
enum runtime_port_action_v1_kind { RUNTIME_PORT_ACTION_V1_READ8 = 1u, RUNTIME_PORT_ACTION_V1_WRITE8 = 2u };
struct runtime_port_action_v1 { uint32_t magic, abi_version, struct_bytes, kind; uint16_t port; uint8_t value, reserved0; };
#ifdef __cplusplus
extern "C" {
#endif
void runtime_port_action_v1_initialize(struct runtime_port_action_v1 *action);
int runtime_port_action_v1_valid(const struct runtime_port_action_v1 *action);
int runtime_mantle_execute_port_action_v1(struct runtime_port_action_v1 *action);
void runtime_port_action_v1_set_lifecycle_active(uint32_t active);
#ifdef __cplusplus
}
#endif
#endif