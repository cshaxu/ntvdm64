#ifndef RUNTIME_BOP_FRAME_TRANSACTION_H
#define RUNTIME_BOP_FRAME_TRANSACTION_H

/*
 * Selector-blind copied-frame transaction.
 *
 * This is an adapter-owned mechanical seam, not an OpenNT provider API.  It
 * copies one machine exception event into the historical CCPU-shaped records,
 * supplies only checked ordinary-RAM transport, and converts the typed result
 * back to the machine outcome.  It never decodes an instruction window or
 * retains provider/session data.
 */

#include "generic_ud_bridge.h"
#include "adapter-softpc/cpu_result.h"
#include "adapter-softpc/exception_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RUNTIME_BOP_FRAME_TRANSACTION_MAGIC 0x42584654u
#define RUNTIME_BOP_FRAME_TRANSACTION_VERSION 1u

typedef struct runtime_bop_frame_transaction {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t reserved0;
    runtime_exception_event boundary;
    runtime_cpu_state cpu;
    runtime_cpu_result result;
} runtime_bop_frame_transaction;

int runtime_bop_frame_transaction_begin(
    const struct runtime_generic_ud_event *event,
    runtime_bop_frame_transaction *transaction);
int runtime_bop_frame_transaction_valid(
    const runtime_bop_frame_transaction *transaction);

/* The context is the borrowed transaction passed to a source-shaped provider.
 * Both operations use the active machine's checked ordinary-RAM boundary. */
int runtime_bop_frame_transaction_guest_read(void *context,
    uint32_t address, uint8_t *bytes, uint32_t byte_count);
int runtime_bop_frame_transaction_guest_write(void *context,
    uint32_t address, const uint8_t *bytes, uint32_t byte_count);

int runtime_bop_frame_transaction_complete(
    const runtime_bop_frame_transaction *transaction,
    struct runtime_generic_ud_outcome *outcome);

#ifdef __cplusplus
}
#endif

#endif
