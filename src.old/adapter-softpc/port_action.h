#ifndef RUNTIME_PORT_ACTION_H
#define RUNTIME_PORT_ACTION_H
#include <stdint.h>
#include <string.h>
#define RUNTIME_PORT_ACTION_MAGIC 0x42585041u
#define RUNTIME_PORT_ACTION_VERSION 1u
enum runtime_port_action_kind { RUNTIME_PORT_ACTION_READ8 = 1u, RUNTIME_PORT_ACTION_WRITE8 = 2u };
struct runtime_port_action { uint32_t magic, abi_version, struct_bytes, kind; uint16_t port; uint8_t value, reserved0; };
#ifdef __cplusplus
extern "C" {
#endif
void runtime_port_action_initialize(struct runtime_port_action *action);
int runtime_port_action_valid(const struct runtime_port_action *action);
int runtime_machine_execute_port_action(struct runtime_port_action *action);
void runtime_port_action_set_lifecycle_active(uint32_t active);
#ifdef __cplusplus
}
#endif
#endif