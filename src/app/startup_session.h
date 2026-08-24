#ifndef RUNTIME_STARTUP_SESSION_H
#define RUNTIME_STARTUP_SESSION_H

#include <stdint.h>
#include <wchar.h>

#include "startup_plan_abi.h"
#include "byob_image.h"

#define RUNTIME_STARTUP_SESSION_MAGIC 0x42585353u
#define RUNTIME_STARTUP_SESSION_VERSION 1u

/* Adapter-local same-island staging for the first cold-start mechanics
 * proof. It has no Bochs object, host-service or guest-service ABI. */
typedef struct runtime_startup_session_v1 {
    uint32_t magic;
    uint32_t version;
    byob_image ntio;
    byob_image ntdos;
    byob_image command;
    uint32_t has_command_placement;
    byob_guest_artifact_placement_descriptor command_placement;
    uint32_t has_host_drive_inventory;
    uint8_t host_drive_inventory_types[26];
} runtime_startup_session_v1;

#ifdef __cplusplus
extern "C" {
#endif

int runtime_startup_session_v1_install(runtime_startup_session_v1 *session,
    const wchar_t *profile_path, const wchar_t *byob_root);
void runtime_startup_session_v1_destroy(runtime_startup_session_v1 *session);

/* Produces only the existing generic NTIO write and real-mode entry values.
 * The returned payload remains adapter-owned and same-island only. */
int runtime_startup_session_v1_prepare_ntio_plan(
    const runtime_startup_session_v1 *session,
    runtime_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes);

int runtime_startup_session_v1_copy_host_drive_inventory(
    const runtime_startup_session_v1 *session, uint8_t out_types[26]);

#ifdef __cplusplus
}
#endif

#endif
