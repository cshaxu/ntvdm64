#include "bx_ntvdm_composition_runtime_v1.h"

#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx_ntvdm_native_bop_composition_v1.h"
#include "bx_ntvdm_host_drive_policy.h"
#include "bx_ntvdm_host_namespace.h"
#include "bx_ntvdm_host_volume_snapshot_v1.h"
#include "bx_ntvdm_mutation_profile_v1.h"
#include "bx_ntvdm_dem_profile_consumer_v1.h"
#include "byob_image.h"
#include "byob_launch_plan_v2.h"
#include "byob_profile.h"
#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_guest_write_abi.h"
#include "bx_ntvdm_initial_state_catalog_v1.h"
#include "bx_ntvdm_startup_configuration_source_v1.h"

#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define BX_NTVDM_COMPOSITION_ENV_PROFILE L"NTDOS64_ADAPTER_PROFILE"
#define BX_NTVDM_COMPOSITION_ENV_ROOT L"NTDOS64_ADAPTER_ROOT"
#define BX_NTVDM_COMPOSITION_ENV_LAUNCH_PLAN L"NTDOS64_ADAPTER_LAUNCH_PLAN"

typedef struct bx_ntvdm_composition_runtime_v1 {
    byob_image ntio, ntdos, command, target, terminal_quit;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    /* This is the capability-owning real-host namespace.  Its retained
     * handles remain private to bx-vdm and are released before any image or
     * provider lifetime is reset. */
    bx_ntvdm_host_namespace_v1 host_namespace;
    bx_ntvdm_host_volume_snapshot_v1 volumes;
    bx_ntvdm_mutation_profile_v1 mutation_profile;
    bx_ntvdm_command_host_context_v1 command_host_context;
    bx_ntvdm_boot_namespace_composition_v1 composition;
    bx_ntvdm_native_bop_composition_v1 native_bop;
    bx_ntvdm_initial_state_v1 initial_state;
    int attempted, installed;
} bx_ntvdm_composition_runtime_v1;

static bx_ntvdm_composition_runtime_v1 runtime;

static int command_environment_name_equal(const char *value, uint32_t bytes,
    const char *expected)
{
    uint32_t index;
    for (index = 0u; expected[index] != '\0'; ++index) {
        char character;
        if (index >= bytes) return 0;
        character = value[index];
        if (character >= 'a' && character <= 'z')
            character = (char)(character - 'a' + 'A');
        if (character != expected[index]) return 0;
    }
    return index == bytes;
}

static int capture_command_environment(
    bx_ntvdm_command_host_context_v1 *context)
{
    wchar_t *wide, *entry;
    uint8_t *environment;
    uint32_t used = 0u;
    int has_prompt = 0;
    int result = 0;
    if (context == 0) return 0;
    environment = (uint8_t *)malloc(BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES);
    if (environment == 0) return 0;
    wide = GetEnvironmentStringsW();
    if (wide == 0) { free(environment); return 0; }
    for (entry = wide; *entry != L'\0'; entry += wcslen(entry) + 1u) {
        int bytes;
        uint32_t index, name_bytes = 0u;
        if (entry[0] == L'=') continue;
        /* OpenNT's cmdGetInitEnvironment grows its copied block as needed.
           Query first, then write directly into the bounded 16-bit VDM
           transaction; no per-variable 1 KiB policy limit is introduced. */
        bytes = WideCharToMultiByte(CP_OEMCP, WC_NO_BEST_FIT_CHARS, entry, -1,
            0, 0, 0, 0);
        if (bytes < 3 || (uint32_t)bytes >
                BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES - used - 1u ||
            !WideCharToMultiByte(CP_OEMCP, WC_NO_BEST_FIT_CHARS, entry, -1,
                (char *)environment + used, bytes, 0, 0)) goto done;
        for (index = 0u; index < (uint32_t)bytes - 1u; ++index) {
            if (environment[used + index] == '=') { name_bytes = index; break; }
        }
        if (name_bytes == 0u || command_environment_name_equal(
                (const char *)environment + used,
                name_bytes, "COMSPEC") || command_environment_name_equal(
                (const char *)environment + used,
                name_bytes, "WINDIR")) continue;
        for (index = 0u; index < name_bytes; ++index)
            if (environment[used + index] >= 'a' && environment[used + index] <= 'z')
                environment[used + index] = (uint8_t)(environment[used + index] - 'a' + 'A');
        if (command_environment_name_equal((const char *)environment + used,
                name_bytes, "PROMPT"))
            has_prompt = 1;
        used += (uint32_t)bytes;
    }
    if (!has_prompt) {
        static const uint8_t prompt[] = "PROMPT=$P$G";
        if (used > BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES -
                sizeof(prompt) - 1u) goto done;
        memcpy(environment + used, prompt, sizeof(prompt));
        used += (uint32_t)sizeof(prompt);
    }
    if (used >= BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES) goto done;
    environment[used++] = 0u;
    result = bx_ntvdm_command_host_context_v1_set_environment(context,
        environment, used);
done:
    FreeEnvironmentStringsW(wide);
    free(environment);
    return result;
}

