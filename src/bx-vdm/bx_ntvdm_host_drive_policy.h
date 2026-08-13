#ifndef BX_NTVDM_HOST_DRIVE_POLICY_H
#define BX_NTVDM_HOST_DRIVE_POLICY_H

#include <stdint.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC 0x42584453u
#define BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION 1u

/* Adapter-local session data.  It is neither a Bochs ABI nor a guest ABI.
 * `types` uses the Win32 drive-type numeric domain; a zero type means that
 * the letter was not admitted to this immutable session snapshot. */
typedef struct bx_ntvdm_host_drive_snapshot_v1 {
    uint32_t magic;
    uint32_t version;
    uint32_t present_mask;
    uint32_t include_mask;
    uint32_t exclude_mask;
    uint32_t admitted_mask;
    uint8_t types[26];
} bx_ntvdm_host_drive_snapshot_v1;

/* Parses a CLI-normalized comma-separated drive list into a letter bitmask.
 * Empty is the deliberate "no restriction" representation. */
int bx_ntvdm_host_drive_policy_v1_parse(const wchar_t *text,
    uint32_t *out_mask);

/* Applies the explicit policy to an already observed host inventory.  An
 * exclusion always wins; no include list means every observed letter starts
 * eligible.  The caller supplies the observation so this function is fully
 * deterministic and has no host side effect. */
int bx_ntvdm_host_drive_snapshot_v1_apply(uint32_t present_mask,
    const uint8_t types[26], uint32_t include_mask, uint32_t exclude_mask,
    bx_ntvdm_host_drive_snapshot_v1 *out);

/* Captures the real Win32 logical-drive inventory once, then applies the two
 * inherited CLI policy values.  It opens no paths and is never callable from
 * Bochs or a guest request path. */
int bx_ntvdm_host_drive_snapshot_v1_capture_from_environment(
    bx_ntvdm_host_drive_snapshot_v1 *out);

int bx_ntvdm_host_drive_snapshot_v1_valid(
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot);

#ifdef __cplusplus
}
#endif

#endif
