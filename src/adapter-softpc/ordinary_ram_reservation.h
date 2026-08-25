#ifndef RUNTIME_ORDINARY_RAM_RESERVATION_H
#define RUNTIME_ORDINARY_RAM_RESERVATION_H

#include <stdint.h>

#define RUNTIME_ORDINARY_RAM_RESERVATION_MAGIC UINT32_C(0x42584f52)
#define RUNTIME_ORDINARY_RAM_RESERVATION_VERSION UINT32_C(1)
#define RUNTIME_ORDINARY_RAM_RESERVATION_MAX_RECORDS UINT32_C(32)
#define RUNTIME_ORDINARY_RAM_RESERVATION_MIN_ALIGNMENT UINT32_C(0x1000)
#define RUNTIME_ORDINARY_RAM_RESERVATION_MAX_ALIGNMENT UINT32_C(0x10000)

enum runtime_ordinary_ram_reservation_kind {
  RUNTIME_ORDINARY_RAM_RESERVATION_ALLOCATE = 1u,
  RUNTIME_ORDINARY_RAM_RESERVATION_RELEASE = 2u
};

enum runtime_ordinary_ram_reservation_status {
  RUNTIME_ORDINARY_RAM_RESERVATION_OK = 0u,
  RUNTIME_ORDINARY_RAM_RESERVATION_REJECTED_INPUT,
  RUNTIME_ORDINARY_RAM_RESERVATION_REJECTED_LIFECYCLE,
  RUNTIME_ORDINARY_RAM_RESERVATION_REJECTED_CONFIGURATION,
  RUNTIME_ORDINARY_RAM_RESERVATION_REJECTED_CAPACITY,
  RUNTIME_ORDINARY_RAM_RESERVATION_REJECTED_ID,
  RUNTIME_ORDINARY_RAM_RESERVATION_REJECTED_MEMORY
};

/* Fixed copied allocation/release record.  `opaque_id` is deliberately not a
 * host pointer or an address-derived token. */
struct runtime_ordinary_ram_reservation {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t kind;
  uint32_t byte_count;
  uint32_t alignment_bytes;
  uint32_t opaque_id;
  uint32_t address;
  uint32_t status;
};

#ifdef __cplusplus
extern "C" {
#endif

void runtime_ordinary_ram_reservation_clear(
  struct runtime_ordinary_ram_reservation *action);
int runtime_ordinary_ram_reservation_valid(
  const struct runtime_ordinary_ram_reservation *action);
/* This configuration is accepted only before its minimal-machine lifecycle
 * becomes active. Both arguments zero retain the ordinary no-reservation
 * default. */
uint32_t runtime_ordinary_ram_reservation_configure(uint64_t base,
  uint64_t bytes);
void runtime_ordinary_ram_reservation_set_lifecycle_active(uint32_t active);
uint32_t runtime_machine_execute_ordinary_ram_reservation(
  struct runtime_ordinary_ram_reservation *action);

#ifdef __cplusplus
}
#endif

#endif