static int capture_command_host_context(
    bx_ntvdm_command_host_context_v1 *context, uint32_t selected_drive)
{
    wchar_t current[MAX_PATH];
    wchar_t processor_wide[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_PROCESSOR_BYTES];
    uint8_t root[4];
    char oem[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES];
    char processor[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_PROCESSOR_BYTES];
    DWORD characters;
    int bytes, processor_bytes;
    if (context == 0 || selected_drive >= 26u) return 0;
    characters = GetCurrentDirectoryW(MAX_PATH, current);
    if (characters > 2u && characters < MAX_PATH && current[1] == L':' &&
        ((current[0] >= L'A' && current[0] <= L'Z' &&
          (uint32_t)(current[0] - L'A') == selected_drive) ||
         (current[0] >= L'a' && current[0] <= L'z' &&
          (uint32_t)(current[0] - L'a') == selected_drive))) {
        bytes = WideCharToMultiByte(CP_OEMCP, WC_NO_BEST_FIT_CHARS, current,
            -1, oem, (int)sizeof(oem), 0, 0);
        if (bytes >= 4 && bytes <= (int)sizeof(oem) &&
            bx_ntvdm_command_host_context_v1_initialize(context,
                selected_drive, (const uint8_t *)oem, (uint32_t)bytes - 1u))
            goto capture;
    }
    root[0] = (uint8_t)('A' + selected_drive);
    root[1] = ':'; root[2] = '\\'; root[3] = '\0';
    if (!bx_ntvdm_command_host_context_v1_initialize(context, selected_drive,
            root, 3u)) return 0;
capture:
    if (!capture_command_environment(context)) return 0;
    characters = GetEnvironmentVariableW(L"ComSpec", processor_wide,
        BX_NTVDM_COMMAND_HOST_CONTEXT_V1_PROCESSOR_BYTES);
    if (characters == 0u || characters >= BX_NTVDM_COMMAND_HOST_CONTEXT_V1_PROCESSOR_BYTES)
        return bx_ntvdm_command_host_context_v1_valid(context);
    processor_bytes = WideCharToMultiByte(CP_OEMCP, WC_NO_BEST_FIT_CHARS,
        processor_wide, -1, processor, (int)sizeof(processor), 0, 0);
    return processor_bytes >= 2 && processor_bytes <= (int)sizeof(processor) &&
        bx_ntvdm_command_host_context_v1_set_processor(context,
            (const uint8_t *)processor, (uint32_t)processor_bytes);
}

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
    bx_ntvdm_dem_package_session_v1_teardown(&runtime.composition.dem);
    bx_ntvdm_host_namespace_v1_release(&runtime.host_namespace);
    byob_image_release(&runtime.ntio);
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
    bx_ntvdm_startup_configuration_input_v1 startup_input;
    uint32_t failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_PROFILE;

    if (runtime.installed) return 1;
    if (runtime.attempted) return -1;
    runtime.attempted = 1;
    bx_ntvdm_mutation_profile_v1_initialize(&runtime.mutation_profile,
        BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    if (!bx_ntvdm_dem_profile_consumer_v1_register_class(
            &runtime.mutation_profile,
            BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) ||
        !bx_ntvdm_command_profile_consumer_v1_register_class(
            &runtime.mutation_profile,
            BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) ||
        !bx_ntvdm_command_profile_consumer_v1_register_class(
            &runtime.mutation_profile,
            BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL, 0x01u) ||
        profile == 0 || root == 0 || launch_text == 0 ||
        byob_profile_validate_file_select(profile, root, &selection) !=
            BYOB_PROFILE_ACCEPTED ||
        selection.has_command_placement == 0u ||
        selection.has_guest_search_metadata == 0u ||
        (selection.declared_target_count != 1u &&
         selection.declared_target_count != 2u))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_IMAGES_AND_LAUNCH;
    if (!byob_launch_plan_v2_from_environment(&launch, launch_text) ||
        launch.slot_count != selection.declared_target_count ||
        byob_image_load_exact(root, &selection.ntio, &runtime.ntio) !=
            BYOB_IMAGE_OK ||
        byob_image_load_exact(root, &selection.ntdos, &runtime.ntdos) !=
            BYOB_IMAGE_OK ||
        byob_image_load_exact(root, &selection.command, &runtime.command) !=
            BYOB_IMAGE_OK ||
        byob_image_load_exact(root, &selection.target, &runtime.target) !=
            BYOB_IMAGE_OK ||
        (selection.declared_target_count == 2u &&
         byob_image_load_exact(root, &selection.terminal_quit,
             &runtime.terminal_quit) != BYOB_IMAGE_OK))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_HOST_CAPABILITY;
    if (
        !bx_ntvdm_host_drive_snapshot_v1_capture(include_mask, exclude_mask,
            &runtime.drives) ||
        selection.command_placement.drive_index >= 26u ||
        (runtime.drives.admitted_mask &
            (UINT32_C(1) << selection.command_placement.drive_index)) == 0u ||
        !bx_ntvdm_host_namespace_v1_initialize(&runtime.host_namespace,
            &runtime.drives) ||
        !bx_ntvdm_host_volume_snapshot_v1_capture(&runtime.drives,
            &runtime.volumes))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_NAMESPACE_INITIALIZE;
    if (!bx_ntvdm_boot_namespace_composition_v1_initialize(&runtime.composition,
            &runtime.ntdos, &runtime.command, &runtime.target,
            selection.declared_target_count == 2u ? &runtime.terminal_quit : 0,
            &selection))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_DRIVE_SNAPSHOT;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_drive_snapshot(&runtime.composition, &runtime.drives))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_VOLUME_SNAPSHOT;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_volume_snapshot(&runtime.composition, &runtime.volumes))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_DEM_PROFILE;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_dem_mutation_profile(&runtime.composition, &runtime.mutation_profile))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_COMMAND_PROFILE;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_command_mutation_profile(&runtime.composition, &runtime.mutation_profile))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_COMMAND_CONTEXT_CAPTURE;
    if (!capture_command_host_context(&runtime.command_host_context, selection.command_placement.drive_index))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_COMMAND_CONTEXT_BIND;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_command_host_context(&runtime.composition, &runtime.command_host_context))
        goto reject;
    /* T204 owns configuration source admission, transformation, contained
       image lifetime and COMMAND path publication.  Compose that existing
       package before the namespace is bound; do not revive profile bytes or
       add a selector-specific startup shortcut here. */
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_STARTUP_CONFIGURATION_BIND;
    if (!bx_ntvdm_startup_configuration_source_v1_from_environment(&startup_input,
            &runtime.mutation_profile) ||
        !bx_ntvdm_boot_namespace_composition_v1_set_startup_configuration(
            &runtime.composition, &startup_input))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_DEM_NAMESPACE_BIND;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_dem_host_namespace(&runtime.composition, &runtime.host_namespace))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_DEM_BOOT_DRIVE;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_dem_boot_drive(&runtime.composition, selection.command_placement.drive_index))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_LAUNCH_PLAN;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_launch_plan(&runtime.composition, &launch))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_COMPOSITION_BIND;
    if (!bx_ntvdm_boot_namespace_composition_v1_bind(&runtime.composition))
        goto reject;
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_NATIVE_BOP_BIND;
    if (!bx_ntvdm_native_bop_composition_v1_initialize(&runtime.native_bop) ||
        !bx_ntvdm_native_bop_composition_v1_bind(&runtime.native_bop))
        goto reject;
    bx_ntvdm_initial_state_v1_clear(&runtime.initial_state);
    failure_stage = BX_NTVDM_COMPOSITION_INSTALL_STAGE_V1_INITIAL_STATE;
    if (selection.has_machine_external_initial_state != 0u &&
        !bx_ntvdm_initial_state_catalog_v1_select(
            (const uint16_t *)selection.machine_external_initial_state_evidence_sha256,
            64u, &runtime.initial_state)) goto reject;
    runtime.installed = 1;
    return 1;

