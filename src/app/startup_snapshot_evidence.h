#ifndef RUNTIME_STARTUP_SNAPSHOT_EVIDENCE_H
#define RUNTIME_STARTUP_SNAPSHOT_EVIDENCE_H

#include <stdint.h>
#include <wchar.h>

#include "startup_snapshot_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Writes an adapter-owned completed snapshot record under the selected BYOB
 * root. The caller supplies only a validated leaf filename. */
int runtime_startup_snapshot_evidence_write(
    const wchar_t *byob_root, const wchar_t *file_name,
    const runtime_startup_snapshot_transaction *transaction,
    const uint8_t *output, uint64_t output_bytes, uint64_t digest);

#ifdef __cplusplus
}
#endif

#endif
