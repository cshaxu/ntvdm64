#include "bx_ntvdm_boot_namespace_plane_v1.h"
#include "bx_ntvdm_bop_ingress_v1.h"
#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_package_session_v1.h"
#include "bx_ntvdm_host_namespace.h"

#include <string.h>

static wchar_t direct_temp_root[MAX_PATH];

static void cleanup_direct_temp_root(void)
{
    wchar_t pattern[MAX_PATH];
    WIN32_FIND_DATAW data;
    HANDLE find;
    if (direct_temp_root[0] == L'\0' ||
        swprintf_s(pattern, MAX_PATH, L"%s\\*", direct_temp_root) < 0) return;
    find = FindFirstFileW(pattern, &data);
    if (find != INVALID_HANDLE_VALUE) {
        do {
            wchar_t child[MAX_PATH];
            if (wcscmp(data.cFileName, L".") == 0 || wcscmp(data.cFileName, L"..") == 0 ||
                swprintf_s(child, MAX_PATH, L"%s\\%s", direct_temp_root,
                    data.cFileName) < 0) continue;
            if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0u)
                RemoveDirectoryW(child);
            else DeleteFileW(child);
        } while (FindNextFileW(find, &data));
        FindClose(find);
    }
    RemoveDirectoryW(direct_temp_root);
    direct_temp_root[0] = L'\0';
}

/* Integration seam only: proves normal BOP ingress reaches the one installed
 * whole-provider session.  It deliberately does not reuse the unrelated
 * historical 73-service/GSET fixture. */
static uint8_t ram[0x100000];

static int create_owned_direct_file(const char *path)
{
    HANDLE handle = CreateFileA(path, GENERIC_WRITE, 0u, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, 0);
    if (handle == INVALID_HANDLE_VALUE) return 0;
    return CloseHandle(handle) != 0;
}

int bx_ntvdm_mantle_execute_mechanical_action_v1(
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    uint32_t index;
    if (!action || !bx_ntvdm_mechanical_action_v1_valid(action)) return 0;
    for (index = 0; index < action->range_count; ++index) {
        const struct bx_ntvdm_mechanical_action_v1_range *range =
            &action->ranges[index];
        if (range->physical_address > sizeof(ram) - range->byte_count) return 0;
        if (action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_READ)
            memcpy(action->payload + range->payload_offset,
                ram + range->physical_address, range->byte_count);
        else if (action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_WRITE)
            memcpy(ram + range->physical_address,
                action->payload + range->payload_offset, range->byte_count);
        else return 0;
    }
    return action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_READ ||
        action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
}

