#ifndef BX_NTVDM_NAMESPACE_ROOT_SET_H
#define BX_NTVDM_NAMESPACE_ROOT_SET_H

#include <stdint.h>
#include <windows.h>

#include "bx_ntvdm_host_drive_policy.h"

typedef struct bx_ntvdm_namespace_root_set_v1 {
    uint32_t magic, abi_version;
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    HANDLE roots[26];
    uint32_t available_mask;
} bx_ntvdm_namespace_root_set_v1;

#define BX_NTVDM_NAMESPACE_ROOT_SET_V1_MAGIC 0x42585253u
#define BX_NTVDM_NAMESPACE_ROOT_SET_V1_VERSION 1u

int bx_ntvdm_namespace_root_set_v1_install(bx_ntvdm_namespace_root_set_v1 *set,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot);
void bx_ntvdm_namespace_root_set_v1_release(bx_ntvdm_namespace_root_set_v1 *set);
int bx_ntvdm_namespace_root_set_v1_valid(
    const bx_ntvdm_namespace_root_set_v1 *set);

#endif
