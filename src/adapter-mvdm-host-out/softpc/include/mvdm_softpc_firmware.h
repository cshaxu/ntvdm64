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

/* Same-shaped media-location binding for the original NTIO image loader.
 * It returns a caller-owned path only when the selected DOS image exists. */
int mvdm_softpc_dos_find_file(const char *name, char *path_out,
    uint32_t path_out_bytes);

/* Selected-product disposition for the historical first-session compatibility
 * probe.  The original AddSystemFiles() body creates C:\\MSDOS.SYS and
 * C:\\IO.SYS only to satisfy a legacy installer.  The current product keeps
 * ordinary admitted host-drive access, but never creates these fake boot
 * markers on the real host volume. */
void mvdm_softpc_prepare_system_file_compatibility(void);

#ifdef __cplusplus
}
#endif

#endif