reject:
    bx_ntvdm_composition_runtime_v1_reset();
    runtime.attempted = 1;
    return -(int)failure_stage;
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

int bx_ntvdm_composition_runtime_v1_prepare_startup_plan(
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes)
{
    bx_ntvdm_cpu_state_v1 entry;
    bx_ntvdm_guest_write_v1 write;
    if (!runtime.installed || plan == 0 || payload == 0 || payload_bytes == 0 ||
        runtime.ntio.bytes == 0 || runtime.ntio.byte_count == 0u) return 0;
    /* NTIO v0's entry geometry is retained from the original startup-session
     * contract. The direct composition owns the profile-selected image. */
    bx_ntvdm_cpu_state_v1_initialize(&entry, BX_NTVDM_CPU_EXECUTION_REAL);
    entry.cs = 0x70u;
    bx_ntvdm_guest_write_v1_initialize(&write, 0x700u,
        (uint64_t)runtime.ntio.byte_count, 0u);
    bx_ntvdm_startup_plan_v1_initialize(plan, &write, &entry, 0x714u, 4u);
    if (!bx_ntvdm_startup_plan_v1_preflight(plan, 0x100000u,
        (uint64_t)runtime.ntio.byte_count)) return 0;
    *payload = runtime.ntio.bytes;
    *payload_bytes = (uint64_t)runtime.ntio.byte_count;
    return 1;
}

