#include "dem_v2_startup_composition.h"

#include "shim/demmisc_shim.h"
#include "byob_image.h"
#include "byob_launch_plan_v2.h"
#include "byob_profile.h"
#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_guest_write_abi.h"
#include "bx_ntvdm_initial_state_catalog_v1.h"
#include "bx_ntvdm_startup_plan_abi.h"

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

typedef struct bx_ntvdm_dem_v2_startup {
    byob_image ntio, ntdos, command, target, terminal_quit;
    bx_ntvdm_initial_state_v1 initial_state;
    byob_launch_plan_v2 launch;
    char command_application[MAX_PATH + 1u];
    uint16_t command_drive;
    int installed;
} bx_ntvdm_dem_v2_startup;

static bx_ntvdm_dem_v2_startup runtime;

static int descriptor_to_wide(const uint16_t *source, uint32_t chars,
    wchar_t *destination, uint32_t capacity)
{
    uint32_t index;
    if (source == NULL || destination == NULL || chars == 0u || chars >= capacity)
        return 0;
    for (index = 0u; index < chars; ++index) {
        if (source[index] == 0u) return 0;
        destination[index] = (wchar_t)source[index];
    }
    destination[chars] = L'\0';
    return 1;
}

static int configure_opennt_dos_directory(const wchar_t *root,
    const byob_profile_selection *selection)
{
    size_t bytes;
    char *oem;

    if (root == NULL || selection == NULL ||
        _wcsicmp(selection->ntdos.file_name, L"ntdos.sys") != 0)
        return 0;
    bytes = (size_t)WideCharToMultiByte(CP_OEMCP, WC_NO_BEST_FIT_CHARS,
        root, -1, NULL, 0, NULL, NULL);
    if (bytes == 0u || bytes > MAX_PATH) return 0;
    oem = (char *)malloc(bytes + 14u);
    if (oem == NULL || WideCharToMultiByte(CP_OEMCP, WC_NO_BEST_FIT_CHARS,
            root, -1, oem, (int)bytes, NULL, NULL) == 0) {
        free(oem);
        return 0;
    }
    /* Divergence from OpenNT dem.c: DemInit uses the installed Windows system
     * directory.  The unpack-and-run CLI instead uses the validated BYOB
     * root, whose identity-checked ntdos.sys is the declared guest input. */
    free(pszDefaultDOSDirectory);
    pszDefaultDOSDirectory = oem;
    return 1;
}

static int configure_command_source(const uint16_t *launch_input,
    uint32_t launch_chars, const byob_profile_selection *selection)
{
    wchar_t launch_text[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];
    const wchar_t *extension;
    uint32_t expected_kind;
    size_t bytes;
    if (launch_input == NULL || selection == NULL ||
        !descriptor_to_wide(launch_input, launch_chars, launch_text,
            BYOB_LAUNCH_PLAN_V2_ENV_CHARS) ||
        !byob_launch_plan_v2_from_environment(&runtime.launch, launch_text) ||
        runtime.launch.slot_count != selection->declared_target_count) return 0;
    extension = wcsrchr(selection->target.file_name, L'.');
    expected_kind = extension != NULL && _wcsicmp(extension, L".COM") == 0 ? BYOB_LAUNCH_TARGET_KIND_V1_COM :
        extension != NULL && _wcsicmp(extension, L".EXE") == 0 ? BYOB_LAUNCH_TARGET_KIND_V1_EXE :
        extension != NULL && _wcsicmp(extension, L".BAT") == 0 ? BYOB_LAUNCH_TARGET_KIND_V1_BAT :
        extension != NULL && _wcsicmp(extension, L".PIF") == 0 ? BYOB_LAUNCH_TARGET_KIND_V1_PIF : 0u;
    if (runtime.launch.first.target_kind != expected_kind) return 0;
    bytes = (size_t)WideCharToMultiByte(CP_OEMCP, WC_NO_BEST_FIT_CHARS,
        selection->target.file_name, -1, runtime.command_application,
        (int)sizeof(runtime.command_application), NULL, NULL);
    if (bytes == 0u || bytes > sizeof(runtime.command_application)) return 0;
    runtime.command_drive = (uint16_t)selection->target_placement.drive_index;
    return 1;
}

