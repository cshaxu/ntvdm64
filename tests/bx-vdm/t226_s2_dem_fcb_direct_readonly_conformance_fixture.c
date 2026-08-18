#include "bx_ntvdm_dem_fcb_handle_partition_v1.h"
#include "bx_ntvdm_dem_fcb_wildcard_partition_v1.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

static int profile_for(uint32_t mode, bx_ntvdm_mutation_profile_v1 *profile)
{
    bx_ntvdm_mutation_profile_v1_initialize(profile, mode);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, 0x0fu);
}

static void boundary_initialize(bx_ntvdm_exception_event_v1 *boundary)
{
    memset(boundary, 0, sizeof(*boundary));
    boundary->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    boundary->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    boundary->struct_bytes = sizeof(*boundary);
    boundary->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    boundary->vector = 6u;
    boundary->fault_rip = 0x100u;
}

static uint32_t token_of(const bx_ntvdm_cpu_result_v2 *result)
{
    return ((uint32_t)result->cpu_delta.gpr16_values[0] << 16u) |
        result->cpu_delta.gpr16_values[5];
}

static int cf_set(const bx_ntvdm_cpu_result_v2 *result)
{
    return (result->eflags_write_mask & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u &&
        (result->eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u;
}

int main(void)
{
    wchar_t temporary[MAX_PATH], short_name[MAX_PATH];
    char oem_path[MAX_PATH], oem_pattern[MAX_PATH];
    bx_ntvdm_host_drive_snapshot_v1 drives = {0};
    bx_ntvdm_host_namespace_v1 host = {0};
    bx_ntvdm_mutation_profile_v1 direct_profile, readonly_profile;
    bx_ntvdm_dem_cwd_context_v1 direct_cwd, readonly_cwd;
    bx_ntvdm_dem_whole_provider_v1 *direct = 0, *readonly = 0;
    bx_ntvdm_exception_event_v1 boundary;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    uint8_t drive;
    uint32_t bytes = 0u, token;
    int status = 1;

    if (!GetTempPathW(MAX_PATH, temporary) ||
        !GetTempFileNameW(temporary, L"n64", 0u, temporary) ||
        !GetShortPathNameW(temporary, short_name, MAX_PATH) || short_name[1] != L':') goto done;
    drive = (uint8_t)(towupper((wint_t)short_name[0]) - L'A');
    if (!WideCharToMultiByte(CP_OEMCP, 0, short_name, -1, oem_path,
            (int)sizeof(oem_path), 0, 0) ||
        sprintf_s(oem_pattern, sizeof(oem_pattern), "%c:\\*.N64", (char)('A' + drive)) < 0)
        goto done;
    drives.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    drives.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    drives.present_mask = drives.admitted_mask = UINT32_C(1) << drive;
    drives.types[drive] = DRIVE_FIXED;
    if (!bx_ntvdm_host_namespace_v1_initialize(&host, &drives) ||
        !profile_for(BX_NTVDM_MUTATION_MODE_V1_DIRECT, &direct_profile) ||
        !profile_for(BX_NTVDM_MUTATION_MODE_V1_READONLY, &readonly_profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&direct_cwd, &direct_profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&readonly_cwd, &readonly_profile)) goto done;
    direct = calloc(1u, sizeof(*direct));
    readonly = calloc(1u, sizeof(*readonly));
    if (!direct || !readonly ||
        !bx_ntvdm_dem_whole_provider_v1_initialize(direct, &direct_profile, &host, &direct_cwd) ||
        !bx_ntvdm_dem_whole_provider_v1_initialize(readonly, &readonly_profile, &host, &readonly_cwd)) goto done;
    boundary_initialize(&boundary);

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0u;
    if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(direct, 0x2du, &boundary,
            &cpu, oem_path, 0, 0u, &bytes, &result) || cf_set(&result) ||
        (token = token_of(&result)) == 0u) goto done;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = token >> 16u; cpu.esi = token & 0xffffu;
    if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(direct, 0x2eu, &boundary,
            &cpu, 0, 0, 0u, &bytes, &result) || cf_set(&result)) goto done;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0u;
    if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(readonly, 0x2du, &boundary,
            &cpu, oem_path, 0, 0u, &bytes, &result) || cf_set(&result) ||
        (token = token_of(&result)) == 0u) goto done;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = token >> 16u; cpu.esi = token & 0xffffu;
    if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(readonly, 0x2eu, &boundary,
            &cpu, 0, 0, 0u, &bytes, &result) || cf_set(&result)) goto done;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    if (!bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(readonly, 0x2cu, &boundary,
            &cpu, oem_path, 0, 0u, &bytes, &result) || !cf_set(&result) ||
        result.cpu_delta.gpr16_values[0] != 5u) goto done;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    if (!bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(readonly, 0x07u, &boundary,
            &cpu, oem_pattern, 0, &result) || !cf_set(&result) ||
        result.cpu_delta.gpr16_values[0] != 5u) goto done;
    status = 0;
done:
    if (direct) { bx_ntvdm_dem_whole_provider_v1_teardown(direct); free(direct); }
    if (readonly) { bx_ntvdm_dem_whole_provider_v1_teardown(readonly); free(readonly); }
    bx_ntvdm_host_namespace_v1_release(&host);
    if (temporary[0]) DeleteFileW(temporary);
    return status;
}