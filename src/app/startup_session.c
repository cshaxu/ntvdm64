#include "startup_session.h"

#include <string.h>

#include "cpu_state_abi.h"
#include "byob_profile.h"

static int runtime_startup_session_v1_load(
    const wchar_t *root, const byob_profile_selection *selection,
    runtime_startup_session_v1 *session)
{
    return selection != 0 &&
        byob_image_load_exact(root, &selection->ntio, &session->ntio) == BYOB_IMAGE_OK &&
        byob_image_load_exact(root, &selection->ntdos, &session->ntdos) == BYOB_IMAGE_OK &&
        byob_image_load_exact(root, &selection->command, &session->command) == BYOB_IMAGE_OK;
}

void runtime_startup_session_v1_destroy(runtime_startup_session_v1 *session)
{
    if (session == 0) return;
    byob_image_release(&session->ntio);
    byob_image_release(&session->ntdos);
    byob_image_release(&session->command);
    memset(session, 0, sizeof(*session));
}

int runtime_startup_session_v1_install(runtime_startup_session_v1 *session,
    const wchar_t *profile_path, const wchar_t *byob_root)
{
    byob_profile_selection selection;
    if (session == 0 || profile_path == 0 || byob_root == 0) return 0;
    runtime_startup_session_v1_destroy(session);
    memset(&selection, 0, sizeof(selection));
    if (byob_profile_validate_file_select(profile_path, byob_root, &selection) !=
            BYOB_PROFILE_ACCEPTED ||
        selection.machine_startup_plan_enabled == 0u ||
        selection.machine_startup_entry_ntio_v0 == 0u ||
        !runtime_startup_session_v1_load(byob_root, &selection, session)) {
        runtime_startup_session_v1_destroy(session);
        return 0;
    }
    session->magic = RUNTIME_STARTUP_SESSION_MAGIC;
    session->version = RUNTIME_STARTUP_SESSION_VERSION;
    if (selection.has_host_drive_inventory != 0u) {
        memcpy(session->host_drive_inventory_types,
            selection.host_drive_inventory.types,
            sizeof(session->host_drive_inventory_types));
        session->has_host_drive_inventory = 1u;
    }
    if (selection.has_command_placement != 0u) {
        memcpy(&session->command_placement, &selection.command_placement,
            sizeof(session->command_placement));
        session->has_command_placement = 1u;
    }
    return 1;
}

int runtime_startup_session_v1_copy_host_drive_inventory(
    const runtime_startup_session_v1 *session, uint8_t out_types[26])
{
    if (session == 0 || out_types == 0 ||
        session->magic != RUNTIME_STARTUP_SESSION_MAGIC ||
        session->version != RUNTIME_STARTUP_SESSION_VERSION ||
        session->has_host_drive_inventory == 0u) return 0;
    memcpy(out_types, session->host_drive_inventory_types,
        sizeof(session->host_drive_inventory_types));
    return 1;
}

int runtime_startup_session_v1_prepare_ntio_plan(
    const runtime_startup_session_v1 *session,
    runtime_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes)
{
    runtime_cpu_state_v1 entry;
    runtime_guest_write_v1 write;
    if (session == 0 || plan == 0 || payload == 0 || payload_bytes == 0 ||
        session->magic != RUNTIME_STARTUP_SESSION_MAGIC ||
        session->version != RUNTIME_STARTUP_SESSION_VERSION ||
        session->ntio.bytes == 0 || session->ntio.byte_count == 0u)
        return 0;
    runtime_cpu_state_v1_initialize(&entry, RUNTIME_CPU_EXECUTION_REAL);
    entry.cs = 0x70u;
    runtime_guest_write_v1_initialize(&write, 0x700u,
        (uint64_t)session->ntio.byte_count, 0u);
    runtime_startup_plan_v1_initialize(plan, &write, &entry, 0x714u, 4u);
    if (!runtime_startup_plan_v1_preflight(plan, 0x100000u,
        (uint64_t)session->ntio.byte_count)) return 0;
    *payload = session->ntio.bytes;
    *payload_bytes = (uint64_t)session->ntio.byte_count;
    return 1;
}
