#include "bx_ntvdm_dem_search_partition_v1.h"

#include <string.h>
#include <wctype.h>

static int profile_for(bx_ntvdm_mutation_profile_v1 *profile, uint32_t mode)
{
    bx_ntvdm_mutation_profile_v1_initialize(profile, mode);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, 0x0fu);
}

static void u32(uint8_t *out, uint32_t value)
{
    out[0] = (uint8_t)value;
    out[1] = (uint8_t)(value >> 8);
    out[2] = (uint8_t)(value >> 16);
    out[3] = (uint8_t)(value >> 24);
}

static int invoke(bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_dem_dta_registration_v1 *dta, uint8_t service,
    bx_ntvdm_exception_event_v1 *boundary, bx_ntvdm_cpu_state_v1 *cpu,
    const uint8_t *input, uint32_t input_bytes,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[51], uint32_t *payload_bytes)
{
    uint8_t opcode[4] = { 0xc4u, 0xc4u, 0x50u, service };
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_guest_gather_read_action_v1 action;
    bx_ntvdm_instruction_window_v1_capture(&window, opcode, sizeof(opcode));
    return bx_ntvdm_dem_search_partition_v1_prepare(provider, dta, service,
        boundary, cpu, &window, &action) && action.total_bytes == input_bytes &&
        bx_ntvdm_dem_search_partition_v1_complete(provider, service, boundary,
            cpu, &action, input, input_bytes, transaction, payload,
            payload_bytes);
}

