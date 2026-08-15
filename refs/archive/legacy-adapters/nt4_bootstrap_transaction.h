#ifndef NT4_BOOTSTRAP_TRANSACTION_H
#define NT4_BOOTSTRAP_TRANSACTION_H

#include "nt4_ntdos_load_plan.h"
#include "nt4_ntio_bootstrap_plan.h"

#include <stddef.h>
#include <stdint.h>

typedef enum nt4_bootstrap_transaction_result {
    NT4_BOOTSTRAP_TRANSACTION_OK = 0,
    NT4_BOOTSTRAP_TRANSACTION_INVALID_ARGUMENT,
    NT4_BOOTSTRAP_TRANSACTION_RAM_RANGE_REJECTED,
    NT4_BOOTSTRAP_TRANSACTION_OVERLAP_REJECTED
} nt4_bootstrap_transaction_result;

typedef struct nt4_bootstrap_preload_view {
    uint32_t physical;
    const uint8_t *bytes;
    size_t byte_count;
} nt4_bootstrap_preload_view;

typedef struct nt4_bootstrap_transaction {
    nt4_bootstrap_preload_view preloads[2];
    size_t preload_count;
    uint16_t entry_cs;
    uint16_t entry_ip;
} nt4_bootstrap_transaction;

/* Trace-only synthetic-buffer evidence. It must not model the runtime boot:
 * original demLoadDos owns NTDOS loading after NTIO entry. Both input plans
 * must outlive the transaction. On failure, out_transaction remains unchanged. */
nt4_bootstrap_transaction_result nt4_bootstrap_transaction_build(
    const nt4_ntio_bootstrap_plan *ntio_plan,
    const nt4_ntdos_load_plan *ntdos_plan,
    size_t ordinary_ram_byte_count,
    nt4_bootstrap_transaction *out_transaction);

#endif