int bx_ntvdm_composition_runtime_v1_copy_initial_state(
    bx_ntvdm_initial_state_v1 *out)
{
    if (out == 0 || !runtime.installed ||
        !bx_ntvdm_initial_state_v1_admitted(&runtime.initial_state, 0x100000u))
        return 0;
    *out = runtime.initial_state;
    return 1;
}

int bx_ntvdm_composition_runtime_v1_prepare_initial_state_action(
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    if (!runtime.installed) return 0;
    return bx_ntvdm_initial_state_action_v1_prepare(&runtime.initial_state,
        action);
}

int bx_ntvdm_composition_runtime_v1_prepare_machine_stage_request(
    struct bx_ntvdm_machine_stage_v1_request *request)
{
    bx_ntvdm_startup_plan_v1 plan;
    const uint8_t *payload;
    uint64_t payload_bytes;

    if (request == 0 || !runtime.installed ||
        !bx_ntvdm_composition_runtime_v1_prepare_startup_plan(&plan, &payload,
            &payload_bytes) || payload == 0 ||
        payload_bytes > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES ||
        !bx_ntvdm_startup_plan_v1_preflight(&plan, UINT64_C(0x100000),
            payload_bytes)) return 0;
    bx_ntvdm_machine_stage_v1_request_clear(request);
    if (!bx_ntvdm_initial_state_action_v1_prepare(&runtime.initial_state,
            &request->initial_state_action)) return 0;
    bx_ntvdm_mechanical_action_v1_clear(&request->startup_action);
    request->startup_action.action_id = 2u;
    request->startup_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
    request->startup_action.range_count = 1u;
    request->startup_action.payload_bytes = (uint32_t) payload_bytes;
    request->startup_action.ranges[0].physical_address =
        plan.payload_write.guest_physical_address;
    request->startup_action.ranges[0].byte_count =
        (uint32_t) plan.payload_write.byte_count;
    request->startup_action.ranges[0].payload_offset = 0u;
    memcpy(request->startup_action.payload, payload, (size_t) payload_bytes);
    request->preserved_state_address = plan.preserved_state_address;
    request->preserved_state_bytes = plan.preserved_state_bytes;
    return bx_ntvdm_machine_stage_v1_request_valid(request);
}

int bx_ntvdm_composition_runtime_v1_prepare_machine_stage_entry(
    struct bx_ntvdm_machine_stage_v1_entry *entry)
{
    bx_ntvdm_startup_plan_v1 plan;
    const uint8_t *payload;
    uint64_t payload_bytes;

    if (entry == 0 || !runtime.installed ||
        !bx_ntvdm_composition_runtime_v1_prepare_startup_plan(&plan, &payload,
            &payload_bytes) || payload == 0 ||
        !bx_ntvdm_startup_plan_v1_preflight(&plan, UINT64_C(0x100000),
            payload_bytes)) return 0;
    bx_ntvdm_machine_stage_v1_entry_clear(entry);
    entry->cs = plan.entry_cpu.cs;
    entry->eip = plan.entry_cpu.eip;
    return bx_ntvdm_machine_stage_v1_entry_valid(entry);
}

int bx_ntvdm_composition_runtime_v1_copy_ordinary_terminal(void)
{
    return runtime.installed &&
        bx_ntvdm_boot_namespace_composition_v1_copy_ordinary_terminal(
            &runtime.composition);
}