static void profile_initialize(byob_profile_selection *profile)
{
    memset(profile, 0, sizeof(*profile));
    memcpy(profile->command_placement.path, L"\\COMMAND.COM", sizeof(L"\\COMMAND.COM"));
    profile->command_placement.drive_index = 2u; profile->has_command_placement = 1u;
    memcpy(profile->target_placement.path, L"\\TARGET.COM", sizeof(L"\\TARGET.COM"));
    profile->target_placement.drive_index = 2u; profile->has_target_placement = 1u;
    profile->declared_target_count = 1u;
    memcpy(profile->declared_targets[0].component.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    memcpy(profile->declared_targets[0].placement.path, L"\\TARGET.COM", sizeof(L"\\TARGET.COM"));
    profile->declared_targets[0].placement.drive_index = 2u;
    memcpy(profile->target.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    memcpy(profile->config_file.path, L"\\CONFIG.SYS", sizeof(L"\\CONFIG.SYS"));
    profile->config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    memcpy(profile->autoexec_file.path, L"\\AUTOEXEC.BAT", sizeof(L"\\AUTOEXEC.BAT"));
    profile->autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    profile->has_guest_boot_files = 1u; profile->has_guest_search_metadata = 1u;
    profile->command_metadata.attributes = 0x20u; profile->target_metadata.attributes = 0x20u;
    profile->command_metadata.dos_date = 1u; profile->target_metadata.dos_date = 1u;
    profile->config_metadata.attributes = 0x20u; profile->autoexec_metadata.attributes = 0x20u;
    profile->config_metadata.dos_date = 1u; profile->autoexec_metadata.dos_date = 1u;
    profile->guest_display_state = BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1;
    profile->ntdos.bytes = 1u;
    memcpy(profile->ntdos.file_name, L"NTDOS.SYS", sizeof(L"NTDOS.SYS"));
}

static int dispatch(bx_ntvdm_dem_package_session_v1 *session, uint8_t service,
    bx_ntvdm_cpu_state_v1 *cpu, bx_ntvdm_cpu_result_v2 *result)
{
    uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, service };
    bx_ntvdm_instruction_window_v1 window; bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection; bx_ntvdm_exception_event_v1 event;
    memset(&event, 0, sizeof(event)); event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.cpu_id = 1u;
    event.vector = 6u; event.fault_rip = 0x100u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    return bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_package_session_v1_dispatch(session, &ingress, &selection,
            &event, cpu, &window, result);
}

static uint32_t token_from(const bx_ntvdm_cpu_result_v2 *result)
{
    return ((uint32_t)result->cpu_delta.gpr16_values[0] << 16) |
        result->cpu_delta.gpr16_values[5];
}

static int success(const bx_ntvdm_cpu_result_v2 *result)
{
    return result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == 0x104u &&
        ((result->eflags_write_mask & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
         (result->eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u);
}
static int cf_ax(const bx_ntvdm_cpu_result_v2 *result, uint16_t ax)
{
    return result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == 0x104u && result->cpu_delta.gpr16_write_mask == 1u &&
        result->cpu_delta.gpr16_values[0] == ax &&
        result->eflags_write_mask == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF &&
        result->eflags_values == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF;
}

int main(int argc, char **argv)
{
    static const uint32_t modes[] = {
        BX_NTVDM_MUTATION_MODE_V1_DIRECT,
        BX_NTVDM_MUTATION_MODE_V1_READONLY,
        BX_NTVDM_MUTATION_MODE_V1_OVERLAY,
        BX_NTVDM_MUTATION_MODE_V1_VIRTUAL
    };
    uint8_t ntdos_bytes[] = { 0xf4u }, command_bytes[] = { 0xf4u }, target_bytes[] = { 0xf4u };
    uint8_t drive_types[26] = {0}; byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) }, target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile; bx_ntvdm_host_drive_snapshot_v1 drives;
    bx_ntvdm_host_namespace_v1 host; wchar_t root[4] = L"C:\\"; DWORD type;
    wchar_t system_directory[MAX_PATH], temporary_path[MAX_PATH]; char fcb_host_path[MAX_PATH], direct_temp_oem[MAX_PATH], direct_file_oem[MAX_PATH], direct_renamed_oem[MAX_PATH], direct_dir_oem[MAX_PATH], direct_fcb_delete_pattern[MAX_PATH], direct_fcb_rename_pattern[MAX_PATH], direct_fcb_rename_template[MAX_PATH], direct_fcb_delete_one[MAX_PATH], direct_fcb_delete_two[MAX_PATH], direct_fcb_rename_one[MAX_PATH], direct_fcb_rename_two[MAX_PATH], direct_fcb_rename_out_one[MAX_PATH], direct_fcb_rename_out_two[MAX_PATH], direct_missing_oem[MAX_PATH]; UINT system_length, temporary_length;
    uint32_t index;
    int search_only = argc == 2 && strcmp(argv[1], "search-only") == 0;
    profile_initialize(&profile); type = GetDriveTypeW(root);
    if (type == DRIVE_NO_ROOT_DIR || type == DRIVE_UNKNOWN) return 1;
    system_length = GetSystemDirectoryW(system_directory, MAX_PATH);
    if (system_length == 0u || system_length >= MAX_PATH ||
        _wcsnicmp(system_directory, L"C:\\", 3u) != 0 ||
        wcscat_s(system_directory, MAX_PATH, L"\\kernel32.dll") != 0 ||
        WideCharToMultiByte(CP_OEMCP, 0, system_directory, -1, fcb_host_path,
            MAX_PATH, 0, 0) == 0) return 3;
    temporary_length = GetTempPathW(MAX_PATH, temporary_path);
    if (temporary_length == 0u || temporary_length >= MAX_PATH ||
        _wcsnicmp(temporary_path, L"C:\\", 3u) != 0 ||
        GetTempFileNameW(temporary_path, L"ntd", 0u, direct_temp_root) == 0u ||
        !DeleteFileW(direct_temp_root) || !CreateDirectoryW(direct_temp_root, 0) ||
        atexit(cleanup_direct_temp_root) != 0 ||
        WideCharToMultiByte(CP_OEMCP, 0, direct_temp_root, -1, direct_temp_oem,
            MAX_PATH, 0, 0) == 0 ||
        strcpy_s(direct_file_oem, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_file_oem, MAX_PATH, "\\HNDL.DAT") != 0 ||
        strcpy_s(direct_renamed_oem, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_renamed_oem, MAX_PATH, "\\RENAMED.DAT") != 0 ||
        strcpy_s(direct_dir_oem, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_dir_oem, MAX_PATH, "\\DOSDIR") != 0 ||
        strcpy_s(direct_fcb_delete_pattern, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_fcb_delete_pattern, MAX_PATH, "\\FCBD?.DAT") != 0 ||
        strcpy_s(direct_fcb_rename_pattern, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_fcb_rename_pattern, MAX_PATH, "\\FCBR?.DAT") != 0 ||
        strcpy_s(direct_fcb_rename_template, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_fcb_rename_template, MAX_PATH, "\\REN??.TMP") != 0 ||
        strcpy_s(direct_fcb_delete_one, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_fcb_delete_one, MAX_PATH, "\\FCBD1.DAT") != 0 ||
        strcpy_s(direct_fcb_delete_two, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_fcb_delete_two, MAX_PATH, "\\FCBD2.DAT") != 0 ||
        strcpy_s(direct_fcb_rename_one, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_fcb_rename_one, MAX_PATH, "\\FCBR1.DAT") != 0 ||
        strcpy_s(direct_fcb_rename_two, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_fcb_rename_two, MAX_PATH, "\\FCBR2.DAT") != 0 ||
        strcpy_s(direct_fcb_rename_out_one, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_fcb_rename_out_one, MAX_PATH, "\\RENR1.TMP") != 0 ||
        strcpy_s(direct_fcb_rename_out_two, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_fcb_rename_out_two, MAX_PATH, "\\RENR2.TMP") != 0 ||
        strcpy_s(direct_missing_oem, MAX_PATH, direct_temp_oem) != 0 ||
        strcat_s(direct_missing_oem, MAX_PATH, "\\NTDNOPE?.ZZZ") != 0) {
        cleanup_direct_temp_root(); return 4;
    }
    drive_types[2] = (uint8_t)type;
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(1u << 2u, drive_types, 0u, 0u, &drives) ||
        !bx_ntvdm_host_namespace_v1_initialize(&host, &drives)) return 2;
    for (index = 0u; index < sizeof(modes) / sizeof(modes[0]); ++index) {
        if (search_only && index >= 2u) break;
        bx_ntvdm_boot_namespace_plane_v1 plane; bx_ntvdm_dem_package_session_v1 session;
        bx_ntvdm_mutation_profile_v1 mutation; bx_ntvdm_cpu_state_v1 cpu;
        bx_ntvdm_cpu_result_v2 result; uint32_t token;
        bx_ntvdm_mutation_profile_v1_initialize(&mutation, modes[index]);
        if (!bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation, BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu)) { bx_ntvdm_host_namespace_v1_release(&host); return 10 + (int)index; }
        if (!bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation, BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu)) { bx_ntvdm_host_namespace_v1_release(&host); return 14 + (int)index; }
        if (!bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation, BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, 0x0fu)) { bx_ntvdm_host_namespace_v1_release(&host); return 18 + (int)index; }
        if (!bx_ntvdm_boot_namespace_plane_v1_initialize(&plane, &ntdos, &command, &target, 0, &profile)) { bx_ntvdm_host_namespace_v1_release(&host); return 22 + (int)index; }
        if (!bx_ntvdm_dem_package_session_v1_initialize(&session, &plane)) { bx_ntvdm_host_namespace_v1_release(&host); return 26 + (int)index; }
        if (!bx_ntvdm_dem_package_session_v1_set_mutation_profile(&session, &mutation)) { bx_ntvdm_host_namespace_v1_release(&host); return 30 + (int)index; }
        if (!bx_ntvdm_dem_package_session_v1_set_drive_snapshot(&session, &drives)) { bx_ntvdm_host_namespace_v1_release(&host); return 34 + (int)index; }
        if (!bx_ntvdm_dem_package_session_v1_set_drive_view_host_namespace(&session, &host)) { bx_ntvdm_host_namespace_v1_release(&host); return 38 + (int)index; }
        if (session.has_whole_provider || session.drive_view_host_namespace != &host) { bx_ntvdm_host_namespace_v1_release(&host); return 42 + (int)index; }
        if (!bx_ntvdm_dem_package_session_v1_set_host_namespace(&session, &host)) { bx_ntvdm_host_namespace_v1_release(&host); return 46 + (int)index; }
        if (!session.has_whole_provider) { bx_ntvdm_host_namespace_v1_release(&host); return 50 + (int)index; }
        memset(ram, 0, sizeof(ram));
        /* Register the bounded DTA/PDB transport once, then exercise a real
         * declared-image namespace -> handle -> RAM -> close chain. */
        ram[0x520u] = 0x00u; ram[0x521u] = 0x06u;
        ram[0x450u] = 0x34u; ram[0x451u] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ds = 0u; cpu.esi = 0x500u; cpu.eax = 0x400u;
        cpu.edx = 0x450u; cpu.ecx = 0x460u;
        if (!dispatch(&session, 0x1bu, &cpu, &result) ||
            result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) {
            bx_ntvdm_dem_package_session_v1_teardown(&session);
            bx_ntvdm_host_namespace_v1_release(&host); return 20 + (int)index;
        }
        memcpy(ram + 0x200u, "C:\\COMMAND.COM", sizeof("C:\\COMMAND.COM"));
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ds = 0u; cpu.esi = 0x200u; cpu.ebx = 0u;
        if (!dispatch(&session, 0x12u, &cpu, &result) || !success(&result) ||
            (result.cpu_delta.gpr16_write_mask & ((1u << 0u) | (1u << 5u))) !=
                ((1u << 0u) | (1u << 5u)) || token_from(&result) == 0u) {
            bx_ntvdm_dem_package_session_v1_teardown(&session);
            bx_ntvdm_host_namespace_v1_release(&host); return 30 + (int)index;
        }
        token = token_from(&result);
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ds = 0u; cpu.edx = 0x300u; cpu.ecx = 1u;
        cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
        if (!dispatch(&session, 0x16u, &cpu, &result) || !success(&result) ||
            ram[0x300u] != 0xf4u) {
            bx_ntvdm_dem_package_session_v1_teardown(&session);
            bx_ntvdm_host_namespace_v1_release(&host); return 40 + (int)index;
        }
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
        if (!dispatch(&session, 0x02u, &cpu, &result) || !success(&result)) {
            bx_ntvdm_dem_package_session_v1_teardown(&session);
            bx_ntvdm_host_namespace_v1_release(&host); return 50 + (int)index;
        }
        /* S3 FastRead integration. Direct uses an admitted host-view file;
         * Readonly uses the declared startup namespace. Both routes must
         * preserve AX:BP token layout and complete a checked guest-RAM write. */
        if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_DIRECT) {
            memcpy(ram + 0x200u, fcb_host_path, strlen(fcb_host_path) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x200u; cpu.ebx = 0u;
            if (!dispatch(&session, 0x12u, &cpu, &result) || !success(&result) ||
                token_from(&result) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 52;
            }
            token = token_from(&result); ram[0x310u] = 0u;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x310u; cpu.ecx = 1u;
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x42u, &cpu, &result) || !success(&result) ||
                result.cpu_delta.gpr16_values[0] != 1u || ram[0x310u] == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 53;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x02u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 54;
            }
        }
        if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_READONLY) {
            memcpy(ram + 0x200u, "C:\\COMMAND.COM", sizeof("C:\\COMMAND.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x200u; cpu.ebx = 0u;
            if (!dispatch(&session, 0x12u, &cpu, &result) || !success(&result) ||
                token_from(&result) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 55;
            }
            token = token_from(&result); ram[0x311u] = 0u;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x311u; cpu.ecx = 1u;
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x42u, &cpu, &result) || !success(&result) ||
                result.cpu_delta.gpr16_values[0] != 1u || ram[0x311u] != 0xf4u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 56;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x02u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 57;
            }
        }
        /* A released opaque handle must remain invalid across the two admitted
         * host-mutation views; do not fall through to a profile-default path. */
        if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_DIRECT ||
            modes[index] == BX_NTVDM_MUTATION_MODE_V1_READONLY) {
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu; cpu.ebx = 0u;
            if (!dispatch(&session, 0x00u, &cpu, &result) || !cf_ax(&result, 6u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 51 + (int)index;
            }
        }
        /* Readonly must refuse the entire host-mutation surface by capability,
         * rather than depending on whether a test path happens to exist. */
        if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_READONLY) {
            memcpy(ram + 0x800u, "C:\\COMMAND.COM", sizeof("C:\\COMMAND.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.ebx = 0u;
            if (!dispatch(&session, 0x12u, &cpu, &result) || !success(&result) ||
                token_from(&result) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 230;
            }
            token = token_from(&result);
            /* A valid readonly namespace token still owns ordinary seek state. */
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu; cpu.ebx = 0u;
            if (!dispatch(&session, 0x00u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 257;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            cpu.ebx = 1u; cpu.ecx = 0x0020u; cpu.edx = 0x5841u;
            if (!dispatch(&session, 0x08u, &cpu, &result) || !cf_ax(&result, 5u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 229;
            }
            ram[0x300u] = 'r';
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            cpu.ds = 0u; cpu.edx = 0x300u; cpu.ecx = 1u;
            if (!dispatch(&session, 0x1eu, &cpu, &result) || !cf_ax(&result, 5u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 231;
            }
            /* demCommit has no content mutation in a readonly namespace; it
             * retains the source-shaped successful no-op terminal. */
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x27u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 232;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x02u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 233;
            }
            memcpy(ram + 0x800u, direct_file_oem, strlen(direct_file_oem) + 1u);
            memcpy(ram + 0x900u, direct_renamed_oem, strlen(direct_renamed_oem) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.ecx = 0u;
            if (!dispatch(&session, 0x03u, &cpu, &result) || !cf_ax(&result, 5u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 233;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.ecx = 0u;
            if (!dispatch(&session, 0x22u, &cpu, &result) || !cf_ax(&result, 5u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 234;
            }
            { static const uint8_t mutations[] = { 0x04u, 0x05u, 0x06u };
              uint32_t mutation_index;
              for (mutation_index = 0u; mutation_index < sizeof(mutations); ++mutation_index) {
                  bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                  cpu.ds = 0u; cpu.edx = 0x800u;
                  if (!dispatch(&session, mutations[mutation_index], &cpu, &result) ||
                      !cf_ax(&result, 5u)) {
                      bx_ntvdm_dem_package_session_v1_teardown(&session);
                      bx_ntvdm_host_namespace_v1_release(&host); return 235 + (int)mutation_index;
                  }
              }
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u; cpu.es = 0u; cpu.edi = 0x900u;
            if (!dispatch(&session, 0x17u, &cpu, &result) || !cf_ax(&result, 5u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 238;
            }
            memcpy(ram + 0x800u, direct_file_oem, strlen(direct_file_oem) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u;
            if (!dispatch(&session, 0x2cu, &cpu, &result) || !cf_ax(&result, 5u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 239;
            }
            memcpy(ram + 0x800u, "C:\\*.COM", sizeof("C:\\*.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.es = 0u; cpu.edi = 0x800u;
            if (!dispatch(&session, 0x07u, &cpu, &result) || !cf_ax(&result, 5u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 239;
            }
            memcpy(ram + 0x900u, "C:\\REN??.TMP", sizeof("C:\\REN??.TMP"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.es = 0u; cpu.edi = 0x900u;
            if (!dispatch(&session, 0x20u, &cpu, &result) || !cf_ax(&result, 5u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 246;
            }
        }
        /* Direct and Readonly use the declared merged snapshot.  The private
         * Overlay/Virtual search views are exercised by their own matrix leg. */
        if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_DIRECT ||
            modes[index] == BX_NTVDM_MUTATION_MODE_V1_READONLY) {
            memset(ram + 0x600u, 0, 43u);
            ram[0x400u] = 0x00u; ram[0x401u] = 0x06u;
            ram[0x402u] = 0x00u; ram[0x403u] = 0x00u;
            memcpy(ram + 0x700u, "C:\\*.COM", sizeof("C:\\*.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x700u; cpu.ecx = 0u;
            if (!dispatch(&session, 0x09u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 60 + (int)index;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!dispatch(&session, 0x0bu, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 70 + (int)index;
            }
            /* OpenNT demFindFirstFCB/demFindNextFCB retain their separate
             * SRCHBUF contract: first gathers DS:SI + ES:DI + PDB, then next
             * consumes that same SRCHBUF token state. */
            memset(ram + 0x600u, 0, 53u);
            memcpy(ram + 0x700u, "C:\\*.COM", sizeof("C:\\*.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x600u; cpu.es = 0u; cpu.edi = 0x700u;
            if (!dispatch(&session, 0x0au, &cpu, &result) || !success(&result) ||
                (ram[0x620u] | ram[0x621u] | ram[0x622u] | ram[0x623u] |
                 ram[0x624u] | ram[0x625u] | ram[0x626u] | ram[0x627u]) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 75 + (int)index;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x600u;
            if (!dispatch(&session, 0x0cu, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 77 + (int)index;
            }
            /* A no-match FindFirst must retain the OpenNT DOS error terminal
             * (AX=18, CF=1), independently of later file-info services. */
            memcpy(ram + 0x700u, direct_missing_oem, strlen(direct_missing_oem) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x700u;
            if (!dispatch(&session, 0x09u, &cpu, &result) || !cf_ax(&result, 18u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 83 + (int)index;
            }
            if (search_only) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                continue;
            }
            memcpy(ram + 0x800u, "C:\\COMMAND.COM", sizeof("C:\\COMMAND.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u;
            if (!dispatch(&session, 0x01u, &cpu, &result) || !success(&result) ||
                (result.cpu_delta.gpr16_write_mask & (1u << 2u)) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 79 + (int)index;
            }
            memcpy(ram + 0x800u, direct_temp_oem, strlen(direct_temp_oem) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.edx = 3u;
            if (!dispatch(&session, 0x44u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 81 + (int)index;
            }
            memset(ram + 0x600u, 0, 53u);
            memcpy(ram + 0x700u, direct_missing_oem, strlen(direct_missing_oem) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x700u;
            if (!dispatch(&session, 0x09u, &cpu, &result) || !cf_ax(&result, 18u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 83 + (int)index;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x600u; cpu.es = 0u; cpu.edi = 0x700u;
            if (!dispatch(&session, 0x0au, &cpu, &result) || !cf_ax(&result, 18u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 85 + (int)index;
            }
        }
        /* Direct package chain: the test-owned temporary root permits an
         * original-shaped create/write/seek/read/time/commit/close/delete
         * sequence without changing any ambient host file. */
        if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_DIRECT) {
            memcpy(ram + 0x800u, direct_file_oem, strlen(direct_file_oem) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.ecx = 0u;
            if (!dispatch(&session, 0x03u, &cpu, &result) || !success(&result) ||
                token_from(&result) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 220;
            }
            token = token_from(&result); ram[0x300u] = 'x'; ram[0x301u] = 'y';
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            cpu.ds = 0u; cpu.edx = 0x300u; cpu.ecx = 2u;
            if (!dispatch(&session, 0x1eu, &cpu, &result) || !success(&result) ||
                result.cpu_delta.gpr16_values[0] != 2u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 221;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu; cpu.ebx = 0u;
            if (!dispatch(&session, 0x00u, &cpu, &result) || !success(&result) ||
                result.cpu_delta.gpr16_values[0] != 0u || result.cpu_delta.gpr16_values[2] != 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 222;
            }
            ram[0x300u] = 0u; ram[0x301u] = 0u;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            cpu.ds = 0u; cpu.edx = 0x300u; cpu.ecx = 2u;
            if (!dispatch(&session, 0x16u, &cpu, &result) || !success(&result) ||
                result.cpu_delta.gpr16_values[0] != 2u || ram[0x300u] != 'x' ||
                ram[0x301u] != 'y') {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 223;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu; cpu.ebx = 0u;
            if (!dispatch(&session, 0x08u, &cpu, &result) || !success(&result) ||
                (result.cpu_delta.gpr16_write_mask & ((1u << 1u) | (1u << 2u))) !=
                    ((1u << 1u) | (1u << 2u))) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 224;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            cpu.ebx = 1u; cpu.ecx = 0x0020u; cpu.edx = 0x5841u;
            if (!dispatch(&session, 0x08u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 254;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x27u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 225;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x02u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 226;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u;
            if (!dispatch(&session, 0x05u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 227;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.ecx = 0u;
            if (!dispatch(&session, 0x22u, &cpu, &result) || !success(&result) || token_from(&result) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 228;
            }
            token = token_from(&result); bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x02u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 229;
            }
            memcpy(ram + 0x900u, direct_renamed_oem, strlen(direct_renamed_oem) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u; cpu.es = 0u; cpu.edi = 0x900u;
            if (!dispatch(&session, 0x17u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 240;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x900u;
            if (!dispatch(&session, 0x01u, &cpu, &result) || !success(&result) || (result.cpu_delta.gpr16_write_mask & (1u << 2u)) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 241;
            }
            memcpy(ram + 0x800u, direct_dir_oem, strlen(direct_dir_oem) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u;
            if (!dispatch(&session, 0x04u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 242;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.edx = 3u;
            if (!dispatch(&session, 0x44u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 243;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u;
            if (!dispatch(&session, 0x06u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 244;
            }
            memcpy(ram + 0x800u, direct_renamed_oem, strlen(direct_renamed_oem) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u;
            if (!dispatch(&session, 0x05u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 245;
            }
            memcpy(ram + 0x800u, direct_file_oem, strlen(direct_file_oem) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u;
            if (!dispatch(&session, 0x2cu, &cpu, &result) || !success(&result) || token_from(&result) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 251;
            }
            token = token_from(&result); bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.esi = token & 0xffffu;
            if (!dispatch(&session, 0x2eu, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 252;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u;
            if (!dispatch(&session, 0x05u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 253;
            }
            if (!create_owned_direct_file(direct_fcb_delete_one) ||
                !create_owned_direct_file(direct_fcb_delete_two)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 247;
            }
            memset(ram + 0x800u, 0, 128u);
            memcpy(ram + 0x800u, direct_fcb_delete_pattern, strlen(direct_fcb_delete_pattern) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.es = 0u; cpu.edi = 0x800u;
            if (!dispatch(&session, 0x07u, &cpu, &result) || !success(&result) ||
                GetFileAttributesA(direct_fcb_delete_one) != INVALID_FILE_ATTRIBUTES ||
                GetFileAttributesA(direct_fcb_delete_two) != INVALID_FILE_ATTRIBUTES) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 248;
            }
            if (!create_owned_direct_file(direct_fcb_rename_one) ||
                !create_owned_direct_file(direct_fcb_rename_two)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 249;
            }
            memset(ram + 0x800u, 0, 128u); memset(ram + 0x900u, 0, 128u);
            memcpy(ram + 0x800u, direct_fcb_rename_pattern, strlen(direct_fcb_rename_pattern) + 1u);
            memcpy(ram + 0x900u, direct_fcb_rename_template, strlen(direct_fcb_rename_template) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.es = 0u; cpu.edi = 0x900u;
            if (!dispatch(&session, 0x20u, &cpu, &result) || !success(&result) ||
                GetFileAttributesA(direct_fcb_rename_one) != INVALID_FILE_ATTRIBUTES ||
                GetFileAttributesA(direct_fcb_rename_two) != INVALID_FILE_ATTRIBUTES ||
                GetFileAttributesA(direct_fcb_rename_out_one) == INVALID_FILE_ATTRIBUTES ||
                GetFileAttributesA(direct_fcb_rename_out_two) == INVALID_FILE_ATTRIBUTES) {
                bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host); return 250;
            }
        }
        /* Direct and Readonly retain OpenNT-style host file access.  Use a
         * real host file only for a read-only FCB open/read/close chain; no
         * host mutation is admitted by this session matrix leg. */
        if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_DIRECT ||
            modes[index] == BX_NTVDM_MUTATION_MODE_V1_READONLY) {
            memcpy(ram + 0x800u, fcb_host_path, strlen(fcb_host_path) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u;
            if (!dispatch(&session, 0x31u, &cpu, &result) || !success(&result) ||
                (result.cpu_delta.gpr16_write_mask & 0x8fu) != 0x8fu) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 185 + (int)index;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.eax = 0u;
            if (!dispatch(&session, 0x2du, &cpu, &result) || !success(&result) ||
                (result.cpu_delta.gpr16_write_mask & ((1u << 0u) | (1u << 5u))) !=
                    ((1u << 0u) | (1u << 5u)) || token_from(&result) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 190 + (int)index;
            }
            token = token_from(&result);
            if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_READONLY) {
                ram[0x600u] = 'x'; bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
                cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
                cpu.ecx = 1u; cpu.ebx = 0u;
                if (!dispatch(&session, 0x2fu, &cpu, &result) || !cf_ax(&result, 5u)) {
                    bx_ntvdm_dem_package_session_v1_teardown(&session);
                    bx_ntvdm_host_namespace_v1_release(&host); return 195 + (int)index;
                }
            }
            ram[0x600u] = 0u;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            cpu.ecx = 1u; cpu.ebx = 1u;
            if (!dispatch(&session, 0x2fu, &cpu, &result) || !success(&result) ||
                (result.cpu_delta.gpr16_write_mask & (1u << 1u)) == 0u ||
                result.cpu_delta.gpr16_values[1] != 1u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 200 + (int)index;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.esi = token & 0xffffu;
            if (!dispatch(&session, 0x2eu, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 210 + (int)index;
            }
        }
        /* Overlay and Virtual must create their own private file through the
         * namespace owner before the FCB path owner can open it. */
        if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_OVERLAY ||
            modes[index] == BX_NTVDM_MUTATION_MODE_V1_VIRTUAL) {
            memcpy(ram + 0x800u, "C:\\FCBTEST.COM", sizeof("C:\\FCBTEST.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.ecx = 0u;
            if (!dispatch(&session, 0x03u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 80 + (int)index;
            }

            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.eax = 2u;
            if (!dispatch(&session, 0x2du, &cpu, &result) || !success(&result) ||
                (result.cpu_delta.gpr16_write_mask & ((1u << 0u) | (1u << 5u))) !=
                    ((1u << 0u) | (1u << 5u)) || token_from(&result) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 90 + (int)index;
            }
            token = token_from(&result);
            ram[0x400u] = 'a'; ram[0x401u] = 'b';
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            cpu.ecx = 2u; cpu.ebx = 0u;
            if (!dispatch(&session, 0x2fu, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 100 + (int)index;
            }
            ram[0x400u] = 0u; ram[0x401u] = 0u;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            cpu.ecx = 2u; cpu.ebx = 1u;
            if (!dispatch(&session, 0x2fu, &cpu, &result) || !success(&result) ||
                ram[0x400u] != 'a' || ram[0x401u] != 'b') {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 110 + (int)index;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.esi = token & 0xffffu;
            if (!dispatch(&session, 0x2eu, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 100 + (int)index;
            }
            memcpy(ram + 0x800u, "C:\\FCBTWO.COM", sizeof("C:\\FCBTWO.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x800u; cpu.ecx = 0u;
            if (!dispatch(&session, 0x03u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 120 + (int)index;
            }
            memset(ram + 0x600u, 0, 43u);
            ram[0x400u] = 0x00u; ram[0x401u] = 0x06u;
            ram[0x402u] = 0x00u; ram[0x403u] = 0x00u;
            memcpy(ram + 0x700u, "C:\\FCB*.COM", sizeof("C:\\FCB*.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x700u; cpu.ecx = 0u;
            if (!dispatch(&session, 0x09u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 130 + (int)index;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!dispatch(&session, 0x0bu, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 140 + (int)index;
            }
            memcpy(ram + 0x800u, "C:\\FCBTWO.COM", sizeof("C:\\FCBTWO.COM"));
            memcpy(ram + 0x900u, "C:\\FCBREN.COM", sizeof("C:\\FCBREN.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u; cpu.es = 0u; cpu.edi = 0x900u;
            if (!dispatch(&session, 0x17u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 150 + (int)index;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x900u;
            if (!dispatch(&session, 0x05u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 160 + (int)index;
            }
            memcpy(ram + 0x800u, "C:\\FCBDIR", sizeof("C:\\FCBDIR"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u;
            if (!dispatch(&session, 0x04u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 170 + (int)index;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.edx = 0x800u;
            if (!dispatch(&session, 0x06u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 180 + (int)index;
            }
        }
        /* S2 lifecycle integration: 50:3C must release only Direct tokens
         * owned by the copied PDB, yet preserve Readonly startup namespace
         * tokens whose owner is session scope (zero). Both notifications are
         * source-shaped void resumes; the following seek distinguishes their
         * private-resource dispositions. */
        if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_DIRECT) {
            memcpy(ram + 0x200u, fcb_host_path, strlen(fcb_host_path) + 1u);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x200u; cpu.ebx = 0u;
            if (!dispatch(&session, 0x12u, &cpu, &result) || !success(&result) ||
                token_from(&result) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 260;
            }
            token = token_from(&result);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ebx = 0x1234u;
            if (!dispatch(&session, 0x3cu, &cpu, &result) ||
                result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
                result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 0u ||
                result.eflags_write_mask != 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 261;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu; cpu.ebx = 0u;
            if (!dispatch(&session, 0x00u, &cpu, &result) || !cf_ax(&result, 6u)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 262;
            }
        }
        if (modes[index] == BX_NTVDM_MUTATION_MODE_V1_READONLY) {
            memcpy(ram + 0x200u, "C:\\COMMAND.COM", sizeof("C:\\COMMAND.COM"));
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = 0u; cpu.esi = 0x200u; cpu.ebx = 0u;
            if (!dispatch(&session, 0x12u, &cpu, &result) || !success(&result) ||
                token_from(&result) == 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 263;
            }
            token = token_from(&result);
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ebx = 0x1234u;
            if (!dispatch(&session, 0x3cu, &cpu, &result) ||
                result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
                result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 0u ||
                result.eflags_write_mask != 0u) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 264;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu; cpu.ebx = 0u;
            if (!dispatch(&session, 0x00u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 265;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x02u, &cpu, &result) || !success(&result)) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host); return 266;
            }
        }
        /* The source-owned FCB direct route has two safe, profile-independent
         * terminals: DOS date query and no-op close of a zero opaque token. */
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!dispatch(&session, 0x30u, &cpu, &result) || !success(&result) ||
            (result.cpu_delta.gpr16_write_mask & ((1u << 0u) | (1u << 2u))) !=
                ((1u << 0u) | (1u << 2u))) {
            bx_ntvdm_dem_package_session_v1_teardown(&session);
            bx_ntvdm_host_namespace_v1_release(&host); return 80 + (int)index;
        }
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!dispatch(&session, 0x2eu, &cpu, &result) || !success(&result)) {
            bx_ntvdm_dem_package_session_v1_teardown(&session);
            bx_ntvdm_host_namespace_v1_release(&host); return 90 + (int)index;
        }
        /* All four profile views must reach the same installed provider. */
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!dispatch(&session, 0x00u, &cpu, &result) || !cf_ax(&result, 6u) ||
            !dispatch(&session, 0x47u, &cpu, &result) || !cf_ax(&result, 6u) ||
            !dispatch(&session, 0x48u, &cpu, &result) || !cf_ax(&result, 6u)) {
            bx_ntvdm_dem_package_session_v1_teardown(&session);
            bx_ntvdm_host_namespace_v1_release(&host); return 20 + (int)index;
        }
        bx_ntvdm_dem_package_session_v1_teardown(&session);
    }
    bx_ntvdm_host_namespace_v1_release(&host);
    return 0;
}
