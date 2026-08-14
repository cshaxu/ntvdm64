#include "bx_ntvdm_composition_runtime_v1.h"

#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx_ntvdm_host_drive_policy.h"
#include "bx_ntvdm_host_volume_snapshot_v1.h"
#include "byob_image.h"
#include "byob_launch_plan_v2.h"
#include "byob_profile.h"

#include <string.h>
#include <windows.h>

#define BX_NTVDM_COMPOSITION_ENV_PROFILE L"NTDOS64_ADAPTER_PROFILE"
#define BX_NTVDM_COMPOSITION_ENV_ROOT L"NTDOS64_ADAPTER_ROOT"
#define BX_NTVDM_COMPOSITION_ENV_LAUNCH_PLAN L"NTDOS64_ADAPTER_LAUNCH_PLAN"

typedef struct bx_ntvdm_composition_runtime_v1 {
    byob_image ntdos, command, target, terminal_quit;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    bx_ntvdm_host_volume_snapshot_v1 volumes;
    bx_ntvdm_boot_namespace_composition_v1 composition;
    int attempted, installed;
} bx_ntvdm_composition_runtime_v1;

static bx_ntvdm_composition_runtime_v1 runtime;

void bx_ntvdm_composition_runtime_v1_reset(void)
{
    if (runtime.installed) {
        bx_ntvdm_boot_namespace_composition_v1_unbind(&runtime.composition);
        bx_ntvdm_search_transaction_v1_release(
            &runtime.composition.plane.provider.search_transaction);
    }
    byob_image_release(&runtime.ntdos);
    byob_image_release(&runtime.command);
    byob_image_release(&runtime.target);
    byob_image_release(&runtime.terminal_quit);
    memset(&runtime, 0, sizeof(runtime));
}

int bx_ntvdm_composition_runtime_v1_install_from_environment(void)
{
    byob_profile_selection selection;
    byob_launch_plan_v2 launch;
    wchar_t profile[MAX_PATH], root[MAX_PATH];
    wchar_t launch_text[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];
    DWORD profile_size, root_size, launch_size, profile_error, root_error;

    if (runtime.installed) return 1;
    if (runtime.attempted) return -1;
    runtime.attempted = 1;
    SetLastError(ERROR_SUCCESS);
    profile_size = GetEnvironmentVariableW(BX_NTVDM_COMPOSITION_ENV_PROFILE,
        profile, MAX_PATH);
    profile_error = GetLastError();
    SetLastError(ERROR_SUCCESS);
    root_size = GetEnvironmentVariableW(BX_NTVDM_COMPOSITION_ENV_ROOT,
        root, MAX_PATH);
    root_error = GetLastError();
    if (profile_size == 0u && root_size == 0u &&
        profile_error == ERROR_ENVVAR_NOT_FOUND &&
        root_error == ERROR_ENVVAR_NOT_FOUND)
        return 0;
    if (profile_size == 0u || root_size == 0u || profile_size >= MAX_PATH ||
        root_size >= MAX_PATH ||
        byob_profile_validate_file_select(profile, root, &selection) !=
            BYOB_PROFILE_ACCEPTED ||
        selection.has_command_placement == 0u ||
        selection.has_guest_search_metadata == 0u ||
        (selection.declared_target_count != 1u &&
         selection.declared_target_count != 2u))
        goto reject;
    launch_size = GetEnvironmentVariableW(BX_NTVDM_COMPOSITION_ENV_LAUNCH_PLAN,
        launch_text, BYOB_LAUNCH_PLAN_V2_ENV_CHARS);
    if (launch_size == 0u || launch_size >= BYOB_LAUNCH_PLAN_V2_ENV_CHARS ||
        !byob_launch_plan_v2_from_environment(&launch, launch_text) ||
        launch.slot_count != selection.declared_target_count ||
        byob_image_load_exact(root, &selection.ntdos, &runtime.ntdos) !=
            BYOB_IMAGE_OK ||
        byob_image_load_exact(root, &selection.command, &runtime.command) !=
            BYOB_IMAGE_OK ||
        byob_image_load_exact(root, &selection.target, &runtime.target) !=
            BYOB_IMAGE_OK ||
        (selection.declared_target_count == 2u &&
         byob_image_load_exact(root, &selection.terminal_quit,
             &runtime.terminal_quit) != BYOB_IMAGE_OK) ||
        !bx_ntvdm_host_drive_snapshot_v1_capture_from_environment(
            &runtime.drives) ||
        !bx_ntvdm_host_volume_snapshot_v1_capture(&runtime.drives,
            &runtime.volumes) ||
        !bx_ntvdm_boot_namespace_composition_v1_initialize(&runtime.composition,
            &runtime.ntdos, &runtime.command, &runtime.target,
            selection.declared_target_count == 2u ? &runtime.terminal_quit : 0,
            &selection) ||
        !bx_ntvdm_boot_namespace_composition_v1_set_drive_snapshot(
            &runtime.composition, &runtime.drives) ||
        !bx_ntvdm_boot_namespace_composition_v1_set_volume_snapshot(
            &runtime.composition, &runtime.volumes) ||
        !bx_ntvdm_boot_namespace_composition_v1_set_launch_plan(
            &runtime.composition, &launch) ||
        !bx_ntvdm_boot_namespace_composition_v1_bind(&runtime.composition))
        goto reject;
    runtime.installed = 1;
    return 1;

reject:
    bx_ntvdm_composition_runtime_v1_reset();
    runtime.attempted = 1;
    return -1;
}
