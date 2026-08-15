#ifndef BX_NTVDM_HOST_NAMESPACE_H
#define BX_NTVDM_HOST_NAMESPACE_H

#include <stdint.h>
#include <windows.h>

#include "bx_ntvdm_host_drive_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BX_NTVDM_HOST_NAMESPACE_V1_MAGIC 0x42584e53u
#define BX_NTVDM_HOST_NAMESPACE_V1_VERSION 1u
#define BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES 4096u

enum bx_ntvdm_host_namespace_v1_result {
    BX_NTVDM_HOST_NAMESPACE_V1_OK = 1,
    BX_NTVDM_HOST_NAMESPACE_V1_REJECTED = 0,
    BX_NTVDM_HOST_NAMESPACE_V1_CAPACITY = -1,
    BX_NTVDM_HOST_NAMESPACE_V1_COLLISION = -2
};

/* An adapter-local, DOS-visible directory entry.  It deliberately contains
 * neither a host path nor a host handle. */
typedef struct bx_ntvdm_host_namespace_entry_v1 {
    wchar_t dos_name[13];
    uint32_t attributes;
    uint64_t byte_count;
    uint16_t dos_time;
    uint16_t dos_date;
} bx_ntvdm_host_namespace_entry_v1;

/* Root handles are opened once from the immutable drive snapshot.  They never
 * cross the adapter boundary. */
typedef struct bx_ntvdm_host_namespace_v1 {
    uint32_t magic;
    uint32_t version;
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    HANDLE roots[26];
    uint32_t available_mask;
} bx_ntvdm_host_namespace_v1;

int bx_ntvdm_host_namespace_v1_initialize(
    bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot);
void bx_ntvdm_host_namespace_v1_release(
    bx_ntvdm_host_namespace_v1 *space);
int bx_ntvdm_host_namespace_v1_valid(
    const bx_ntvdm_host_namespace_v1 *space);

/* Enumerates one existing DOS-relative directory under an admitted drive.
 * `relative_directory` is a backslash-separated sequence of DOS-visible 8.3
 * component names, or empty for the drive root.  It is never an absolute host
 * path.  Reparse points and entries lacking an unambiguous 8.3 projection are
 * excluded. Results are sorted by DOS name. */
int bx_ntvdm_host_namespace_v1_enumerate(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_directory,
    bx_ntvdm_host_namespace_entry_v1 *entries, uint32_t entry_capacity,
    uint32_t *out_count);

/* Confirms that one DOS-relative directory exists below an admitted root.
 * It exposes no host path or handle and changes no ambient process directory.
 * An empty relative name denotes the admitted drive root. */
int bx_ntvdm_host_namespace_v1_directory_exists(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_directory);

/* Opens one DOS-relative 8.3 file below an admitted root.  The returned
 * handle remains adapter-private: callers must place it in a session-owned
 * opaque-token table, never in guest state.  Absolute paths, dot segments,
 * non-8.3 components and reparse traversal are rejected. */
int bx_ntvdm_host_namespace_v1_open_file(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, ACCESS_MASK desired_access,
    ULONG share_access, DWORD creation_disposition, HANDLE *handle_out);

/* The extended form retains the native failure as a Win32 error value for a
 * source-derived provider's demClientError-shaped result.  Like the basic
 * form, it keeps the root and returned handle adapter-private. */
int bx_ntvdm_host_namespace_v1_open_file_ex(
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive_index,
    const wchar_t *relative_path, ACCESS_MASK desired_access,
    ULONG share_access, DWORD creation_disposition, HANDLE *handle_out,
    DWORD *win32_error_out);

#ifdef __cplusplus
}
#endif

#endif
