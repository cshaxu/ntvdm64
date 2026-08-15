#ifndef BX_NTVDM_SESSION_HOST_CONTEXT_V1_H
#define BX_NTVDM_SESSION_HOST_CONTEXT_V1_H

#include <stdint.h>
#include <wchar.h>

#include "bx_ntvdm_command_host_context_v1.h"
#include "bx_ntvdm_dem_profile_consumer_v1.h"
#include "bx_ntvdm_host_namespace.h"
#include "bx_ntvdm_mutation_overlay_v1.h"

#define BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAGIC 0x42585348u
#define BX_NTVDM_SESSION_HOST_CONTEXT_V1_VERSION 1u
#define BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE 128u

enum bx_ntvdm_session_host_context_v1_result {
    BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK = 1,
    BX_NTVDM_SESSION_HOST_CONTEXT_V1_REJECTED = 0,
    BX_NTVDM_SESSION_HOST_CONTEXT_V1_NOT_FOUND = -1,
    BX_NTVDM_SESSION_HOST_CONTEXT_V1_READONLY = -2,
    BX_NTVDM_SESSION_HOST_CONTEXT_V1_CAPACITY = -3,
    BX_NTVDM_SESSION_HOST_CONTEXT_V1_HOST_FAILURE = -4
};

/* Session-owned adapter state for the historical one-thread host CWD model.
 * It retains neither a raw host handle nor an absolute host pathname.  The
 * borrowed namespace stays adapter-private and supplies rooted validation. */
typedef struct bx_ntvdm_session_host_context_v1 {
    uint32_t magic, abi_version, struct_bytes, default_drive;
    bx_ntvdm_dem_profile_consumer_v1 profile;
    bx_ntvdm_mutation_overlay_v1 overlay;
    const bx_ntvdm_host_namespace_v1 *host_namespace;
    wchar_t direct_relative[26][BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE];
    wchar_t virtual_relative[26][BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE];
    uint8_t environment[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES];
    uint32_t environment_bytes;
} bx_ntvdm_session_host_context_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_session_host_context_v1_initialize(
    bx_ntvdm_session_host_context_v1 *context,
    const bx_ntvdm_mutation_profile_v1 *profile,
    const bx_ntvdm_host_namespace_v1 *host_namespace,
    uint8_t initial_drive);
int bx_ntvdm_session_host_context_v1_valid(
    const bx_ntvdm_session_host_context_v1 *context);
void bx_ntvdm_session_host_context_v1_teardown(
    bx_ntvdm_session_host_context_v1 *context);
int bx_ntvdm_session_host_context_v1_set_environment(
    bx_ntvdm_session_host_context_v1 *context, const uint8_t *environment,
    uint32_t environment_bytes);
int bx_ntvdm_session_host_context_v1_set(
    bx_ntvdm_session_host_context_v1 *context, uint8_t drive_index,
    const wchar_t *relative_directory, uint32_t *win32_error_out);
int bx_ntvdm_session_host_context_v1_query(
    const bx_ntvdm_session_host_context_v1 *context, uint8_t drive_index,
    wchar_t *relative_directory, uint32_t capacity);
int bx_ntvdm_session_host_context_v1_reconcile_direct(
    bx_ntvdm_session_host_context_v1 *context, uint8_t drive_index,
    uint32_t *win32_error_out);
int bx_ntvdm_session_host_context_v1_set_default_drive(
    bx_ntvdm_session_host_context_v1 *context, uint8_t drive_index);
int bx_ntvdm_session_host_context_v1_query_default_drive(
    const bx_ntvdm_session_host_context_v1 *context, uint8_t *drive_index_out);
int bx_ntvdm_session_host_context_v1_project_command(
    const bx_ntvdm_session_host_context_v1 *context,
    bx_ntvdm_command_host_context_v1 *projection);

#ifdef __cplusplus
}
#endif

#endif
