#ifndef MVDM_SOFTPC_FIRMWARE_H
#define MVDM_SOFTPC_FIRMWARE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Same-shaped host resource lookup binding used by the original
 * host_find_file() caller.  It owns no firmware bytes and returns only a
 * caller-provided, synchronous path buffer. */
int mvdm_softpc_firmware_find_file(const char *name, char *path_out,
    uint32_t path_out_bytes);

#ifdef __cplusplus
}
#endif

#endif
