#ifndef BX_NTVDM_DEM_WHOLE_PROVIDER_V1_H
#define BX_NTVDM_DEM_WHOLE_PROVIDER_V1_H

#include "bx_ntvdm_dem_local_file_backend_v1.h"
#include "bx_ntvdm_search_transaction_v1.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_readonly_namespace.h"
#include "bx_ntvdm_profile_search_snapshot_v1.h"

#define BX_NTVDM_DEM_WHOLE_PROVIDER_V1_MAGIC 0x42585750u
#define BX_NTVDM_DEM_WHOLE_PROVIDER_V1_VERSION 1u

/* This is the single state owner for T202/S4's 29 local-file identities.
 * It is deliberately not a BOP ingress or CPU/guest-memory interface. */
typedef struct bx_ntvdm_dem_whole_provider_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t reserved0;
    const bx_ntvdm_host_namespace_v1 *host_namespace;
    const bx_ntvdm_dem_cwd_context_v1 *cwd;
    /* Borrowed from the boot/startup provider.  It is restricted to its two
     * generated startup images and contains no host handle or guest pointer. */
    bx_ntvdm_readonly_namespace_v1 *startup_namespace;
    const bx_ntvdm_profile_search_snapshot_v1 *declared_search_snapshot;
    bx_ntvdm_dem_file_session_v1 files;
    bx_ntvdm_dem_local_file_backend_v1 local_files;
    bx_ntvdm_search_transaction_v1 search;
    /* `demsrch.c` PATHFIRST resolves the registered DTA far pointer.  The
     * resulting guest-physical DTA is retained only for the matching
     * PATHNEXT transaction; it is never a host pointer. */
    uint64_t path_search_dta_address;
    uint32_t next_action_id;
    uint32_t pending_service;
    uint32_t pending_action_id;
    uint32_t pending_bytes;
    bx_ntvdm_exception_event_v1 pending_boundary;
    bx_ntvdm_cpu_state_v1 pending_cpu;
    bx_ntvdm_guest_gather_read_action_v1 pending_gather;
} bx_ntvdm_dem_whole_provider_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_dem_whole_provider_v1_initialize(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_mutation_profile_v1 *profile,
    const bx_ntvdm_host_namespace_v1 *host_namespace,
    const bx_ntvdm_dem_cwd_context_v1 *cwd);
int bx_ntvdm_dem_whole_provider_v1_valid(
    const bx_ntvdm_dem_whole_provider_v1 *provider);
void bx_ntvdm_dem_whole_provider_v1_teardown(
    bx_ntvdm_dem_whole_provider_v1 *provider);
int bx_ntvdm_dem_whole_provider_v1_set_startup_namespace(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    bx_ntvdm_readonly_namespace_v1 *startup_namespace);
int bx_ntvdm_dem_whole_provider_v1_set_declared_search_snapshot(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_profile_search_snapshot_v1 *snapshot);

/* Membership is an owner-package guard. It does not imply implementation of
 * the service or permit a selector-specific profile decision. */
int bx_ntvdm_dem_whole_provider_v1_owns_service(uint8_t service);

/* One provider-private, copied gather continuation. The returned action is a
 * mechanical request only; completion verifies its exact copied boundary and
 * copies the bytes into caller storage before clearing the pending state. */
int bx_ntvdm_dem_whole_provider_v1_prepare_gather(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_range *ranges, uint32_t range_count,
    bx_ntvdm_guest_gather_read_action_v1 *action_out);
int bx_ntvdm_dem_whole_provider_v1_complete_gather(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint32_t byte_count,
    uint8_t copied_bytes[BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES],
    uint32_t *copied_byte_count);
/* A mechanical read may be declined before it produces copied bytes.  Cancel
 * only the exact provider-owned continuation so a failed transport cannot
 * leave a stale guest operation blocking a later BOP. */
int bx_ntvdm_dem_whole_provider_v1_cancel_gather(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action);

#ifdef __cplusplus
}
#endif

#endif
