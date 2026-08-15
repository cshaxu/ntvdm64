#ifndef BX_NTVDM_READONLY_NAMESPACE_H
#define BX_NTVDM_READONLY_NAMESPACE_H

#include <stdint.h>

#include "byob_image.h"
#include "byob_profile.h"

#define BX_NTVDM_READONLY_NAMESPACE_MAX_FILES 5u
#define BX_NTVDM_READONLY_NAMESPACE_MAX_READ_BYTES 65536u

/* Adapter-local metadata only.  `bytes` is immutable image/profile storage;
 * this type never crosses the Bochs boundary or contains a host handle. */
typedef struct bx_ntvdm_readonly_namespace_file_v1 {
    const uint8_t *bytes;
    uint64_t byte_count;
    uint16_t dos_time, dos_date;
    wchar_t path[BYOB_PROFILE_GUEST_PATH_MAX_CHARS];
} bx_ntvdm_readonly_namespace_file_v1;

typedef struct bx_ntvdm_readonly_namespace_v1 {
    bx_ntvdm_readonly_namespace_file_v1 files[BX_NTVDM_READONLY_NAMESPACE_MAX_FILES];
    uint32_t file_count;
    uint32_t drive_index;
    uint32_t generation;
    uint32_t open;
    uint32_t open_file_index;
    uint64_t offset;
} bx_ntvdm_readonly_namespace_v1;

int bx_ntvdm_readonly_namespace_v1_initialize(
    bx_ntvdm_readonly_namespace_v1 *value, const byob_image *command,
    const byob_profile_selection *selection);
int bx_ntvdm_readonly_namespace_v1_append_target(
    bx_ntvdm_readonly_namespace_v1 *value, const byob_image *target,
    const byob_profile_selection *selection);
int bx_ntvdm_readonly_namespace_v1_append_terminal_quit(
    bx_ntvdm_readonly_namespace_v1 *value, const byob_image *terminal_quit,
    const byob_profile_selection *selection);
int bx_ntvdm_readonly_namespace_v1_open(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t drive_index,
    const wchar_t *canonical_path, uint32_t *token, uint64_t *byte_count);
int bx_ntvdm_readonly_namespace_v1_seek(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t token, uint64_t offset);
int bx_ntvdm_readonly_namespace_v1_read(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t token, uint8_t *destination,
    uint32_t requested_bytes, uint32_t *read_bytes);
int bx_ntvdm_readonly_namespace_v1_close(
    bx_ntvdm_readonly_namespace_v1 *value, uint32_t token);
int bx_ntvdm_readonly_namespace_v1_file_times(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t token,
    uint16_t *dos_time, uint16_t *dos_date);
/* The startup overlay intentionally exposes only the two source-derived
 * replacement images.  COMMAND/target profile entries remain outside the
 * direct-host DEM provider. */
int bx_ntvdm_readonly_namespace_v1_match_startup_path(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t drive_index,
    const wchar_t *canonical_path, uint64_t *byte_count_out);
/* Returns a one-based declared-image slot for a canonical pathname, or zero.
 * Unlike match_startup_path, this identity-only query does not require bytes
 * to be ready and never exposes the pathname or file storage. */
uint32_t bx_ntvdm_readonly_namespace_v1_declared_slot(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t drive_index,
    const wchar_t *canonical_path, uint32_t *bytes_ready_out);
int bx_ntvdm_readonly_namespace_v1_owns_token(
    const bx_ntvdm_readonly_namespace_v1 *value, uint32_t token);

#endif
