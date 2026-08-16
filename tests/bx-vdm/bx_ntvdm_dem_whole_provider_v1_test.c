#include "bx_ntvdm_dem_whole_provider_v1.h"
#include "bx_ntvdm_dem_handle_partition_v1.h"
#include "bx_ntvdm_dem_namespace_partition_v1.h"
#include "bx_ntvdm_dem_namespace_identity_observation_v1.h"
#include "bx_ntvdm_dem_namespace_route_partition_v1.h"
#include "bx_ntvdm_dem_handle_route_partition_v1.h"
#include "bx_ntvdm_dem_overlay_handle_backend_v1.h"
#include "bx_ntvdm_dem_overlay_namespace_view_v1.h"
#include "bx_ntvdm_dem_overlay_mutation_backend_v1.h"
#include "bx_ntvdm_dem_fcb_overlay_backend_v1.h"
#include "bx_ntvdm_dem_fcb_handle_partition_v1.h"
#include "bx_ntvdm_dem_fcb_wildcard_partition_v1.h"
#include "bx_ntvdm_dem_fcb_io_route_partition_v1.h"
#include "bx_ntvdm_dem_fcb_path_route_partition_v1.h"

#include <stdio.h>
#include <string.h>
#include <wctype.h>

static int profile_for_mode(bx_ntvdm_mutation_profile_v1 *profile, uint32_t mode)
{
    bx_ntvdm_mutation_profile_v1_initialize(profile, mode);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, 0x0fu);
}

static int profile_for(bx_ntvdm_mutation_profile_v1 *profile)
{ return profile_for_mode(profile, BX_NTVDM_MUTATION_MODE_V1_DIRECT); }

