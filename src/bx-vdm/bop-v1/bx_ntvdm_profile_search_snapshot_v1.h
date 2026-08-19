#ifndef BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_H
#define BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_H

#include "bx_ntvdm_host_namespace.h"
#include "bx_ntvdm_readonly_namespace.h"

#define BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_MAGIC 0x42585053u
#define BX_NTVDM_PROFILE_SEARCH_SNAPSHOT_V1_VERSION 1u

/* An adapter-local, immutable projection of the closed BYOB resource set.
 * It deliberately reuses only the DOS-entry value type; it never initializes
 * or accepts bx_ntvdm_host_namespace_v1, a root handle, or a host pathname. */
typedef struct bx_ntvdm_profile_search_snapshot_v1 {
    uint32_t magic;
    uint32_t version;
    uint32_t drive_index;
    uint32_t entry_count;
    bx_ntvdm_host_namespace_entry_v1 entries[BX_NTVDM_READONLY_NAMESPACE_MAX_FILES];
} bx_ntvdm_profile_search_snapshot_v1;

int bx_ntvdm_profile_search_snapshot_v1_initialize(
    bx_ntvdm_profile_search_snapshot_v1 *value,
    const bx_ntvdm_readonly_namespace_v1 *namespace_value,
    const byob_profile_selection *selection);
int bx_ntvdm_profile_search_snapshot_v1_reproject_contents(
    bx_ntvdm_profile_search_snapshot_v1 *value,
    const bx_ntvdm_readonly_namespace_v1 *namespace_value);
int bx_ntvdm_profile_search_snapshot_v1_valid(
    const bx_ntvdm_profile_search_snapshot_v1 *value);

#endif
