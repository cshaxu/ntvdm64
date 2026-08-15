#include "bx_ntvdm_dem_whole_provider_v1.h"
#include "bx_ntvdm_dem_handle_partition_v1.h"
#include "bx_ntvdm_dem_namespace_partition_v1.h"
#include "bx_ntvdm_dem_fcb_handle_partition_v1.h"
#include "bx_ntvdm_dem_fcb_wildcard_partition_v1.h"
#include "bx_ntvdm_dem_fcb_io_route_partition_v1.h"

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
    uint8_t drive;
    uint32_t service;
    int failed = 0;

    if (GetTempPathW(MAX_PATH, temporary) == 0u ||
        GetTempFileNameW(temporary, L"n64", 0u, temporary) == 0u ||
        GetShortPathNameW(temporary, short_name, MAX_PATH) == 0u ||
        short_name[1] != L':' || WideCharToMultiByte(CP_OEMCP, 0, short_name,
            -1, oem_short, (int)sizeof(oem_short), 0, 0) == 0) return 1;
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
                 (1u << 4u) | (1u << 5u))) !=
                ((1u << 0u) | (1u << 1u) | (1u << 2u) | (1u << 3u) |
                 (1u << 4u) | (1u << 5u))) failed = 66;
        else {
            fcb_token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
                result.cpu_delta.gpr16_values[5];
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            token_into_cpu(&cpu, fcb_token); cpu.ebx = 1u; cpu.ecx = 2u;
            if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider, 0x2fu,
                &boundary, &cpu, 0, output, sizeof(output), &output_bytes, &result) ||
                cf_set(&result) || output_bytes != 2u ||
                result.cpu_delta.gpr16_values[2] != 2u) failed = 67;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = fcb_token >> 16; cpu.esi = fcb_token & 0xffffu;
            if (!failed && (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider,
                    0x2eu, &boundary, &cpu, 0, 0, 0u, &output_bytes, &result) ||
                cf_set(&result))) failed = 68;
        }
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!failed && (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider,
                0x30u, &boundary, &cpu, 0, 0, 0u, &output_bytes, &result) ||
            cf_set(&result) || (result.cpu_delta.gpr16_write_mask & 9u) != 9u)) failed = 69;
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
                    result.cpu_delta.gpr16_values[2] != 2u) failed = 693;
            }
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = fcb_token >> 16; cpu.ebp = fcb_token & 0xffffu;
            cpu.ecx = 2u; cpu.ebx = 1u;
            if (!failed && (!bx_ntvdm_dem_fcb_io_route_partition_v1_dispatch(&provider,
                    0x2fu, &boundary, &cpu, &window, 0x300u, &fcb_action, &result) ||
                fcb_action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE ||
                fcb_action.payload_bytes != 2u || memcmp(fcb_action.payload, "ab", 2u) != 0 ||
                cf_set(&result) || result.cpu_delta.gpr16_values[2] != 2u)) failed = 694;
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
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&provider,
                0x31u, &boundary, &cpu, oem_short, 0, 0u, &output_bytes, &result) ||
            cf_set(&result) || (result.cpu_delta.gpr16_write_mask & 0x4fu) != 0x4fu) failed = 70;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&provider, 0x01u,
                &boundary, &cpu, oem_short, 0, &result) || cf_set(&result) ||
            (result.cpu_delta.gpr16_write_mask & (1u << 2u)) == 0u) failed = 49;
    }
    if (!failed) {
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
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
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&alternate, 0x2cu,
                    &boundary, &cpu, oem_short, 0, 0u, &output_bytes, &result) ||
                !cf_set(&result) || !ax_is(&result, 1u)) failed = 641;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!failed && (!bx_ntvdm_dem_namespace_partition_v1_dispatch(&alternate, 0x03u,
                    &boundary, &cpu, oem_short, 0, &result) || !cf_set(&result) ||
                !ax_is(&result, 1u))) failed = 65;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 0u;
            if (!failed && (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&alternate,
                    0x07u, &boundary, &cpu, oem_profile_pattern, 0, &result) ||
                !cf_set(&result) || !ax_is(&result, 1u) || !oem_file_exists(oem_rename_one)))
                failed = 651;
            bx_ntvdm_dem_whole_provider_v1_teardown(&alternate);
        }
    }
    if (!failed) {
        bx_ntvdm_mutation_profile_v1 alternate_profile;
        bx_ntvdm_dem_cwd_context_v1 alternate_cwd;
        bx_ntvdm_dem_whole_provider_v1 alternate;
        if (!profile_for_mode(&alternate_profile, BX_NTVDM_MUTATION_MODE_V1_VIRTUAL) ||
            !bx_ntvdm_dem_cwd_context_v1_initialize(&alternate_cwd, &alternate_profile) ||
            !bx_ntvdm_dem_whole_provider_v1_initialize(&alternate,
                &alternate_profile, &space, &alternate_cwd)) failed = 66;
        else {
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.eax = 0u;
            if (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(&alternate, 0x07u,
                    &boundary, &cpu, oem_profile_pattern, 0, &result) || !cf_set(&result) ||
                !ax_is(&result, 1u) || !oem_file_exists(oem_rename_one)) failed = 661;
            bx_ntvdm_dem_whole_provider_v1_teardown(&alternate);
        }
    }
    bx_ntvdm_dem_whole_provider_v1_teardown(&provider);
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