int main(void)
{
    wchar_t temporary[MAX_PATH], directory[MAX_PATH], short_directory[MAX_PATH];
    wchar_t alpha[MAX_PATH], zeta[MAX_PATH];
    char oem_directory[MAX_PATH], request[128] = {0};
    uint8_t path_first[134] = {0}, path_next[45] = {0};
    uint8_t fcb_first[183] = {0}, fcb_next[55] = {0}, payload[51] = {0};
    uint8_t drive, *dta_bytes = path_first + 128u;
    DWORD written = 0u;
    HANDLE file = INVALID_HANDLE_VALUE;
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_host_drive_snapshot_v1 snapshot = {0};
    bx_ntvdm_host_namespace_v1 space = {0};
    bx_ntvdm_dem_cwd_context_v1 cwd;
    bx_ntvdm_dem_whole_provider_v1 provider;
    bx_ntvdm_dem_dta_registration_v1 dta = { 0x900u, 0xa00u, 0xb00u, 0xc00u };
    bx_ntvdm_exception_event_v1 boundary = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(boundary),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x100u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    uint32_t payload_bytes = 0u;
    int failed = 0;

    if (!GetTempPathW(MAX_PATH, temporary) || !GetTempFileNameW(temporary,
            L"n64", 0u, directory) || !DeleteFileW(directory) ||
        !CreateDirectoryW(directory, 0) || !GetShortPathNameW(directory,
            short_directory, MAX_PATH) || short_directory[1] != L':' ||
        !WideCharToMultiByte(CP_OEMCP, 0, short_directory, -1, oem_directory,
            (int)sizeof(oem_directory), 0, 0)) return 1;
    swprintf_s(alpha, MAX_PATH, L"%s\\ALPHA.TXT", short_directory);
    swprintf_s(zeta, MAX_PATH, L"%s\\ZETA.TXT", short_directory);
    file = CreateFileW(alpha, GENERIC_WRITE, 0u, 0, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE || !WriteFile(file, "a", 1u, &written,
            0) || written != 1u) failed = 2;
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    file = CreateFileW(zeta, GENERIC_WRITE, 0u, 0, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, 0);
    if (!failed && (file == INVALID_HANDLE_VALUE || !WriteFile(file, "z", 1u,
            &written, 0) || written != 1u)) failed = 3;
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    drive = (uint8_t)(towupper((wint_t)short_directory[0]) - L'A');
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = UINT32_C(1) << drive;
    snapshot.admitted_mask = snapshot.present_mask;
    snapshot.types[drive] = DRIVE_FIXED;
    if (!failed && (!profile_for(&profile, BX_NTVDM_MUTATION_MODE_V1_DIRECT) || !bx_ntvdm_host_namespace_v1_initialize(
            &space, &snapshot) || !bx_ntvdm_dem_cwd_context_v1_initialize(&cwd,
            &profile) || !bx_ntvdm_dem_whole_provider_v1_initialize(&provider,
            &profile, &space, &cwd))) failed = 4;
    if (!failed) {
        strcpy_s(request, sizeof(request), oem_directory);
        strcat_s(request, sizeof(request), "\\*.TXT");
        memcpy(path_first, request, strlen(request) + 1u);
        dta_bytes[0] = 0x00u; dta_bytes[1] = 0x09u;
        dta_bytes[2] = 0x00u; dta_bytes[3] = 0x00u;
        path_first[132] = 0x34u; path_first[133] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x09u, &boundary, &cpu, path_first,
                sizeof(path_first), &transaction, payload, &payload_bytes) ||
            payload_bytes != 30u || memcmp(payload + 17u, "ALPHA.TXT", 9u))
            failed = 5;
    }
    if (!failed) {
        memcpy(path_next, payload, 8u); path_next[43] = 0x34u;
        path_next[44] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x0bu, &boundary, &cpu, path_next,
                sizeof(path_next), &transaction, payload, &payload_bytes) ||
            payload_bytes != 30u || memcmp(payload + 17u, "ZETA.TXT", 8u))
            failed = 6;
    }
    if (!failed) {
        bx_ntvdm_search_transaction_v1_release(&provider.search);
        bx_ntvdm_search_transaction_v1_initialize(&provider.search);
        memcpy(fcb_first + 53u, request, strlen(request) + 1u);
        fcb_first[181] = 0x34u; fcb_first[182] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x0au, &boundary, &cpu, fcb_first,
                sizeof(fcb_first), &transaction, payload, &payload_bytes) ||
            payload_bytes != 51u || memcmp(payload, "ALPHA   TXT", 11u)) failed = 7;
    }
    if (!failed) {
        memcpy(fcb_next + 32u, payload + 31u, 8u);
        fcb_next[53] = 0x34u; fcb_next[54] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x0cu, &boundary, &cpu, fcb_next,
                sizeof(fcb_next), &transaction, payload, &payload_bytes) ||
            payload_bytes != 51u || memcmp(payload, "ZETA    TXT", 11u)) failed = 8;
    }
    if (!failed && (!bx_ntvdm_dem_search_partition_v1_owns_service(0x09u) ||
        !bx_ntvdm_dem_search_partition_v1_owns_service(0x0au) ||
        !bx_ntvdm_dem_search_partition_v1_owns_service(0x0bu) ||
        !bx_ntvdm_dem_search_partition_v1_owns_service(0x0cu) ||
        bx_ntvdm_dem_search_partition_v1_owns_service(0x0du))) failed = 9;
    if (!failed) {
        bx_ntvdm_search_query_v1 overlay_query;
        wchar_t overlay_alpha[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
        wchar_t overlay_beta[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
        bx_ntvdm_dem_whole_provider_v1_teardown(&provider);
        if (!profile_for(&profile, BX_NTVDM_MUTATION_MODE_V1_OVERLAY) ||
            !bx_ntvdm_dem_cwd_context_v1_initialize(&cwd, &profile) ||
            !bx_ntvdm_dem_whole_provider_v1_initialize(&provider, &profile, &space, &cwd) ||
            !bx_ntvdm_search_request_v1_decode_first_path((const uint8_t *)request, 0u,
                &overlay_query) ||
            swprintf_s(overlay_alpha, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, L"%s\\ALPHA.TXT",
                overlay_query.relative_directory) < 0 ||
            swprintf_s(overlay_beta, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, L"%s\\BETA.TXT",
                overlay_query.relative_directory) < 0 ||
            !bx_ntvdm_dem_overlay_store_v1_tombstone(&provider.overlay_store, drive,
                overlay_alpha) || !bx_ntvdm_dem_overlay_store_v1_put_file(
                &provider.overlay_store, drive, overlay_beta, FILE_ATTRIBUTE_NORMAL,
                (const uint8_t *)"b", 1u)) failed = 10;
    }
    if (!failed) {
        memset(path_first, 0, sizeof(path_first));
        memcpy(path_first, request, strlen(request) + 1u);
        dta_bytes = path_first + 128u;
        dta_bytes[0] = 0x00u; dta_bytes[1] = 0x09u;
        path_first[132] = 0x34u; path_first[133] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x09u, &boundary, &cpu, path_first,
                sizeof(path_first), &transaction, payload, &payload_bytes)) failed = 110;
        else if (payload_bytes != 30u) failed = 111;
        else if (memcmp(payload + 17u, "BETA.TXT", 8u)) failed = 112;
    }
    if (!failed) {
        memcpy(path_next, payload, 8u); path_next[43] = 0x34u; path_next[44] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x0bu, &boundary, &cpu, path_next,
                sizeof(path_next), &transaction, payload, &payload_bytes) ||
            payload_bytes != 30u || memcmp(payload + 17u, "ZETA.TXT", 8u)) failed = 12;
    }
    if (!failed) {
        bx_ntvdm_search_transaction_v1_release(&provider.search);
        bx_ntvdm_search_transaction_v1_initialize(&provider.search);
        memset(fcb_first, 0, sizeof(fcb_first));
        memcpy(fcb_first + 53u, request, strlen(request) + 1u);
        fcb_first[181] = 0x34u; fcb_first[182] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x0au, &boundary, &cpu, fcb_first,
                sizeof(fcb_first), &transaction, payload, &payload_bytes) ||
            payload_bytes != 51u || memcmp(payload, "BETA    TXT", 11u)) failed = 13;
    }
    if (!failed) {
        memset(fcb_next, 0, sizeof(fcb_next));
        memcpy(fcb_next + 32u, payload + 31u, 8u);
        fcb_next[53] = 0x34u; fcb_next[54] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x0cu, &boundary, &cpu, fcb_next,
                sizeof(fcb_next), &transaction, payload, &payload_bytes) ||
            payload_bytes != 51u || memcmp(payload, "ZETA    TXT", 11u)) failed = 14;
    }
    if (!failed) {
        bx_ntvdm_search_query_v1 virtual_query;
        wchar_t virtual_gamma[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
        wchar_t virtual_omega[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
        bx_ntvdm_dem_whole_provider_v1_teardown(&provider);
        if (!profile_for(&profile, BX_NTVDM_MUTATION_MODE_V1_VIRTUAL) ||
            !bx_ntvdm_dem_cwd_context_v1_initialize(&cwd, &profile) ||
            !bx_ntvdm_dem_whole_provider_v1_initialize(&provider, &profile, &space, &cwd) ||
            !bx_ntvdm_search_request_v1_decode_first_path((const uint8_t *)request, 0u,
                &virtual_query) ||
            (virtual_query.relative_directory[0] != L'\0' &&
             !bx_ntvdm_dem_overlay_store_v1_put_directory(&provider.overlay_store, drive,
                virtual_query.relative_directory, FILE_ATTRIBUTE_DIRECTORY)) ||
            swprintf_s(virtual_gamma, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, L"%s\\GAMMA.TXT",
                virtual_query.relative_directory) < 0 ||
            swprintf_s(virtual_omega, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, L"%s\\OMEGA.TXT",
                virtual_query.relative_directory) < 0 ||
            !bx_ntvdm_dem_overlay_store_v1_put_file(&provider.overlay_store, drive,
                virtual_gamma, FILE_ATTRIBUTE_NORMAL, (const uint8_t *)"g", 1u) ||
            !bx_ntvdm_dem_overlay_store_v1_put_file(&provider.overlay_store, drive,
                virtual_omega, FILE_ATTRIBUTE_NORMAL, (const uint8_t *)"o", 1u)) failed = 15;
    }
    if (!failed) {
        memset(path_first, 0, sizeof(path_first));
        memcpy(path_first, request, strlen(request) + 1u);
        dta_bytes = path_first + 128u;
        dta_bytes[0] = 0x00u; dta_bytes[1] = 0x09u;
        path_first[132] = 0x34u; path_first[133] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x09u, &boundary, &cpu, path_first,
                sizeof(path_first), &transaction, payload, &payload_bytes) ||
            payload_bytes != 30u || memcmp(payload + 17u, "GAMMA.TXT", 9u)) failed = 16;
    }
    if (!failed) {
        memcpy(path_next, payload, 8u); path_next[43] = 0x34u; path_next[44] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x0bu, &boundary, &cpu, path_next,
                sizeof(path_next), &transaction, payload, &payload_bytes) ||
            payload_bytes != 30u || memcmp(payload + 17u, "OMEGA.TXT", 9u)) failed = 17;
    }
    if (!failed) {
        bx_ntvdm_search_transaction_v1_release(&provider.search);
        bx_ntvdm_search_transaction_v1_initialize(&provider.search);
        memset(fcb_first, 0, sizeof(fcb_first));
        memcpy(fcb_first + 53u, request, strlen(request) + 1u);
        fcb_first[181] = 0x34u; fcb_first[182] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x0au, &boundary, &cpu, fcb_first,
                sizeof(fcb_first), &transaction, payload, &payload_bytes) ||
            payload_bytes != 51u || memcmp(payload, "GAMMA   TXT", 11u)) failed = 18;
    }
    if (!failed) {
        memset(fcb_next, 0, sizeof(fcb_next));
        memcpy(fcb_next + 32u, payload + 31u, 8u);
        fcb_next[53] = 0x34u; fcb_next[54] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!invoke(&provider, &dta, 0x0cu, &boundary, &cpu, fcb_next,
                sizeof(fcb_next), &transaction, payload, &payload_bytes) ||
            payload_bytes != 51u || memcmp(payload, "OMEGA   TXT", 11u)) failed = 19;
    }
    if (!failed) bx_ntvdm_dem_whole_provider_v1_teardown(&provider);
    if (space.magic != 0u) bx_ntvdm_host_namespace_v1_release(&space);
    DeleteFileW(alpha); DeleteFileW(zeta); RemoveDirectoryW(directory);
    return failed;
}
