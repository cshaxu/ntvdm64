#ifndef BX_NTVDM_STARTUP_SNAPSHOT_EVIDENCE_H
#define BX_NTVDM_STARTUP_SNAPSHOT_EVIDENCE_H

#include <stdint.h>
#include <wchar.h>

#include "startup_snapshot_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Writes an adapter-owned completed snapshot record under the selected BYOB
 * root. The caller supplies only a validated leaf filename. */
int bx_ntvdm_startup_snapshot_evidence_v1_write(
    const wchar_t *byob_root, const wchar_t *file_name,
    const bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    const uint8_t *output, uint64_t output_bytes, uint64_t digest);

#ifdef __cplusplus
}
#endif

#endif
