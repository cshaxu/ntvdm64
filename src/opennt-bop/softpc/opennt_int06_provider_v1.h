#ifndef INT06_PROVIDER_V1_H
#define INT06_PROVIDER_V1_H

#include <stdint.h>
#include <string.h>

#define INT06_PROVIDER_V1_MAGIC 0x4f493036u
#define INT06_PROVIDER_V1_VERSION 1u
#define INT06_BDA_INTERRUPT_CAUSE 0x046bu

enum int06_provider_v1_operation_kind {
  INT06_PROVIDER_V1_OPERATION_NONE = 0u,
  INT06_PROVIDER_V1_OPERATION_RAM_READ8 = 1u,
  INT06_PROVIDER_V1_OPERATION_RAM_WRITE8 = 2u,
  INT06_PROVIDER_V1_OPERATION_PORT_READ8 = 3u,
  INT06_PROVIDER_V1_OPERATION_PORT_WRITE8 = 4u,
  INT06_PROVIDER_V1_OPERATION_COMPLETE = 5u
};

enum int06_provider_v1_status {
  INT06_PROVIDER_V1_STATUS_PENDING = 0u,
  INT06_PROVIDER_V1_STATUS_COMPLETE = 1u,
  INT06_PROVIDER_V1_STATUS_INVALID = 2u,
  INT06_PROVIDER_V1_STATUS_FAILED = 3u
};

struct int06_provider_v1_operation {
  uint32_t kind;
  uint32_t reserved0;
  uint64_t address;
  uint8_t value;
  uint8_t reserved1[7];
};

struct int06_provider_v1_state {
  uint32_t magic, abi_version, struct_bytes, status;
  uint32_t phase, reserved0;
  uint64_t stack_physical;
  uint16_t saved_ip, saved_cs;
  uint8_t master_isr, master_imr, slave_isr, slave_imr;
};

#ifdef __cplusplus
extern "C" {
#endif

void int06_provider_v1_initialize(struct int06_provider_v1_state *state);
int int06_provider_v1_begin(struct int06_provider_v1_state *state,
  uint16_t ss, uint32_t esp);
/* Begins the original unexpected_int common suffix directly.  This is used
 * for the direct 02h entry; it deliberately does not read or modify a saved frame. */
int int06_provider_v1_begin_unexpected(
  struct int06_provider_v1_state *state);
int int06_provider_v1_next(const struct int06_provider_v1_state *state,
  struct int06_provider_v1_operation *operation);
int int06_provider_v1_accept(struct int06_provider_v1_state *state,
  const struct int06_provider_v1_operation *operation, uint8_t read_value);
int int06_provider_v1_complete(const struct int06_provider_v1_state *state);

#ifdef __cplusplus
}
#endif
#endif