static int cf_set(const bx_ntvdm_cpu_result_v2 *result)
{
    return (result->eflags_write_mask & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u &&
        (result->eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u;
}

static int ax_is(const bx_ntvdm_cpu_result_v2 *result, uint16_t value)
{
    return (result->cpu_delta.gpr16_write_mask & 1u) != 0u &&
        result->cpu_delta.gpr16_values[0] == value;
}

static void token_into_cpu(bx_ntvdm_cpu_state_v1 *cpu, uint32_t token)
{
    cpu->eax = token >> 16;
    cpu->ebp = token & 0xffffu;
}

static int oem_to_wide(const char *source, wchar_t target[MAX_PATH])
{
    return source != 0 && target != 0 && MultiByteToWideChar(CP_OEMCP, 0,
        source, -1, target, MAX_PATH) != 0;
}

static int replace_extension(char target[MAX_PATH], const char *source,
    const char *extension)
{
    char *dot;
    if (target == 0 || source == 0 || extension == 0 ||
        strcpy_s(target, MAX_PATH, source) != 0) return 0;
    dot = strrchr(target, '.');
    return dot != 0 && strcpy_s(dot, MAX_PATH - (size_t)(dot - target), extension) == 0;
}

static int create_oem_file(const char *path)
{
    wchar_t wide[MAX_PATH];
    HANDLE file;
    if (!oem_to_wide(path, wide)) return 0;
    file = CreateFileW(wide, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE |
        FILE_SHARE_DELETE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) return 0;
    return CloseHandle(file) != 0;
}

static int oem_file_exists(const char *path)
{
    wchar_t wide[MAX_PATH];
    return oem_to_wide(path, wide) && GetFileAttributesW(wide) != INVALID_FILE_ATTRIBUTES;
}

static void delete_oem_file(const char *path)
{
    wchar_t wide[MAX_PATH];
    if (oem_to_wide(path, wide)) DeleteFileW(wide);
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_host_drive_snapshot_v1 snapshot = {0};
    bx_ntvdm_host_namespace_v1 space;
    bx_ntvdm_dem_cwd_context_v1 cwd;
    bx_ntvdm_dem_whole_provider_v1 provider;
    bx_ntvdm_readonly_namespace_v1 startup_images = {0};
    bx_ntvdm_exception_event_v1 boundary = {0};
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_guest_gather_read_action_v1 action;
    bx_ntvdm_guest_range range = { 0x200u, 3u };
    uint8_t input[3] = { 1u, 2u, 3u }, output[256] = {0};
    uint32_t output_bytes = 0u;
    uint32_t token = 0u, fcb_token = 0u;
    HANDLE file = INVALID_HANDLE_VALUE;
    DWORD written = 0u;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_dem_namespace_identity_observation_v1 identity;
    bx_ntvdm_instruction_window_v1 window;
    struct bx_ntvdm_mechanical_action_v1 fcb_action;
    wchar_t temporary[MAX_PATH], short_name[MAX_PATH];
    char oem_short[MAX_PATH] = {0}, oem_created[MAX_PATH] = {0};
    char oem_renamed[MAX_PATH] = {0}, oem_dir[MAX_PATH] = {0};
    char oem_wild_one[MAX_PATH] = {0}, oem_wild_two[MAX_PATH] = {0};
    char oem_wild_three[MAX_PATH] = {0};
    char oem_wild_pattern[MAX_PATH] = {0}, oem_rename_one[MAX_PATH] = {0};
    char oem_rename_two[MAX_PATH] = {0}, oem_rename_pattern[MAX_PATH] = {0};
    char oem_rename_destination[MAX_PATH] = {0}, oem_profile_pattern[MAX_PATH] = {0};
    char oem_config[MAX_PATH] = {0}, oem_command[MAX_PATH] = {0};
    char oem_target[MAX_PATH] = {0}, oem_quit[MAX_PATH] = {0};
    uint8_t drive;
    uint32_t service;
    int failed = 0;

    if (GetTempPathW(MAX_PATH, temporary) == 0u ||
        GetTempFileNameW(temporary, L"n64", 0u, temporary) == 0u ||
        GetShortPathNameW(temporary, short_name, MAX_PATH) == 0u ||
        short_name[1] != L':' || WideCharToMultiByte(CP_OEMCP, 0, short_name,
            -1, oem_short, (int)sizeof(oem_short), 0, 0) == 0) return 1;
    file = CreateFileW(temporary, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE |
        FILE_SHARE_DELETE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE || !WriteFile(file, "host", 4u, &written, 0) ||
        written != 4u) { if (file != INVALID_HANDLE_VALUE) CloseHandle(file); return 1; }
    if (!CloseHandle(file)) return 1;
    file = INVALID_HANDLE_VALUE;
    drive = (uint8_t)(towupper((wint_t)short_name[0]) - L'A');
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = UINT32_C(1) << drive;
    snapshot.admitted_mask = snapshot.present_mask;
    snapshot.types[drive] = DRIVE_FIXED;
    if (!profile_for(&profile) || !bx_ntvdm_host_namespace_v1_initialize(&space,
            &snapshot) || !bx_ntvdm_dem_cwd_context_v1_initialize(&cwd,
            &profile) || !bx_ntvdm_dem_whole_provider_v1_initialize(&provider,
            &profile, &space, &cwd)) failed = 10;
    if (!failed) {
        static const uint8_t config_bytes[] = "FILES=20\r\n";
        static const uint8_t autoexec_bytes[] = "@ECHO OFF\r\n";
        static const uint8_t command_bytes[] = "COMMAND";
        static const uint8_t target_bytes[] = "TARGET";
        static const uint8_t quit_bytes[] = "QUIT";
        startup_images.file_count = 5u;
        startup_images.drive_index = drive;
        startup_images.generation = UINT32_C(0x4e534001);
        startup_images.files[0].bytes = command_bytes;
        startup_images.files[0].byte_count = sizeof(command_bytes) - 1u;
        startup_images.files[1].bytes = config_bytes;
        startup_images.files[1].byte_count = sizeof(config_bytes) - 1u;
        startup_images.files[1].dos_time = 0x1234u;
        startup_images.files[1].dos_date = 0x5678u;
        startup_images.files[2].bytes = autoexec_bytes;
        startup_images.files[2].byte_count = sizeof(autoexec_bytes) - 1u;
        startup_images.files[2].dos_time = 0x1234u;
        startup_images.files[2].dos_date = 0x5678u;
        startup_images.files[3].bytes = target_bytes;
        startup_images.files[3].byte_count = sizeof(target_bytes) - 1u;
        startup_images.files[4].bytes = quit_bytes;
        startup_images.files[4].byte_count = sizeof(quit_bytes) - 1u;
        if (wcscpy_s(startup_images.files[0].path,
                BYOB_PROFILE_GUEST_PATH_MAX_CHARS, L"\\COMMAND.COM") != 0 ||
            wcscpy_s(startup_images.files[1].path,
                BYOB_PROFILE_GUEST_PATH_MAX_CHARS, L"\\CONFIG.SYS") != 0 ||
            wcscpy_s(startup_images.files[2].path,
                BYOB_PROFILE_GUEST_PATH_MAX_CHARS, L"\\AUTOEXEC.BAT") != 0 ||
            wcscpy_s(startup_images.files[3].path,
                BYOB_PROFILE_GUEST_PATH_MAX_CHARS, L"\\TARGET.COM") != 0 ||
            wcscpy_s(startup_images.files[4].path,
                BYOB_PROFILE_GUEST_PATH_MAX_CHARS, L"\\QUIT.COM") != 0 ||
            sprintf_s(oem_config, sizeof(oem_config), "%c:\\CONFIG.SYS",
                (char)('A' + drive)) < 0 ||
            sprintf_s(oem_command, sizeof(oem_command), "%c:\\COMMAND.COM",
                (char)('A' + drive)) < 0 ||
            sprintf_s(oem_target, sizeof(oem_target), "%c:\\TARGET.COM",
                (char)('A' + drive)) < 0 ||
            sprintf_s(oem_quit, sizeof(oem_quit), "%c:\\QUIT.COM",
                (char)('A' + drive)) < 0 ||
            !bx_ntvdm_dem_whole_provider_v1_set_startup_namespace(&provider,
                &startup_images)) failed = 11;
    }
    boundary.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    boundary.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    boundary.struct_bytes = sizeof(boundary);
    boundary.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    boundary.cpu_id = 1u;
    boundary.vector = 6u;
    boundary.fault_rip = 0x100u;
    {
        const uint8_t bop[4] = { 0xc4u, 0xc4u, 0x50u, 0x2fu };
        bx_ntvdm_instruction_window_v1_capture(&window, bop, sizeof(bop));
    }
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    if (!failed) {
        uint32_t startup_token;
        const uint8_t bop_open[4] = { 0xc4u, 0xc4u, 0x50u, 0x12u };
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ebx = 0u;
        bx_ntvdm_dem_namespace_identity_observation_v1_enable(1u);
        if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x12u,
                &boundary, &cpu, oem_config, 0, &result) || cf_set(&result) ||
            result.cpu_delta.gpr16_values[2] != sizeof("FILES=20\r\n") - 1u)
            failed = 12;
        startup_token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
            result.cpu_delta.gpr16_values[5];
        if (!failed && (startup_token == 0u ||
            !bx_ntvdm_dem_file_session_v1_token_kind(&provider.files, startup_token,
                &service) || service != BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_READONLY_NAMESPACE))
            failed = 12;
        if (!failed) {
            struct bx_ntvdm_mechanical_action_v1 overlay_action;
            bx_ntvdm_instruction_window_v1_capture(&window, bop_open, sizeof(bop_open));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            token_into_cpu(&cpu, startup_token); cpu.ecx = sizeof("FILES=20\r\n") - 1u;
            cpu.edx = 0x300u; cpu.ds = 0u; cpu.eflags = 0x40u;
            ((uint8_t *)window.bytes)[3] = 0x16u;
            if (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&provider, 0x16u,
                    &boundary, &cpu, &window, &overlay_action, &result) || cf_set(&result) ||
                overlay_action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE ||
                overlay_action.payload_bytes != sizeof("FILES=20\r\n") - 1u ||
                memcmp(overlay_action.payload, "FILES=20\r\n",
                    sizeof("FILES=20\r\n") - 1u) != 0) failed = 13;
            ((uint8_t *)window.bytes)[3] = 0x1eu;
            if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&provider,
                    0x1eu, &boundary, &cpu, &window, &overlay_action, &result) ||
                !cf_set(&result) || !ax_is(&result, 5u))) failed = 14;
            ((uint8_t *)window.bytes)[3] = 0x02u;
            cpu.ecx = cpu.edx = 0xffffu;
            if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&provider,
                    0x02u, &boundary, &cpu, &window, &overlay_action, &result) ||
                cf_set(&result) || startup_images.open != 0u)) failed = 15;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ebx = 0u;
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x12u,
                    &boundary, &cpu, oem_command, 0, &result) || cf_set(&result) ||
                result.cpu_delta.gpr16_values[2] != 7u ||
                !bx_ntvdm_dem_namespace_identity_observation_v1_copy(&identity) ||
                identity.identity_class != BX_NTVDM_DEM_NAMESPACE_IDENTITY_V1_DECLARED ||
                identity.declared_slot != 1u || identity.declared_bytes_ready != 1u ||
                identity.admitted_drive == 0u || identity.wfp_bytes != sizeof("C:\\COMMAND.COM") ||
                identity.wfp_drive_prefix != BX_NTVDM_DEM_NAMESPACE_WFP_DRIVE_C ||
                identity.wfp_root_separator != BX_NTVDM_DEM_NAMESPACE_WFP_ROOT_BACKSLASH ||
                identity.wfp_component_count != 1u)) failed = 16;
            if (!failed) {
                struct bx_ntvdm_mechanical_action_v1 close_action;
                token_into_cpu(&cpu, ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                    result.cpu_delta.gpr16_values[5]);
                if (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&provider, 0x02u,
                        &boundary, &cpu, &window, &close_action, &result) || cf_set(&result)) failed = 17;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL); cpu.ebx = 0u;
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x12u,
                    &boundary, &cpu, oem_target, 0, &result) || cf_set(&result) ||
                result.cpu_delta.gpr16_values[2] != 6u)) failed = 18;
            if (!failed) {
                struct bx_ntvdm_mechanical_action_v1 close_action;
                token_into_cpu(&cpu, ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                    result.cpu_delta.gpr16_values[5]);
                if (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&provider, 0x02u,
                        &boundary, &cpu, &window, &close_action, &result) || cf_set(&result)) failed = 19;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL); cpu.ebx = 0u;
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x12u,
                    &boundary, &cpu, oem_quit, 0, &result) || cf_set(&result) ||
                result.cpu_delta.gpr16_values[2] != 4u)) failed = 20;
            {
                const uint8_t bop_fcb[4] = { 0xc4u, 0xc4u, 0x50u, 0x2fu };
                bx_ntvdm_instruction_window_v1_capture(&window, bop_fcb,
                    sizeof(bop_fcb));
            }
        }
    }
    for (service = 0u; !failed && service < 0x49u; ++service) {
        const int expected = service == 0x00u || service == 0x01u ||
            service == 0x02u || service == 0x03u || service == 0x04u ||
            service == 0x05u || service == 0x06u || service == 0x07u ||
            service == 0x08u || service == 0x09u || service == 0x0au ||
            service == 0x0bu || service == 0x0cu || service == 0x12u ||
            service == 0x16u || service == 0x17u || service == 0x1eu ||
            service == 0x20u || service == 0x22u || service == 0x27u ||
            service == 0x2cu || service == 0x2du || service == 0x2eu ||
            service == 0x2fu || service == 0x30u || service == 0x31u ||
            service == 0x44u || service == 0x47u || service == 0x48u;
        if (bx_ntvdm_dem_whole_provider_v1_owns_service((uint8_t)service) != expected)
            failed = 20;
    }
    if (!failed) {
        static const uint8_t base[] = "base";
        bx_ntvdm_mutation_profile_v1 overlay_profile;
        bx_ntvdm_dem_cwd_context_v1 overlay_cwd;
        bx_ntvdm_dem_whole_provider_v1 *overlay = 0;
        uint32_t backend_token, overlay_token;
        wchar_t overlay_relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
        uint8_t overlay_drive = 0u;
        DWORD overlay_error = ERROR_INVALID_PARAMETER;
        int overlay_empty = 0;
        uint16_t overlay_time = 0u, overlay_date = 0u;
        bx_ntvdm_dem_overlay_namespace_node_v1 overlay_node;
        if (!profile_for_mode(&overlay_profile, BX_NTVDM_MUTATION_MODE_V1_OVERLAY) ||
            !bx_ntvdm_dem_cwd_context_v1_initialize(&overlay_cwd, &overlay_profile) ||
            (overlay = (bx_ntvdm_dem_whole_provider_v1 *)HeapAlloc(GetProcessHeap(),
                HEAP_ZERO_MEMORY, sizeof(*overlay))) == 0 ||
                !bx_ntvdm_dem_whole_provider_v1_initialize(overlay, &overlay_profile,
                &space, &overlay_cwd) ||
            bx_ntvdm_dem_path_v1_resolve(oem_short, &overlay_cwd, &overlay_drive,
                overlay_relative) != BX_NTVDM_DEM_PATH_V1_OK ||
            !bx_ntvdm_dem_fcb_overlay_backend_v1_open(&overlay->files,
                &overlay->overlay_files, &space, overlay_drive, overlay_relative,
                BX_NTVDM_DEM_OVERLAY_FILE_V1_READ, 3u, OPEN_EXISTING, 0u,
                &overlay_token, &backend_token, &overlay_time,
                &overlay_date, &overlay_error) ||
            !bx_ntvdm_dem_overlay_handle_backend_v1_close(&overlay->files,
                &overlay->overlay_files, overlay_token) ||
            !bx_ntvdm_dem_overlay_file_v1_open(&overlay->overlay_files, drive,
                L"overlay.bin", BX_NTVDM_DEM_OVERLAY_FILE_V1_READ |
                BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE, base, sizeof(base) - 1u,
                FILE_ATTRIBUTE_NORMAL, 1, 0, &backend_token) ||
            !bx_ntvdm_dem_file_session_v1_adopt_backend(&overlay->files,
                BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE, backend_token,
                0u, &overlay_token)) failed = 21;
        else {
            const uint8_t handle_bop[4] = { 0xc4u, 0xc4u, 0x50u, 0x00u };
            bx_ntvdm_instruction_window_v1_capture(&window, handle_bop, sizeof(handle_bop));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            token_into_cpu(&cpu, overlay_token);
            cpu.ebx = cpu.ecx = cpu.edx = cpu.esi = cpu.eflags = 0u;
            if (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(overlay, 0x00u,
                    &boundary, &cpu, &window, &fcb_action, &result) || cf_set(&result) ||
                !ax_is(&result, 0u)) failed = 211;
            ((uint8_t *)window.bytes)[3] = 0x1eu;
            token_into_cpu(&cpu, overlay_token); cpu.ecx = 3u; cpu.edx = 0x500u;
            cpu.ds = 0u; cpu.eflags = 0x40u;
            if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(overlay, 0x1eu,
                    &boundary, &cpu, &window, &fcb_action, &result) ||
                fcb_action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ ||
                fcb_action.payload_bytes != 3u)) failed = 212;
            if (!failed) {
                memcpy(fcb_action.payload, "COW", 3u);
                if (!bx_ntvdm_dem_handle_route_partition_v1_complete_read(overlay, 0x1eu,
                        &boundary, &cpu, &fcb_action, &result) || cf_set(&result) ||
                    !ax_is(&result, 3u)) failed = 213;
            }
            ((uint8_t *)window.bytes)[3] = 0x00u;
            token_into_cpu(&cpu, overlay_token);
            cpu.ebx = cpu.ecx = cpu.edx = cpu.esi = cpu.eflags = 0u;
            if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(overlay, 0x00u,
                    &boundary, &cpu, &window, &fcb_action, &result) || cf_set(&result))) failed = 214;
            ((uint8_t *)window.bytes)[3] = 0x16u;
            token_into_cpu(&cpu, overlay_token); cpu.ecx = 4u; cpu.edx = 0x600u;
            cpu.ds = 0u; cpu.eflags = 0x40u;
            if (!failed && !bx_ntvdm_dem_handle_route_partition_v1_dispatch(overlay, 0x16u,
                    &boundary, &cpu, &window, &fcb_action, &result)) failed = 215;
            else if (!failed && cf_set(&result)) failed = 216;
            else if (!failed && fcb_action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE) failed = 217;
            else if (!failed && fcb_action.payload_bytes != 4u) failed = 218;
            else if (!failed && memcmp(fcb_action.payload, "COWe", 4u) != 0) failed = 219;
            ((uint8_t *)window.bytes)[3] = 0x08u;
            token_into_cpu(&cpu, overlay_token);
            if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(overlay, 0x08u,
                    &boundary, &cpu, &window, &fcb_action, &result) || cf_set(&result) ||
                (result.cpu_delta.gpr16_write_mask & ((1u << 1u) | (1u << 2u))) !=
                    ((1u << 1u) | (1u << 2u)))) failed = 220;
            ((uint8_t *)window.bytes)[3] = 0x27u;
            token_into_cpu(&cpu, overlay_token);
            if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(overlay, 0x27u,
                    &boundary, &cpu, &window, &fcb_action, &result) || cf_set(&result))) failed = 221;
            ((uint8_t *)window.bytes)[3] = 0x02u;
            token_into_cpu(&cpu, overlay_token); cpu.ecx = cpu.edx = 0xffffu;
            if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(overlay, 0x02u,
                    &boundary, &cpu, &window, &fcb_action, &result) || cf_set(&result) ||
                bx_ntvdm_dem_file_session_v1_token_kind(&overlay->files, overlay_token,
                    &service))) failed = 222;
        }
        if (overlay != 0) {
            uint32_t opened, share_token;
            overlay->direct_namespace_owner = 0x1234u;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ebx = 0u;
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(overlay, 0x12u,
                    &boundary, &cpu, oem_short, 0, &result) || cf_set(&result))) failed = 223;
            opened = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                result.cpu_delta.gpr16_values[5];
            if (!failed && (!bx_ntvdm_dem_file_session_v1_token_kind(&overlay->files,
                    opened, &service) || service != BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE))
                failed = 224;
            if (!failed) {
                const uint8_t close_bop[4] = { 0xc4u, 0xc4u, 0x50u, 0x02u };
                bx_ntvdm_instruction_window_v1_capture(&window, close_bop, sizeof(close_bop));
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                token_into_cpu(&cpu, opened); cpu.ecx = cpu.edx = 0xffffu;
                if (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(overlay, 0x02u,
                        &boundary, &cpu, &window, &fcb_action, &result) || cf_set(&result)) failed = 225;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ebx = 0x20u;
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(overlay, 0x12u,
                    &boundary, &cpu, oem_short, 0, &result) || cf_set(&result))) failed = 229;
            share_token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                result.cpu_delta.gpr16_values[5];
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(overlay, 0x12u,
                    &boundary, &cpu, oem_short, 0, &result) || !cf_set(&result) ||
                !ax_is(&result, ERROR_SHARING_VIOLATION))) failed = 230;
            {
                const uint8_t close_bop[4] = { 0xc4u, 0xc4u, 0x50u, 0x02u };
                bx_ntvdm_instruction_window_v1_capture(&window, close_bop, sizeof(close_bop));
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                token_into_cpu(&cpu, share_token); cpu.ecx = cpu.edx = 0xffffu;
                if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(overlay, 0x02u,
                        &boundary, &cpu, &window, &fcb_action, &result) || cf_set(&result))) failed = 231;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ecx = FILE_ATTRIBUTE_HIDDEN;
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(overlay, 0x03u,
                    &boundary, &cpu, oem_short, 0, &result) || cf_set(&result))) failed = 226;
            opened = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                result.cpu_delta.gpr16_values[5];
            if (!failed && (!bx_ntvdm_dem_file_session_v1_token_kind(&overlay->files,
                    opened, &service) || service != BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE))
                failed = 227;
            if (!failed && (bx_ntvdm_dem_path_v1_resolve(oem_short, &overlay_cwd,
                    &overlay_drive, overlay_relative) != BX_NTVDM_DEM_PATH_V1_OK ||
                bx_ntvdm_dem_overlay_store_v1_lookup(&overlay->overlay_store, overlay_drive,
                    overlay_relative) == 0 || bx_ntvdm_dem_overlay_store_v1_lookup(
                    &overlay->overlay_store, overlay_drive, overlay_relative)->attributes !=
                    FILE_ATTRIBUTE_HIDDEN)) failed = 233;
            if (!failed && (!bx_ntvdm_dem_overlay_namespace_view_v1_query(
                    &overlay->overlay_store, &space, overlay_drive, overlay_relative,
                    &overlay_node, &overlay_error) ||
                overlay_node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE)) failed = 234;
            if (!failed && !bx_ntvdm_dem_overlay_store_v1_put_directory(
                    &overlay->overlay_store, overlay_drive, L"N64OVD99", 0u)) failed = 235;
            if (!failed && !bx_ntvdm_dem_overlay_store_v1_add_relocation(
                    &overlay->overlay_store, overlay_drive, L"N64MVD99", L"N64OVD99")) failed = 236;
            if (!failed && (!bx_ntvdm_dem_overlay_namespace_view_v1_query(&overlay->overlay_store,
                    &space, overlay_drive, L"N64MVD99", &overlay_node, &overlay_error) ||
                overlay_node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY)) failed = 237;
            if (!failed && (!bx_ntvdm_dem_overlay_namespace_view_v1_query(&overlay->overlay_store,
                    &space, overlay_drive, L"N64OVD99", &overlay_node, &overlay_error) ||
                overlay_node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT)) failed = 238;
            if (!failed && !bx_ntvdm_dem_overlay_namespace_view_v1_directory_empty(
                    &overlay->overlay_store, &space, overlay_drive, L"N64MVD99",
                    &overlay_empty, &overlay_error)) failed = 239;
            if (!failed && !overlay_empty) failed = 242;
            if (!failed && !bx_ntvdm_dem_overlay_store_v1_put_file(&overlay->overlay_store,
                    overlay_drive, L"N64MVD99\\CHILD.TXT", 0u, 0, 0u)) failed = 240;
            if (!failed && (!bx_ntvdm_dem_overlay_namespace_view_v1_directory_empty(
                    &overlay->overlay_store, &space, overlay_drive, L"N64MVD99",
                    &overlay_empty, &overlay_error) || overlay_empty)) failed = 241;
            {
                bx_ntvdm_host_namespace_entry_v1 overlay_entries[4];
                uint32_t overlay_entry_count = 0u;
                if (!failed && (!bx_ntvdm_dem_overlay_namespace_view_v1_enumerate(
                        &overlay->overlay_store, &space, overlay_drive, L"N64MVD99",
                        overlay_entries, 4u, &overlay_entry_count, &overlay_error) ||
                    overlay_error != ERROR_SUCCESS || overlay_entry_count != 1u ||
                    _wcsicmp(overlay_entries[0].dos_name, L"CHILD.TXT") != 0)) failed = 243;
            }
            if (!failed && (!bx_ntvdm_dem_overlay_mutation_backend_v1_create_directory(
                    &overlay->overlay_store, &space, overlay_drive, L"N64MUTD",
                    &overlay_error) || overlay_error != ERROR_SUCCESS ||
                !bx_ntvdm_dem_overlay_store_v1_put_file(&overlay->overlay_store,
                    overlay_drive, L"N64MUTD\\CHILD.TXT", 0u, 0, 0u) ||
                !bx_ntvdm_dem_overlay_mutation_backend_v1_remove_directory(
                    &overlay->overlay_store, &space, overlay_drive, L"N64MUTD",
                    &overlay_error) || overlay_error != ERROR_DIR_NOT_EMPTY)) failed = 250;
            if (!failed && (!bx_ntvdm_dem_overlay_mutation_backend_v1_delete_file(
                    &overlay->overlay_store, &space, overlay_drive, L"N64MUTD\\CHILD.TXT",
                    &overlay_error) || overlay_error != ERROR_SUCCESS ||
                !bx_ntvdm_dem_overlay_mutation_backend_v1_remove_directory(
                    &overlay->overlay_store, &space, overlay_drive, L"N64MUTD",
                    &overlay_error) || overlay_error != ERROR_SUCCESS)) failed = 251;
            if (!failed && (!bx_ntvdm_dem_overlay_store_v1_put_file(&overlay->overlay_store,
                    overlay_drive, L"N64MUTA.TXT", 0u, (const uint8_t *)"cow", 3u) ||
                !bx_ntvdm_dem_overlay_mutation_backend_v1_rename(&overlay->overlay_store,
                    &overlay->overlay_files, &space, overlay_drive, L"N64MUTA.TXT",
                    overlay_drive, L"N64MUTB.TXT", &overlay_error) ||
                overlay_error != ERROR_SUCCESS ||
                bx_ntvdm_dem_overlay_store_v1_lookup(&overlay->overlay_store, overlay_drive,
                    L"N64MUTB.TXT") == 0 || bx_ntvdm_dem_overlay_store_v1_lookup(
                    &overlay->overlay_store, overlay_drive, L"N64MUTB.TXT")->byte_count != 3u)) failed = 252;
            if (!failed && (!bx_ntvdm_dem_overlay_mutation_backend_v1_create_directory(
                    &overlay->overlay_store, &space, overlay_drive, L"N64MUTR",
                    &overlay_error) || overlay_error != ERROR_SUCCESS ||
                !bx_ntvdm_dem_overlay_mutation_backend_v1_rename(&overlay->overlay_store,
                    &overlay->overlay_files, &space, overlay_drive, L"N64MUTR",
                    overlay_drive, L"N64MUTS", &overlay_error) || overlay_error != ERROR_SUCCESS ||
                !bx_ntvdm_dem_overlay_namespace_view_v1_query(&overlay->overlay_store, &space,
                    overlay_drive, L"N64MUTS", &overlay_node, &overlay_error) ||
                overlay_node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY ||
                !bx_ntvdm_dem_overlay_namespace_view_v1_query(&overlay->overlay_store, &space,
                    overlay_drive, L"N64MUTR", &overlay_node, &overlay_error) ||
                overlay_node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT)) failed = 253;
            file = CreateFileW(temporary, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
            if (!failed && (file == INVALID_HANDLE_VALUE || !ReadFile(file, output, 4u, &written, 0) ||
                written != 4u || memcmp(output, "host", 4u) != 0)) failed = 232;
            if (file != INVALID_HANDLE_VALUE) { CloseHandle(file); file = INVALID_HANDLE_VALUE; }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(overlay, 0x22u,
                    &boundary, &cpu, oem_short, 0, &result) || !cf_set(&result) ||
                !ax_is(&result, ERROR_FILE_EXISTS))) failed = 228;
            bx_ntvdm_dem_whole_provider_v1_teardown(overlay);
            HeapFree(GetProcessHeap(), 0, overlay);
        }
        {
            const uint8_t fcb_bop[4] = { 0xc4u, 0xc4u, 0x50u, 0x2fu };
            bx_ntvdm_instruction_window_v1_capture(&window, fcb_bop, sizeof(fcb_bop));
        }
    }
    if (!failed && (!bx_ntvdm_dem_whole_provider_v1_prepare_gather(&provider,
            0x12u, &boundary, &cpu, &range, 1u, &action) ||
        action.total_bytes != 3u ||
        bx_ntvdm_dem_whole_provider_v1_prepare_gather(&provider, 0x12u,
            &boundary, &cpu, &range, 1u, &action) != 0 ||
        !bx_ntvdm_dem_whole_provider_v1_complete_gather(&provider, 0x12u,
            &boundary, &cpu, &action, input, sizeof(input), output,
            &output_bytes) || output_bytes != sizeof(input) ||
        memcmp(input, output, sizeof(input)) != 0 ||
        bx_ntvdm_dem_whole_provider_v1_complete_gather(&provider, 0x12u,
            &boundary, &cpu, &action, input, sizeof(input), output,
            &output_bytes) != 0)) failed = 30;
    if (!failed && (!bx_ntvdm_dem_whole_provider_v1_prepare_gather(&provider,
            0x09u, &boundary, &cpu, &range, 1u, &action) ||
        !bx_ntvdm_dem_whole_provider_v1_cancel_gather(&provider, 0x09u,
            &boundary, &cpu, &action) ||
        !bx_ntvdm_dem_whole_provider_v1_prepare_gather(&provider, 0x09u,
            &boundary, &cpu, &range, 1u, &action) ||
        !bx_ntvdm_dem_whole_provider_v1_cancel_gather(&provider, 0x09u,
            &boundary, &cpu, &action))) failed = 31;
    if (!failed) {
        file = CreateFileW(temporary, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (file == INVALID_HANDLE_VALUE ||
            !WriteFile(file, "abc", 3u, &written, 0) || written != 3u ||
            !SetFilePointerEx(file, (LARGE_INTEGER){0}, 0, FILE_BEGIN) ||
            !bx_ntvdm_dem_file_session_v1_adopt(&provider.files, file, &token)) {
            if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
            failed = 40;
        }
    }
    if (!failed && !replace_extension(oem_profile_pattern, oem_short, ".N?")) failed = 76;
    if (!failed) {
        char cross_drive[MAX_PATH] = {0};
        sprintf_s(cross_drive, sizeof(cross_drive), "%c:\\N?.TMP",
            (char)('A' + ((drive + 1u) % 26u)));
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!failed && (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&provider, 0x20u,
                &boundary, &cpu, oem_profile_pattern, cross_drive, &result) ||
            !cf_set(&result) || !ax_is(&result, 17u))) failed = 761;
    }
    if (!failed) {
        wchar_t hidden[MAX_PATH];
        wchar_t readonly[MAX_PATH];
        if (!replace_extension(oem_wild_one, oem_short, ".W1") ||
            !replace_extension(oem_wild_two, oem_short, ".W2") ||
            !replace_extension(oem_wild_three, oem_short, ".W3") ||
            !replace_extension(oem_wild_pattern, oem_short, ".W?") ||
            !create_oem_file(oem_wild_one) || !create_oem_file(oem_wild_two) ||
            !create_oem_file(oem_wild_three) ||
            !oem_to_wide(oem_wild_two, hidden) ||
            !SetFileAttributesW(hidden, FILE_ATTRIBUTE_HIDDEN) ||
            !oem_to_wide(oem_wild_three, readonly) ||
            !SetFileAttributesW(readonly, FILE_ATTRIBUTE_READONLY)) failed = 71;
        if (!failed) {
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 0u;
            if (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&provider, 0x07u,
                    &boundary, &cpu, oem_wild_pattern, 0, &result) || cf_set(&result) ||
                oem_file_exists(oem_wild_one) || !oem_file_exists(oem_wild_two) ||
                !oem_file_exists(oem_wild_three)) failed = 72;
        }
        if (!failed) {
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 1u; cpu.edx = FILE_ATTRIBUTE_READONLY;
            if (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&provider, 0x07u,
                    &boundary, &cpu, oem_wild_pattern, 0, &result) || cf_set(&result) ||
                oem_file_exists(oem_wild_three) || !oem_file_exists(oem_wild_two)) failed = 721;
        }
        if (!failed) {
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 1u; cpu.edx = FILE_ATTRIBUTE_HIDDEN;
            if (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&provider, 0x07u,
                    &boundary, &cpu, oem_wild_pattern, 0, &result) || cf_set(&result) ||
                oem_file_exists(oem_wild_two)) failed = 73;
        }
    }
    if (!failed) {
        if (!replace_extension(oem_rename_one, oem_short, ".R1") ||
            !replace_extension(oem_rename_two, oem_short, ".R2") ||
            !replace_extension(oem_rename_pattern, oem_short, ".R?") ||
            !replace_extension(oem_rename_destination, oem_short, ".N?") ||
            !create_oem_file(oem_rename_one) || !create_oem_file(oem_rename_two)) failed = 74;
        if (!failed) {
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&provider, 0x20u,
                    &boundary, &cpu, oem_rename_pattern, oem_rename_destination, &result) ||
                cf_set(&result) || oem_file_exists(oem_rename_one) ||
                oem_file_exists(oem_rename_two) ||
                !replace_extension(oem_rename_one, oem_short, ".N1") ||
                !replace_extension(oem_rename_two, oem_short, ".N2") ||
                !oem_file_exists(oem_rename_one) || !oem_file_exists(oem_rename_two)) failed = 75;
        }
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider, 0x2du,
                &boundary, &cpu, oem_short, 0, 0u, &output_bytes, &result) ||
            cf_set(&result) || (result.cpu_delta.gpr16_write_mask &
                ((1u << 0u) | (1u << 1u) | (1u << 2u) | (1u << 3u) |
                 (1u << 5u) | (1u << 6u))) !=
                ((1u << 0u) | (1u << 1u) | (1u << 2u) | (1u << 3u) |
                 (1u << 5u) | (1u << 6u))) failed = 66;
        else {
            fcb_token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                result.cpu_delta.gpr16_values[5];
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            token_into_cpu(&cpu, fcb_token); cpu.ebx = 1u; cpu.ecx = 2u;
            if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider, 0x2fu,
                &boundary, &cpu, 0, output, sizeof(output), &output_bytes, &result) ||
                cf_set(&result) || output_bytes != 2u ||
                result.cpu_delta.gpr16_values[1] != 2u) failed = 67;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = fcb_token >> 16; cpu.esi = fcb_token & 0xffffu;
            if (!failed && (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider,
                    0x2eu, &boundary, &cpu, 0, 0, 0u, &output_bytes, &result) ||
                cf_set(&result))) failed = 68;
        }
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!failed && (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider,
                0x30u, &boundary, &cpu, 0, 0, 0u, &output_bytes, &result) ||
            cf_set(&result) || (result.cpu_delta.gpr16_write_mask & 5u) != 5u)) failed = 69;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.eax = 2u; /* demOpenFCB read/write mode for the write/read route. */
        if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider, 0x2du,
                &boundary, &cpu, oem_short, 0, 0u, &output_bytes, &result) || cf_set(&result)) failed = 691;
        else {
            fcb_token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                result.cpu_delta.gpr16_values[5];
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = fcb_token >> 16; cpu.ebp = fcb_token & 0xffffu;
            cpu.ecx = 2u; cpu.ebx = 0u;
            if (!bx_ntvdm_dem_fcb_io_route_partition_v1_dispatch(&provider, 0x2fu,
                    &boundary, &cpu, &window, 0x300u, &fcb_action, &result) ||
                fcb_action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ ||
                fcb_action.ranges[0].physical_address != 0x300u) failed = 692;
            else {
                memcpy(fcb_action.payload, "ab", 2u);
                if (!bx_ntvdm_dem_fcb_io_route_partition_v1_complete_write(&provider,
                        0x2fu, &boundary, &cpu, &fcb_action, &result) || cf_set(&result) ||
                    result.cpu_delta.gpr16_values[1] != 2u) failed = 693;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = fcb_token >> 16; cpu.ebp = fcb_token & 0xffffu;
            cpu.ecx = 2u; cpu.ebx = 1u;
            if (!failed && (!bx_ntvdm_dem_fcb_io_route_partition_v1_dispatch(&provider,
                    0x2fu, &boundary, &cpu, &window, 0x300u, &fcb_action, &result) ||
                fcb_action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE ||
                fcb_action.payload_bytes != 2u || memcmp(fcb_action.payload, "ab", 2u) != 0 ||
                cf_set(&result) || result.cpu_delta.gpr16_values[1] != 2u)) failed = 694;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = fcb_token >> 16; cpu.esi = fcb_token & 0xffffu;
            if (!failed && (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider,
                    0x2eu, &boundary, &cpu, 0, 0, 0u, &output_bytes, &result) ||
                cf_set(&result))) failed = 695;
        }
    }
    if (!failed) {
        file = INVALID_HANDLE_VALUE; /* provider owns the adopted handle. */
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        token_into_cpu(&cpu, token);
        cpu.ecx = 0u;
        cpu.edx = 1u;
        if (!bx_ntvdm_dem_handle_partition_v1_dispatch(&provider, 0x00u,
                &boundary, &cpu, 0, 0u, 0, &result) || cf_set(&result) ||
            !ax_is(&result, 1u) || result.cpu_delta.gpr16_values[2] != 0u)
            failed = 41;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        token_into_cpu(&cpu, token);
        cpu.ecx = 2u;
        cpu.eflags = 0x40u;
        if (!bx_ntvdm_dem_handle_partition_v1_dispatch(&provider, 0x16u,
                &boundary, &cpu, output, sizeof(output), &output_bytes, &result) ||
            cf_set(&result) || !ax_is(&result, 2u) || output_bytes != 2u ||
            output[0] != 'b' || output[1] != 'c') failed = 42;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        token_into_cpu(&cpu, token);
        cpu.ecx = 1u;
        cpu.eflags = 0x40u;
        input[0] = 'Z';
        if (!bx_ntvdm_dem_handle_partition_v1_dispatch(&provider, 0x1eu,
                &boundary, &cpu, input, 1u, &output_bytes, &result) ||
            cf_set(&result) || !ax_is(&result, 1u) || output_bytes != 1u)
            failed = 43;
    }
    if (!failed) {
        HANDLE observed = INVALID_HANDLE_VALUE;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        token_into_cpu(&cpu, token);
        cpu.ecx = 0u;
        cpu.eflags = 0u;
        cpu.ebx = 0u;
        cpu.esi = 2u;
        if (!bx_ntvdm_dem_handle_partition_v1_dispatch(&provider, 0x1eu,
                &boundary, &cpu, input, 0u, &output_bytes, &result) ||
            cf_set(&result) ||
            !bx_ntvdm_dem_file_session_v1_lookup(&provider.files, token, &observed) ||
            GetFileSize(observed, 0) != 2u) failed = 44;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        token_into_cpu(&cpu, token);
        if (!bx_ntvdm_dem_handle_partition_v1_dispatch(&provider, 0x08u,
                &boundary, &cpu, 0, 0u, 0, &result) || cf_set(&result) ||
            (result.cpu_delta.gpr16_write_mask & 6u) != 6u) failed = 45;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ebx = 2u;
        if (!bx_ntvdm_dem_handle_partition_v1_dispatch(&provider, 0x08u,
                &boundary, &cpu, 0, 0u, 0, &result) || cf_set(&result) ||
            (result.cpu_delta.gpr16_write_mask & 6u) != 6u) failed = 46;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        token_into_cpu(&cpu, token);
        if (!bx_ntvdm_dem_handle_partition_v1_dispatch(&provider, 0x27u,
                &boundary, &cpu, 0, 0u, 0, &result) || cf_set(&result)) failed = 47;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        token_into_cpu(&cpu, token);
        cpu.ecx = cpu.edx = 0xffffu;
        if (!bx_ntvdm_dem_handle_partition_v1_dispatch(&provider, 0x02u,
                &boundary, &cpu, 0, 0u, 0, &result) || cf_set(&result) ||
            bx_ntvdm_dem_file_session_v1_lookup(&provider.files, token, &file))
            failed = 48;
    }
    if (!failed) {
        const uint8_t namespace_bop[4] = { 0xc4u, 0xc4u, 0x50u, 0x01u };
        bx_ntvdm_instruction_window_v1_capture(&window, namespace_bop, sizeof(namespace_bop));
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ds = 0x20u; cpu.edx = 0x20u;
        if (!bx_ntvdm_dem_namespace_route_partition_v1_prepare(&provider, 0x01u,
                &boundary, &cpu, &window, &fcb_action) ||
            fcb_action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ ||
            fcb_action.payload_bytes != BX_NTVDM_DEM_PATH_V1_MAX_OEM_BYTES ||
            fcb_action.ranges[0].physical_address != 0x220u) failed = 481;
        else {
            memset(fcb_action.payload, 0, fcb_action.payload_bytes);
            if (strcpy_s((char *)fcb_action.payload, fcb_action.payload_bytes, oem_short) != 0 ||
                !bx_ntvdm_dem_namespace_route_partition_v1_complete(&provider, 0x01u,
                    &boundary, &cpu, &fcb_action, &result) || cf_set(&result) ||
                (result.cpu_delta.gpr16_write_mask & (1u << 2u)) == 0u) failed = 482;
        }
    }
    if (!failed) {
        static const uint8_t services[] = { 0x01u, 0x03u, 0x04u, 0x05u,
            0x06u, 0x12u, 0x17u, 0x22u, 0x44u };
        uint32_t index;
        for (index = 0u; index < sizeof(services); ++index) {
            uint8_t bop[4] = { 0xc4u, 0xc4u, 0x50u, services[index] };
            uint64_t first = (services[index] == 0x03u || services[index] == 0x12u ||
                services[index] == 0x22u || services[index] == 0x44u) ? 0x230u : 0x220u;
            bx_ntvdm_instruction_window_v1_capture(&window, bop, sizeof(bop));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0x20u; cpu.es = 0x30u; cpu.edx = 0x20u; cpu.esi = 0x30u; cpu.edi = 0x40u;
            if (!bx_ntvdm_dem_namespace_route_partition_v1_prepare(&provider, services[index],
                    &boundary, &cpu, &window, &fcb_action) ||
                fcb_action.range_count != (services[index] == 0x17u ? 2u : 1u) ||
                fcb_action.ranges[0].physical_address != first ||
                (services[index] == 0x17u &&
                    fcb_action.ranges[1].physical_address != 0x340u) ||
                !bx_ntvdm_dem_whole_provider_v1_cancel_gather(&provider, services[index],
                    &boundary, &cpu, &provider.pending_gather)) { failed = 483; break; }
        }
    }
    if (!failed) {
        static const uint8_t services[] = { 0x07u, 0x20u, 0x2cu, 0x2du, 0x31u };
        uint32_t index;
        for (index = 0u; index < sizeof(services); ++index) {
            uint8_t bop[4] = { 0xc4u, 0xc4u, 0x50u, services[index] };
            uint64_t first = services[index] == 0x07u ? 0x340u : 0x230u;
            bx_ntvdm_instruction_window_v1_capture(&window, bop, sizeof(bop));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0x20u; cpu.es = 0x30u; cpu.esi = 0x30u; cpu.edi = 0x40u;
            if (!bx_ntvdm_dem_fcb_path_route_partition_v1_prepare(&provider, services[index],
                    &boundary, &cpu, &window, &fcb_action) ||
                fcb_action.range_count != (services[index] == 0x20u ? 2u : 1u) ||
                fcb_action.ranges[0].physical_address != first ||
                (services[index] == 0x20u &&
                    fcb_action.ranges[1].physical_address != 0x340u) ||
                !bx_ntvdm_dem_whole_provider_v1_cancel_gather(&provider, services[index],
                    &boundary, &cpu, &provider.pending_gather)) { failed = 486; break; }
        }
    }
    if (!failed) {
        const uint8_t fcb_bop[4] = { 0xc4u, 0xc4u, 0x50u, 0x2du };
        bx_ntvdm_instruction_window_v1_capture(&window, fcb_bop, sizeof(fcb_bop));
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ds = 0x20u; cpu.esi = 0x30u;
        if (!bx_ntvdm_dem_fcb_path_route_partition_v1_prepare(&provider, 0x2du,
                &boundary, &cpu, &window, &fcb_action)) failed = 487;
        else {
            memset(fcb_action.payload, 0, fcb_action.payload_bytes);
            if (strcpy_s((char *)fcb_action.payload, fcb_action.payload_bytes, oem_short) != 0 ||
                !bx_ntvdm_dem_fcb_path_route_partition_v1_complete(&provider, 0x2du,
                    &boundary, &cpu, &fcb_action, &result) || cf_set(&result) ||
                (result.cpu_delta.gpr16_write_mask & ((1u << 0u) | (1u << 5u))) !=
                    ((1u << 0u) | (1u << 5u))) failed = 488;
            else {
                uint32_t fcb_path_token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                    result.cpu_delta.gpr16_values[5];
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                cpu.eax = fcb_path_token >> 16; cpu.esi = fcb_path_token & 0xffffu;
                if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider, 0x2eu,
                        &boundary, &cpu, 0, 0, 0u, &output_bytes, &result) || cf_set(&result)) failed = 489;
            }
        }
    }
    if (!failed) {
        const uint8_t fcb_bop[4] = { 0xc4u, 0xc4u, 0x50u, 0x31u };
        bx_ntvdm_instruction_window_v1_capture(&window, fcb_bop, sizeof(fcb_bop));
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ds = 0x20u; cpu.esi = 0x30u;
        if (!bx_ntvdm_dem_fcb_path_route_partition_v1_prepare(&provider, 0x31u,
                &boundary, &cpu, &window, &fcb_action)) failed = 490;
        else {
            memset(fcb_action.payload, 'x', fcb_action.payload_bytes);
            if (bx_ntvdm_dem_fcb_path_route_partition_v1_complete(&provider, 0x31u,
                    &boundary, &cpu, &fcb_action, &result) ||
                provider.pending_action_id != 0u) failed = 491;
        }
    }
    if (!failed) {
        const uint8_t namespace_bop[4] = { 0xc4u, 0xc4u, 0x50u, 0x03u };
        bx_ntvdm_instruction_window_v1_capture(&window, namespace_bop, sizeof(namespace_bop));
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ds = 0x20u; cpu.esi = 0x30u;
        if (!bx_ntvdm_dem_namespace_route_partition_v1_prepare(&provider, 0x03u,
                &boundary, &cpu, &window, &fcb_action)) failed = 484;
        else {
            memset(fcb_action.payload, 'x', fcb_action.payload_bytes);
            if (bx_ntvdm_dem_namespace_route_partition_v1_complete(&provider, 0x03u,
                    &boundary, &cpu, &fcb_action, &result) ||
                provider.pending_action_id != 0u) failed = 485;
        }
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider,
                0x31u, &boundary, &cpu, oem_short, 0, 0u, &output_bytes, &result) ||
            cf_set(&result) || (result.cpu_delta.gpr16_write_mask & 0x8fu) != 0x8fu) failed = 70;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x01u,
                &boundary, &cpu, oem_short, 0, &result) || cf_set(&result) ||
            (result.cpu_delta.gpr16_write_mask & (1u << 2u)) == 0u) failed = 49;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        /* The direct call below bypasses package_session's copied CurrentPDB
         * transaction, so supply its already-validated test owner explicitly. */
        provider.direct_namespace_owner = 0x1234u;
        cpu.ebx = 0u; /* demOpen: read, compatibility sharing */
        if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x12u,
                &boundary, &cpu, oem_short, 0, &result) || cf_set(&result) ||
            (result.cpu_delta.gpr16_write_mask & ((1u << 0u) | (1u << 1u) |
                (1u << 2u) | (1u << 3u) | (1u << 5u))) !=
                ((1u << 0u) | (1u << 1u) | (1u << 2u) | (1u << 3u) |
                 (1u << 5u))) failed = 50;
        else {
            token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                result.cpu_delta.gpr16_values[5];
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            token_into_cpu(&cpu, token);
            cpu.ecx = cpu.edx = 0xffffu;
            if (!bx_ntvdm_dem_handle_partition_v1_dispatch(&provider, 0x02u,
                    &boundary, &cpu, 0, 0u, 0, &result) || cf_set(&result)) failed = 51;
        }
    }
    if (!failed) {
        char *dot;
        strcpy_s(oem_created, sizeof(oem_created), oem_short);
        dot = strrchr(oem_created, '.');
        if (dot == 0) failed = 52;
        else strcpy_s(dot, sizeof(oem_created) - (size_t)(dot - oem_created), ".N01");
        strcpy_s(oem_renamed, sizeof(oem_renamed), oem_created);
        dot = strrchr(oem_renamed, '.');
        if (!failed) strcpy_s(dot, sizeof(oem_renamed) - (size_t)(dot - oem_renamed), ".N02");
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x03u,
                &boundary, &cpu, oem_created, 0, &result) || cf_set(&result))) failed = 53;
        if (!failed) {
            token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                result.cpu_delta.gpr16_values[5];
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            token_into_cpu(&cpu, token); cpu.ecx = cpu.edx = 0xffffu;
            if (!bx_ntvdm_dem_handle_partition_v1_dispatch(&provider, 0x02u,
                    &boundary, &cpu, 0, 0u, 0, &result) || cf_set(&result)) failed = 54;
        }
        if (!failed) {
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x17u,
                    &boundary, &cpu, oem_created, oem_renamed, &result) || cf_set(&result)) failed = 55;
        }
        if (!failed) {
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x05u,
                    &boundary, &cpu, oem_renamed, 0, &result) || cf_set(&result)) failed = 56;
        }
    }
    if (!failed) {
        char *dot;
        strcpy_s(oem_dir, sizeof(oem_dir), oem_short);
        dot = strrchr(oem_dir, '.');
        if (dot == 0) failed = 57;
        else strcpy_s(dot, sizeof(oem_dir) - (size_t)(dot - oem_dir), ".D01");
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x04u,
                &boundary, &cpu, oem_dir, 0, &result) || cf_set(&result))) failed = 58;
        if (!failed) {
            char check_path[MAX_PATH];
            strcpy_s(check_path, sizeof(check_path), oem_dir + 2);
            strcat_s(check_path, sizeof(check_path), "\\");
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.edx = (uint32_t)drive + 1u;
            if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x44u,
                    &boundary, &cpu, check_path, 0, &result) || cf_set(&result) ||
                (result.cpu_delta.gpr16_write_mask & (1u << 3u)) == 0u) failed = 59;
        }
        if (!failed) {
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x06u,
                    &boundary, &cpu, oem_dir, 0, &result) || cf_set(&result)) failed = 60;
        }
    }
    if (!failed) {
        bx_ntvdm_mutation_profile_v1 alternate_profile;
        bx_ntvdm_dem_cwd_context_v1 alternate_cwd;
        bx_ntvdm_dem_whole_provider_v1 alternate;
        if (!profile_for_mode(&alternate_profile, BX_NTVDM_MUTATION_MODE_V1_READONLY) ||
            !bx_ntvdm_dem_cwd_context_v1_initialize(&alternate_cwd, &alternate_profile) ||
            !bx_ntvdm_dem_whole_provider_v1_initialize(&alternate,
                &alternate_profile, &space, &alternate_cwd)) failed = 61;
        else {
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 0u;
            if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&alternate, 0x2du,
                    &boundary, &cpu, oem_short, 0, 0u, &output_bytes, &result) ||
                cf_set(&result)) failed = 621;
            else {
                uint32_t readonly_token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                    result.cpu_delta.gpr16_values[5];
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                cpu.eax = readonly_token >> 16; cpu.esi = readonly_token & 0xffffu;
                if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&alternate, 0x2eu,
                        &boundary, &cpu, 0, 0, 0u, &output_bytes, &result) || cf_set(&result))
                    failed = 622;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&alternate,
                    0x2cu, &boundary, &cpu, oem_short, 0, 0u, &output_bytes, &result) ||
                !cf_set(&result) || !ax_is(&result, 5u))) failed = 623;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 0u;
            if (!failed && (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&alternate,
                    0x07u, &boundary, &cpu, oem_profile_pattern, 0, &result) ||
                !cf_set(&result) || !ax_is(&result, 5u) || !oem_file_exists(oem_rename_one)))
                failed = 624;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x03u,
                    &boundary, &cpu, oem_short, 0, &result) || !cf_set(&result) ||
                !ax_is(&result, 5u)) failed = 62;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 1u; cpu.ecx = FILE_ATTRIBUTE_HIDDEN;
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate,
                    0x01u, &boundary, &cpu, oem_short, 0, &result) || !cf_set(&result) ||
                !ax_is(&result, 5u))) failed = 63;
            if (!failed) {
                static const uint8_t mutations[] = { 0x03u, 0x04u, 0x05u,
                    0x06u, 0x17u, 0x22u };
                uint32_t index;
                for (index = 0u; index < sizeof(mutations); ++index) {
                    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                    if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, mutations[index],
                            &boundary, &cpu, oem_short,
                            mutations[index] == 0x17u ? oem_profile_pattern : 0, &result) ||
                        !cf_set(&result) || !ax_is(&result, 5u)) { failed = 625; break; }
                }
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&alternate,
                    0x20u, &boundary, &cpu, oem_profile_pattern, oem_profile_pattern, &result) ||
                !cf_set(&result) || !ax_is(&result, 5u))) failed = 626;
            bx_ntvdm_dem_whole_provider_v1_teardown(&alternate);
        }
    }
    if (!failed) {
        bx_ntvdm_mutation_profile_v1 alternate_profile;
        bx_ntvdm_dem_cwd_context_v1 alternate_cwd;
        bx_ntvdm_dem_whole_provider_v1 alternate;
        if (!profile_for_mode(&alternate_profile, BX_NTVDM_MUTATION_MODE_V1_OVERLAY) ||
            !bx_ntvdm_dem_cwd_context_v1_initialize(&alternate_cwd, &alternate_profile) ||
            !bx_ntvdm_dem_whole_provider_v1_initialize(&alternate,
                &alternate_profile, &space, &alternate_cwd)) failed = 64;
        else {
            char overlay_dir[MAX_PATH], overlay_renamed[MAX_PATH];
            char *overlay_dot;
            const uint8_t fcb_bop[4] = { 0xc4u, 0xc4u, 0x50u, 0x2fu };
            uint32_t overlay_fcb_token;
            uint32_t overlay_fcb_size = 0u;
            uint8_t overlay_host_before[8] = {0}, overlay_host_after[8] = {0};
            DWORD overlay_host_bytes = 0u, overlay_host_after_bytes = 0u;
            /* Exercise the complete Overlay FCB handle lifecycle against the
             * host-base file before the later 2C create truncates its private
             * COW copy.  The read result is a checked DTA write action. */
            file = CreateFileW(temporary, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE |
                FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);
            if (file == INVALID_HANDLE_VALUE || !ReadFile(file, overlay_host_before,
                    sizeof(overlay_host_before), &overlay_host_bytes, 0)) failed = 639;
            if (file != INVALID_HANDLE_VALUE) { CloseHandle(file); file = INVALID_HANDLE_VALUE; }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 0u;
            if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&alternate, 0x2du,
                    &boundary, &cpu, oem_short, 0, 0u, &output_bytes, &result) ||
                cf_set(&result)) failed = 640;
            else {
                overlay_fcb_token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                    result.cpu_delta.gpr16_values[5];
                overlay_fcb_size = ((uint32_t)result.cpu_delta.gpr16_values[2] << 16) |
                    result.cpu_delta.gpr16_values[6];
                bx_ntvdm_instruction_window_v1_capture(&window, fcb_bop, sizeof(fcb_bop));
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                token_into_cpu(&cpu, overlay_fcb_token); cpu.ebx = 1u; cpu.ecx = 2u;
                if (!bx_ntvdm_dem_fcb_io_route_partition_v1_dispatch(&alternate, 0x2fu,
                        &boundary, &cpu, &window, 0x300u, &fcb_action, &result) ||
                    cf_set(&result) || fcb_action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE ||
                    fcb_action.payload_bytes != 2u || memcmp(fcb_action.payload, "ab", 2u) != 0 ||
                    result.cpu_delta.gpr16_values[1] != 2u) failed = 641;
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                cpu.eax = overlay_fcb_token >> 16; cpu.esi = overlay_fcb_token & 0xffffu;
                if (!failed && (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&alternate,
                        0x2eu, &boundary, &cpu, 0, 0, 0u, &output_bytes, &result) ||
                    cf_set(&result))) failed = 642;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && !bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&alternate,
                    0x31u, &boundary, &cpu, oem_short, 0, 0u, &output_bytes, &result)) failed = 6430;
            else if (!failed && cf_set(&result)) failed = 6431;
            else if (!failed && result.cpu_delta.gpr16_values[7] != (uint16_t)overlay_fcb_size) failed = 6432;
            else if (!failed && result.cpu_delta.gpr16_values[3] !=
                (uint16_t)(overlay_fcb_size >> 16)) failed = 6433;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&alternate, 0x2cu,
                    &boundary, &cpu, oem_short, 0, 0u, &output_bytes, &result) ||
                cf_set(&result)) failed = 641;
            if (!failed) {
                overlay_fcb_token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                    result.cpu_delta.gpr16_values[5];
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                cpu.eax = overlay_fcb_token >> 16; cpu.esi = overlay_fcb_token & 0xffffu;
                if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&alternate, 0x2eu,
                        &boundary, &cpu, 0, 0, 0u, &output_bytes, &result) || cf_set(&result))
                    failed = 641;
            }
            file = CreateFileW(temporary, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE |
                FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);
            if (!failed && (file == INVALID_HANDLE_VALUE || !ReadFile(file, overlay_host_after,
                    sizeof(overlay_host_after), &overlay_host_after_bytes, 0) ||
                overlay_host_after_bytes != overlay_host_bytes || memcmp(overlay_host_after,
                    overlay_host_before, overlay_host_bytes) != 0)) failed = 644;
            if (file != INVALID_HANDLE_VALUE) { CloseHandle(file); file = INVALID_HANDLE_VALUE; }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x03u,
                    &boundary, &cpu, oem_short, 0, &result) || !cf_set(&result) ||
                !ax_is(&result, 1u))) failed = 65;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x01u,
                    &boundary, &cpu, oem_short, 0, &result) || cf_set(&result))) failed = 660;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 1u; cpu.ecx = FILE_ATTRIBUTE_HIDDEN;
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x01u,
                    &boundary, &cpu, oem_short, 0, &result) || cf_set(&result))) failed = 664;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x01u,
                    &boundary, &cpu, oem_short, 0, &result) || cf_set(&result) ||
                result.cpu_delta.gpr16_values[2] != FILE_ATTRIBUTE_HIDDEN)) failed = 665;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 0u;
            if (!failed && (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&alternate,
                    0x07u, &boundary, &cpu, oem_profile_pattern, 0, &result) ||
                cf_set(&result) || !oem_file_exists(oem_rename_one)))
                failed = 651;
            {
                char overlay_wild_source[MAX_PATH], overlay_wild_destination[MAX_PATH];
                wchar_t overlay_wild_relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
                wchar_t overlay_wild_target[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
                uint8_t overlay_wild_drive = 0u;
                if (!failed && (!replace_extension(overlay_wild_source, oem_short, ".OW1") ||
                    !replace_extension(overlay_wild_destination, oem_short, ".NEW") ||
                    bx_ntvdm_dem_path_v1_resolve(overlay_wild_source, &alternate_cwd,
                        &overlay_wild_drive, overlay_wild_relative) != BX_NTVDM_DEM_PATH_V1_OK ||
                    bx_ntvdm_dem_path_v1_resolve(overlay_wild_destination, &alternate_cwd,
                        &overlay_wild_drive, overlay_wild_target) != BX_NTVDM_DEM_PATH_V1_OK ||
                    !bx_ntvdm_dem_overlay_store_v1_put_file(&alternate.overlay_store,
                        overlay_wild_drive, overlay_wild_relative, FILE_ATTRIBUTE_NORMAL,
                        (const uint8_t *)"cow", 3u))) failed = 645;
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                if (!failed && (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&alternate,
                        0x20u, &boundary, &cpu, overlay_wild_source, overlay_wild_destination,
                        &result) || cf_set(&result) ||
                    bx_ntvdm_dem_overlay_store_v1_lookup(&alternate.overlay_store,
                        overlay_wild_drive, overlay_wild_relative) == 0 ||
                    bx_ntvdm_dem_overlay_store_v1_lookup(&alternate.overlay_store,
                        overlay_wild_drive, overlay_wild_relative)->state !=
                        BX_NTVDM_DEM_OVERLAY_STORE_V1_TOMBSTONE ||
                    bx_ntvdm_dem_overlay_store_v1_lookup(&alternate.overlay_store,
                        overlay_wild_drive, overlay_wild_target) == 0 ||
                    bx_ntvdm_dem_overlay_store_v1_lookup(&alternate.overlay_store,
                        overlay_wild_drive, overlay_wild_target)->byte_count != 3u)) failed = 646;
            }
            strcpy_s(overlay_dir, sizeof(overlay_dir), oem_short);
            strcpy_s(overlay_renamed, sizeof(overlay_renamed), oem_short);
            overlay_dot = strrchr(overlay_dir, '.');
            if (!failed && overlay_dot == 0) failed = 654;
            else if (!failed) strcpy_s(overlay_dot,
                sizeof(overlay_dir) - (size_t)(overlay_dot - overlay_dir), ".OD1");
            overlay_dot = strrchr(overlay_renamed, '.');
            if (!failed && overlay_dot == 0) failed = 655;
            else if (!failed) strcpy_s(overlay_dot,
                sizeof(overlay_renamed) - (size_t)(overlay_dot - overlay_renamed), ".OR1");
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate,
                    0x04u, &boundary, &cpu, overlay_dir, 0, &result) || cf_set(&result))) failed = 656;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate,
                    0x06u, &boundary, &cpu, overlay_dir, 0, &result) || cf_set(&result))) failed = 657;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate,
                    0x17u, &boundary, &cpu, oem_short, overlay_renamed, &result) ||
                cf_set(&result))) failed = 658;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate,
                    0x05u, &boundary, &cpu, overlay_renamed, 0, &result) ||
                cf_set(&result))) failed = 659;
            if (!failed) {
                static const uint8_t mutations[] = { 0x03u, 0x22u };
                uint32_t index;
                for (index = 0u; index < sizeof(mutations); ++index) {
                    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                    if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, mutations[index],
                            &boundary, &cpu, oem_short,
                            mutations[index] == 0x17u ? oem_profile_pattern : 0, &result) ||
                        !cf_set(&result) || !ax_is(&result, 1u)) { failed = 652; break; }
                }
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && !bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&alternate,
                    0x20u, &boundary, &cpu, oem_profile_pattern, oem_profile_pattern, &result)) failed = 6530;
            else if (!failed && !cf_set(&result)) failed = 6531;
            else if (!failed && !oem_file_exists(oem_rename_one)) failed = 6532;
            bx_ntvdm_dem_whole_provider_v1_teardown(&alternate);
        }
    }
    if (!failed) {
        bx_ntvdm_mutation_profile_v1 alternate_profile;
        bx_ntvdm_dem_cwd_context_v1 alternate_cwd;
        bx_ntvdm_dem_whole_provider_v1 alternate;
        if (!profile_for_mode(&alternate_profile, BX_NTVDM_MUTATION_MODE_V1_VIRTUAL)) failed = 6601;
        else if (!bx_ntvdm_dem_cwd_context_v1_initialize(&alternate_cwd, &alternate_profile)) failed = 6602;
        else if (!bx_ntvdm_dem_whole_provider_v1_initialize(&alternate,
                &alternate_profile, &space, &alternate_cwd)) failed = 6603;
        else {
            char virtual_dir[MAX_PATH] = {0}, virtual_file[MAX_PATH] = {0};
            char virtual_renamed[MAX_PATH] = {0}, virtual_check[MAX_PATH] = {0};
            uint32_t virtual_token = 0u;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 0u;
            if (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&alternate, 0x07u,
                    &boundary, &cpu, oem_profile_pattern, 0, &result) || !cf_set(&result) ||
                !ax_is(&result, 1u) || !oem_file_exists(oem_rename_one)) failed = 661;
            if (!failed && (sprintf_s(virtual_dir, sizeof(virtual_dir), "%c:\\VRTDIR",
                    (char)('A' + drive)) < 0 || sprintf_s(virtual_file, sizeof(virtual_file),
                    "%s\\VRT.TXT", virtual_dir) < 0 || sprintf_s(virtual_renamed,
                    sizeof(virtual_renamed), "%s\\NEW.TXT", virtual_dir) < 0 ||
                    strcpy_s(virtual_check, sizeof(virtual_check), virtual_dir + 2) != 0)) failed = 662;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x04u,
                    &boundary, &cpu, virtual_dir, 0, &result) || cf_set(&result))) failed = 663;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x03u,
                    &boundary, &cpu, virtual_file, 0, &result) || cf_set(&result))) failed = 664;
            if (!failed) virtual_token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                result.cpu_delta.gpr16_values[5];
            if (!failed && virtual_token == 0u) failed = 664;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 1u; cpu.ecx = FILE_ATTRIBUTE_HIDDEN;
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x01u,
                    &boundary, &cpu, virtual_file, 0, &result) || cf_set(&result))) failed = 665;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x01u,
                    &boundary, &cpu, virtual_file, 0, &result) || cf_set(&result) ||
                result.cpu_delta.gpr16_values[2] != FILE_ATTRIBUTE_HIDDEN)) failed = 666;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL); cpu.edx = drive + 1u;
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x44u,
                    &boundary, &cpu, virtual_check, 0, &result) || cf_set(&result))) failed = 667;
            if (!failed) {
                const uint8_t bop_seek[4] = { 0xc4u, 0xc4u, 0x50u, 0x00u };
                struct bx_ntvdm_mechanical_action_v1 handle_action;
                bx_ntvdm_instruction_window_v1_capture(&window, bop_seek, sizeof(bop_seek));
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                token_into_cpu(&cpu, virtual_token); cpu.ebx = 0u;
                if (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&alternate, 0x00u,
                        &boundary, &cpu, &window, &handle_action, &result) || cf_set(&result)) failed = 6671;
                ((uint8_t *)window.bytes)[3] = 0x1eu;
                cpu.ecx = 2u; cpu.edx = 0x300u; cpu.ds = 0u;
                if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&alternate, 0x1eu,
                        &boundary, &cpu, &window, &handle_action, &result) ||
                    handle_action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ ||
                    handle_action.payload_bytes != 2u)) failed = 6672;
                if (!failed) {
                    handle_action.payload[0] = 'h'; handle_action.payload[1] = 'i';
                    if (!bx_ntvdm_dem_handle_route_partition_v1_complete_read(&alternate, 0x1eu,
                            &boundary, &cpu, &handle_action, &result) || cf_set(&result) ||
                        !ax_is(&result, 2u)) failed = 6673;
                }
                ((uint8_t *)window.bytes)[3] = 0x00u;
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                token_into_cpu(&cpu, virtual_token); cpu.ebx = 0u;
                if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&alternate, 0x00u,
                        &boundary, &cpu, &window, &handle_action, &result) || cf_set(&result))) failed = 6674;
                ((uint8_t *)window.bytes)[3] = 0x16u;
                cpu.ecx = 2u; cpu.edx = 0x302u; cpu.ds = 0u; cpu.eflags = 0x40u;
                if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&alternate, 0x16u,
                        &boundary, &cpu, &window, &handle_action, &result) || cf_set(&result) ||
                    handle_action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE ||
                    handle_action.payload_bytes != 2u || memcmp(handle_action.payload, "hi", 2u) != 0)) failed = 6675;
                ((uint8_t *)window.bytes)[3] = 0x08u;
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                token_into_cpu(&cpu, virtual_token); cpu.ebx = 1u;
                cpu.ecx = 0x1234u; cpu.edx = 0x5678u;
                if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&alternate, 0x08u,
                        &boundary, &cpu, &window, &handle_action, &result) || cf_set(&result))) failed = 66751;
                bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                token_into_cpu(&cpu, virtual_token);
                if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&alternate, 0x08u,
                        &boundary, &cpu, &window, &handle_action, &result) || cf_set(&result) ||
                    result.cpu_delta.gpr16_values[1] != 0x1234u ||
                    result.cpu_delta.gpr16_values[2] != 0x5678u)) failed = 66752;
                ((uint8_t *)window.bytes)[3] = 0x02u;
                cpu.ecx = cpu.edx = 0xffffu;
                if (!failed && (!bx_ntvdm_dem_handle_route_partition_v1_dispatch(&alternate, 0x02u,
                        &boundary, &cpu, &window, &handle_action, &result) || cf_set(&result))) failed = 6676;
                if (!failed && bx_ntvdm_dem_handle_route_partition_v1_claims_request(
                        &alternate, 0x00u, &cpu)) failed = 6677;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x17u,
                    &boundary, &cpu, virtual_file, virtual_renamed, &result) || cf_set(&result))) failed = 668;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x05u,
                    &boundary, &cpu, virtual_renamed, 0, &result) || cf_set(&result))) failed = 669;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x06u,
                    &boundary, &cpu, virtual_dir, 0, &result) || cf_set(&result))) failed = 670;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&alternate,
                    0x20u, &boundary, &cpu, oem_profile_pattern, oem_profile_pattern, &result) ||
                !cf_set(&result) || !ax_is(&result, 1u))) failed = 671;
            bx_ntvdm_dem_whole_provider_v1_teardown(&alternate);
        }
    }
    bx_ntvdm_dem_whole_provider_v1_teardown(&provider);
    bx_ntvdm_dem_namespace_identity_observation_v1_enable(0u);
    failed |= bx_ntvdm_dem_whole_provider_v1_valid(&provider) != 0;
    bx_ntvdm_host_namespace_v1_release(&space);
    delete_oem_file(oem_wild_one);
    delete_oem_file(oem_wild_two);
    delete_oem_file(oem_wild_three);
    delete_oem_file(oem_rename_one);
    delete_oem_file(oem_rename_two);
    DeleteFileW(temporary);
    return failed;
}
