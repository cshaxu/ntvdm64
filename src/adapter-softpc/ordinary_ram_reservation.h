#ifndef RUNTIME_ORDINARY_RAM_RESERVATION_V1_H
#define RUNTIME_ORDINARY_RAM_RESERVATION_V1_H

#include <stdint.h>

#define RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAGIC UINT32_C(0x42584f52)
#define RUNTIME_ORDINARY_RAM_RESERVATION_V1_VERSION UINT32_C(1)
#define RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAX_RECORDS UINT32_C(32)
#define RUNTIME_ORDINARY_RAM_RESERVATION_V1_MIN_ALIGNMENT UINT32_C(0x1000)
#define RUNTIME_ORDINARY_RAM_RESERVATION_V1_MAX_ALIGNMENT UINT32_C(0x10000)

enum runtime_ordinary_ram_reservation_v1_kind {
  RUNTIME_ORDINARY_RAM_RESERVATION_V1_ALLOCATE = 1u,
  RUNTIME_ORDINARY_RAM_RESERVATION_V1_RELEASE = 2u
};

enum runtime_ordinary_ram_reservation_v1_status {
  RUNTIME_ORDINARY_RAM_RESERVATION_V1_OK = 0u,
  RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_INPUT,
  RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_LIFECYCLE,
  RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_CONFIGURATION,
  RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_CAPACITY,
  RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_ID,
  RUNTIME_ORDINARY_RAM_RESERVATION_V1_REJECTED_MEMORY
};

/* Fixed copied allocation/release record.  `opaque_id` is deliberately not a
 * host pointer or an address-derived token. */
struct runtime_ordinary_ram_reservation_v1 {
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

void runtime_ordinary_ram_reservation_v1_clear(
  struct runtime_ordinary_ram_reservation_v1 *action);
int runtime_ordinary_ram_reservation_v1_valid(
  const struct runtime_ordinary_ram_reservation_v1 *action);
/* This configuration is accepted only before its minimal-machine lifecycle
 * becomes active. Both arguments zero retain the ordinary no-reservation
 * default. */
uint32_t runtime_ordinary_ram_reservation_v1_configure(uint64_t base,
  uint64_t bytes);
void runtime_ordinary_ram_reservation_v1_set_lifecycle_active(uint32_t active);
uint32_t runtime_mantle_execute_ordinary_ram_reservation_v1(
  struct runtime_ordinary_ram_reservation_v1 *action);

#ifdef __cplusplus
}
#endif

#endif
