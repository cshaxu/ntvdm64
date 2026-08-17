#ifndef OPENNT_INT06_PROVIDER_V1_H
#define OPENNT_INT06_PROVIDER_V1_H

#include <stdint.h>
#include <string.h>

#define OPENNT_INT06_PROVIDER_V1_MAGIC 0x4f493036u
#define OPENNT_INT06_PROVIDER_V1_VERSION 1u
#define OPENNT_INT06_BDA_INTERRUPT_CAUSE 0x046bu

enum opennt_int06_provider_v1_operation_kind {
  OPENNT_INT06_PROVIDER_V1_OPERATION_NONE = 0u,
  OPENNT_INT06_PROVIDER_V1_OPERATION_RAM_READ8 = 1u,
  OPENNT_INT06_PROVIDER_V1_OPERATION_RAM_WRITE8 = 2u,
  OPENNT_INT06_PROVIDER_V1_OPERATION_PORT_READ8 = 3u,
  OPENNT_INT06_PROVIDER_V1_OPERATION_PORT_WRITE8 = 4u,
  OPENNT_INT06_PROVIDER_V1_OPERATION_COMPLETE = 5u
};

enum opennt_int06_provider_v1_status {
  OPENNT_INT06_PROVIDER_V1_STATUS_PENDING = 0u,
  OPENNT_INT06_PROVIDER_V1_STATUS_COMPLETE = 1u,
  OPENNT_INT06_PROVIDER_V1_STATUS_INVALID = 2u,
  OPENNT_INT06_PROVIDER_V1_STATUS_FAILED = 3u
};

struct opennt_int06_provider_v1_operation {
  uint32_t kind;
  uint32_t reserved0;
  uint64_t address;
  uint8_t value;
  uint8_t reserved1[7];
};

struct opennt_int06_provider_v1_state {
  uint32_t magic, abi_version, struct_bytes, status;
  uint32_t phase, reserved0;
  uint64_t stack_physical;
  uint16_t saved_ip, saved_cs;
  uint8_t master_isr, master_imr, slave_isr, slave_imr;
};

#ifdef __cplusplus
extern "C" {
#endif

void opennt_int06_provider_v1_initialize(struct opennt_int06_provider_v1_state *state);
int opennt_int06_provider_v1_begin(struct opennt_int06_provider_v1_state *state,
  uint16_t ss, uint32_t esp);
int opennt_int06_provider_v1_next(const struct opennt_int06_provider_v1_state *state,
  struct opennt_int06_provider_v1_operation *operation);
int opennt_int06_provider_v1_accept(struct opennt_int06_provider_v1_state *state,
  const struct opennt_int06_provider_v1_operation *operation, uint8_t read_value);
int opennt_int06_provider_v1_complete(const struct opennt_int06_provider_v1_state *state);

#ifdef __cplusplus
}
#endif
#endif