void bx_ntvdm_dem_v2_startup_reset(void)
{
    byob_image_release(&runtime.ntio);
    byob_image_release(&runtime.ntdos);
    byob_image_release(&runtime.command);
    byob_image_release(&runtime.target);
    byob_image_release(&runtime.terminal_quit);
    bx_ntvdm_initial_state_v1_clear(&runtime.initial_state);
    memset(&runtime, 0, sizeof(runtime));
    free(pszDefaultDOSDirectory);
    pszDefaultDOSDirectory = NULL;
}

int bx_ntvdm_dem_v2_startup_install(const uint16_t *profile_input,
    uint32_t profile_chars, const uint16_t *root_input, uint32_t root_chars,
    const uint16_t *launch, uint32_t launch_chars, uint32_t include_mask,
    uint32_t exclude_mask, uint32_t mutation_mode)
{
    wchar_t profile[261], root[261];
    byob_profile_selection selection;

    (void)include_mask; (void)exclude_mask;
    if (runtime.installed) return 1;
    /* v2 is intentionally Direct-only; legacy mutation profiles are not a
     * substitute for an OpenNT owner contract. */
    if (mutation_mode != 1u || launch_chars == 0u ||
        !descriptor_to_wide(profile_input, profile_chars, profile, 261u) ||
        !descriptor_to_wide(root_input, root_chars, root, 261u) ||
        byob_profile_validate_file_select(profile, root, &selection) !=
            BYOB_PROFILE_ACCEPTED || selection.machine_startup_plan_enabled == 0u ||
        selection.machine_startup_entry_ntio_v0 == 0u ||
        byob_image_load_exact(root, &selection.ntio, &runtime.ntio) != BYOB_IMAGE_OK ||
        byob_image_load_exact(root, &selection.ntdos, &runtime.ntdos) != BYOB_IMAGE_OK ||
        byob_image_load_exact(root, &selection.command, &runtime.command) != BYOB_IMAGE_OK ||
        byob_image_load_exact(root, &selection.target, &runtime.target) != BYOB_IMAGE_OK ||
        (selection.declared_target_count == 2u &&
         byob_image_load_exact(root, &selection.terminal_quit,
             &runtime.terminal_quit) != BYOB_IMAGE_OK) ||
        !configure_opennt_dos_directory(root, &selection) ||
        !configure_command_source(launch, launch_chars, &selection)) {
        bx_ntvdm_dem_v2_startup_reset();
        return 0;
    }
    bx_ntvdm_initial_state_v1_clear(&runtime.initial_state);
    if (selection.has_machine_external_initial_state != 0u &&
        !bx_ntvdm_initial_state_catalog_v1_select(
            (const uint16_t *)selection.machine_external_initial_state_evidence_sha256,
            64u, &runtime.initial_state)) {
        bx_ntvdm_dem_v2_startup_reset();
        return 0;
    }
    runtime.installed = 1;
    return 1;
}

int bx_ntvdm_dem_v2_startup_copy_command_source(char *application,
    uint32_t application_capacity, char *tail, uint32_t tail_capacity,
    uint16_t *drive, uint16_t *code_page)
{
    uint32_t tail_bytes;
    if (!runtime.installed || application == NULL || tail == NULL || drive == NULL ||
        code_page == NULL || application_capacity == 0u || tail_capacity == 0u ||
        runtime.command_application[0] == '\0') return 0;
    tail_bytes = runtime.launch.first.tail_bytes;
    if (tail_bytes + 1u > tail_capacity ||
        strlen(runtime.command_application) + 1u > application_capacity) return 0;
    memcpy(application, runtime.command_application,
        strlen(runtime.command_application) + 1u);
    memcpy(tail, runtime.launch.first.tail, tail_bytes);
    tail[tail_bytes] = '\0';
    *drive = runtime.command_drive;
    /* OpenNT cmdMapCodePage maps the normal Western host code page to DOS 437.
     * The restricted launch declaration is ASCII, so this fixed initial value
     * preserves that original command-side contract without host-locale data. */
    *code_page = 437u;
    return 1;
}

