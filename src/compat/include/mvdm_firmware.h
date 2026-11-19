#ifndef COMPAT_MVDM_FIRMWARE_H
#define COMPAT_MVDM_FIRMWARE_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Copy one selected immutable firmware image from the product executable into
 * an original caller-owned buffer.  It is deliberately not a file-search
 * fallback: only the three fixed firmware identities are accepted. */
long mvdm_softpc_firmware_read_embedded_rom(const char *name, void *bytes_out,
    uint32_t bytes_out_capacity);

#ifdef __cplusplus
}
#endif
#endif
