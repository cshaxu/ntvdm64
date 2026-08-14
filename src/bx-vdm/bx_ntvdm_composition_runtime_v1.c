#include "bx_ntvdm_composition_runtime_v1.h"

#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx_ntvdm_native_bop_composition_v1.h"
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
    bx_ntvdm_native_bop_composition_v1 native_bop;
    int attempted, installed;
} bx_ntvdm_composition_runtime_v1;

static bx_ntvdm_composition_runtime_v1 runtime;

void bx_ntvdm_composition_runtime_v1_reset(void)
{
    /* Binding either half succeeds independently.  Rejection after the first
     * bind must therefore undo both halves, rather than leaving an active
     * provider behind for a later CLI invocation. */
    bx_ntvdm_native_bop_composition_v1_unbind(&runtime.native_bop);
    if (runtime.installed || runtime.composition.bound) {
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

static int install(const wchar_t *profile, const wchar_t *root,
    const wchar_t *launch_text, uint32_t include_mask, uint32_t exclude_mask)
{
    byob_profile_selection selection;
    byob_launch_plan_v2 launch;

    if (runtime.installed) return 1;
    if (runtime.attempted) return -1;
    runtime.attempted = 1;
    if (profile == 0 || root == 0 || launch_text == 0 ||
        byob_profile_validate_file_select(profile, root, &selection) !=
            BYOB_PROFILE_ACCEPTED ||
        selection.has_command_placement == 0u ||
        selection.has_guest_search_metadata == 0u ||
        (selection.declared_target_count != 1u &&
         selection.declared_target_count != 2u))
        goto reject;
    if (!byob_launch_plan_v2_from_environment(&launch, launch_text) ||
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
        !bx_ntvdm_host_drive_snapshot_v1_capture(include_mask, exclude_mask,
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
        !bx_ntvdm_boot_namespace_composition_v1_bind(&runtime.composition) ||
        !bx_ntvdm_native_bop_composition_v1_initialize(&runtime.native_bop) ||
        !bx_ntvdm_native_bop_composition_v1_bind(&runtime.native_bop))
        goto reject;
    runtime.installed = 1;
    return 1;

reject:
    bx_ntvdm_composition_runtime_v1_reset();
    runtime.attempted = 1;
    return -1;
}

int bx_ntvdm_composition_runtime_v1_install_from_environment(void)
{
    wchar_t profile[MAX_PATH], root[MAX_PATH];
    wchar_t launch_text[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];
    DWORD profile_size, root_size, launch_size, profile_error, root_error;
    uint32_t include_mask, exclude_mask;
    wchar_t include_text[52] = {0}, exclude_text[52] = {0};
    DWORD include_size, exclude_size;

    if (runtime.installed) return 1;
    if (runtime.attempted) return -1;
    SetLastError(ERROR_SUCCESS);
    profile_size = GetEnvironmentVariableW(BX_NTVDM_COMPOSITION_ENV_PROFILE, profile, MAX_PATH);
    profile_error = GetLastError();
    SetLastError(ERROR_SUCCESS);
    root_size = GetEnvironmentVariableW(BX_NTVDM_COMPOSITION_ENV_ROOT, root, MAX_PATH);
    root_error = GetLastError();
    if (profile_size == 0u && root_size == 0u && profile_error == ERROR_ENVVAR_NOT_FOUND && root_error == ERROR_ENVVAR_NOT_FOUND) return 0;
    launch_size = GetEnvironmentVariableW(BX_NTVDM_COMPOSITION_ENV_LAUNCH_PLAN, launch_text, BYOB_LAUNCH_PLAN_V2_ENV_CHARS);
    include_size = GetEnvironmentVariableW(L"NTDOS64_HOST_INCLUDE_DRIVES", include_text, 52u);
    exclude_size = GetEnvironmentVariableW(L"NTDOS64_HOST_EXCLUDE_DRIVES", exclude_text, 52u);
    if (profile_size == 0u || root_size == 0u || profile_size >= MAX_PATH || root_size >= MAX_PATH ||
        launch_size == 0u || launch_size >= BYOB_LAUNCH_PLAN_V2_ENV_CHARS || include_size >= 52u || exclude_size >= 52u ||
        !bx_ntvdm_host_drive_policy_v1_parse(include_text, &include_mask) ||
        !bx_ntvdm_host_drive_policy_v1_parse(exclude_text, &exclude_mask)) {
        runtime.attempted = 1;
        return -1;
    }
    return install(profile, root, launch_text, include_mask, exclude_mask);
}

static int descriptor_to_wide(const uint16_t *source, uint32_t chars,
    wchar_t *destination, uint32_t capacity)
{
    uint32_t index;
    if (source == 0 || destination == 0 || chars >= capacity) return 0;
    for (index = 0u; index < chars; ++index) destination[index] = (wchar_t) source[index];
    destination[chars] = L'\0';
    return 1;
}

int bx_ntvdm_composition_runtime_v1_install_from_copied_input(
    const uint16_t *profile_input, uint32_t profile_chars,
    const uint16_t *root_input, uint32_t root_chars,
    const uint16_t *launch_input, uint32_t launch_chars,
    uint32_t include_mask, uint32_t exclude_mask)
{
    wchar_t profile[261], root[261], launch[257];
    if (!descriptor_to_wide(profile_input, profile_chars, profile, 261u) ||
        !descriptor_to_wide(root_input, root_chars, root, 261u) ||
        !descriptor_to_wide(launch_input, launch_chars, launch, 257u)) return -1;
    return install(profile, root, launch, include_mask, exclude_mask);
}