static int startup_plan(bx_ntvdm_startup_plan_v1 *plan,
    const uint8_t **payload, uint64_t *payload_bytes)
{
    bx_ntvdm_cpu_state_v1 entry;
    bx_ntvdm_guest_write_v1 write;
    if (!runtime.installed || plan == NULL || payload == NULL ||
        payload_bytes == NULL || runtime.ntio.bytes == NULL ||
        runtime.ntio.byte_count == 0u) return 0;
    bx_ntvdm_cpu_state_v1_initialize(&entry, BX_NTVDM_CPU_EXECUTION_REAL);
    entry.cs = 0x70u;
    bx_ntvdm_guest_write_v1_initialize(&write, 0x700u,
        (uint64_t)runtime.ntio.byte_count, 0u);
    bx_ntvdm_startup_plan_v1_initialize(plan, &write, &entry, 0x714u, 4u);
    if (!bx_ntvdm_startup_plan_v1_preflight(plan, UINT64_C(0x100000),
            (uint64_t)runtime.ntio.byte_count)) return 0;
    *payload = runtime.ntio.bytes;
    *payload_bytes = (uint64_t)runtime.ntio.byte_count;
    return 1;
}

int bx_ntvdm_dem_v2_startup_prepare_machine_stage_request(
    struct bx_ntvdm_machine_stage_v1_request *request)
{
    bx_ntvdm_startup_plan_v1 plan;
    const uint8_t *payload;
    uint64_t payload_bytes;
    if (request == NULL || !startup_plan(&plan, &payload, &payload_bytes) ||
        payload_bytes > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES) return 0;
    bx_ntvdm_machine_stage_v1_request_clear(request);
    if (!bx_ntvdm_initial_state_action_v1_prepare(&runtime.initial_state,
            &request->initial_state_action)) return 0;
    /* This is the selector-blind mantle ABI initializer.  Request clear owns
     * the enclosing record only; each embedded action still needs its fixed
     * ABI identity before the copied OpenNT NTIO bytes are assigned. */
    bx_ntvdm_mechanical_action_v1_clear(&request->startup_action);
    request->startup_action.action_id = 2u;
    request->startup_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
    request->startup_action.range_count = 1u;
    request->startup_action.payload_bytes = (uint32_t)payload_bytes;
    request->startup_action.ranges[0].physical_address = plan.payload_write.guest_physical_address;
    request->startup_action.ranges[0].byte_count = (uint32_t)plan.payload_write.byte_count;
    memcpy(request->startup_action.payload, payload, (size_t)payload_bytes);
    request->preserved_state_address = plan.preserved_state_address;
    request->preserved_state_bytes = plan.preserved_state_bytes;
    return bx_ntvdm_machine_stage_v1_request_valid(request);
}

int bx_ntvdm_dem_v2_startup_prepare_machine_stage_entry(
    struct bx_ntvdm_machine_stage_v1_entry *entry)
{
    bx_ntvdm_startup_plan_v1 plan;
    const uint8_t *payload;
    uint64_t payload_bytes;
    if (entry == NULL || !startup_plan(&plan, &payload, &payload_bytes)) return 0;
    bx_ntvdm_machine_stage_v1_entry_clear(entry);
    entry->cs = plan.entry_cpu.cs;
    entry->eip = plan.entry_cpu.eip;
    return bx_ntvdm_machine_stage_v1_entry_valid(entry);
}

int bx_ntvdm_dem_v2_startup_copy_ordinary_terminal(void)
{
    /* COMMAND ownership is deliberately outside T230.  No legacy COMMAND
     * state is consulted merely to classify a Direct DEM terminal. */
    return 0;
}
