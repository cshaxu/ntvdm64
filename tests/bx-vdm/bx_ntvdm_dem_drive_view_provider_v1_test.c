#include "bop-v1/bx_ntvdm_dem_drive_view_provider_v1.h"

#include <string.h>
#include <windows.h>

static void event_cpu(bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_state_v1 *cpu)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->vector = 6u;
    event->fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu->ds = 0x10u;
    cpu->edx = 2u;
}

static int profile(uint32_t mode, bx_ntvdm_mutation_profile_v1 *value)
{
    bx_ntvdm_mutation_profile_v1_initialize(value, mode);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(value,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu);
}

static int provider(bx_ntvdm_dem_drive_view_provider_v1 *value,
    const bx_ntvdm_mutation_profile_v1 *profile_value)
{
    return bx_ntvdm_dem_drive_view_provider_v1_initialize(value) &&
        bx_ntvdm_dem_drive_view_provider_v1_set_mutation_profile(value,
            profile_value);
}

static int invoke(bx_ntvdm_dem_drive_view_provider_v1 *value,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t service,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    const uint8_t *input, uint64_t input_bytes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t *payload)
{
    bx_ntvdm_guest_gather_read_action_v1 read;
    return bx_ntvdm_dem_drive_view_provider_v1_prepare_cwd(value, service,
        event, cpu, &read) &&
        bx_ntvdm_dem_drive_view_provider_v1_complete_cwd(value, host_namespace,
            service, event, cpu, &read, input, input_bytes, transaction, payload);
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 direct_profile, readonly_profile,
        overlay_profile, virtual_profile;
    bx_ntvdm_dem_drive_view_provider_v1 direct, readonly, overlay, virtual_view;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    bx_ntvdm_host_namespace_v1 host_namespace;
    bx_ntvdm_guest_gather_read_action_v1 action;
    uint8_t input[67] = { 0 }, payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD] = { 0 };
    wchar_t original[MAX_PATH];
    DWORD original_bytes;
    int failed = 0;

    original_bytes = GetCurrentDirectoryW(MAX_PATH, original);
    if (original_bytes == 0u || original_bytes >= MAX_PATH) return 2;
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = snapshot.admitted_mask = UINT32_C(1) << 2u;
    snapshot.types[2] = DRIVE_FIXED;
    memcpy(input, "C:\\", 4u);
    if (!bx_ntvdm_host_namespace_v1_initialize(&host_namespace, &snapshot) ||
        !profile(BX_NTVDM_MUTATION_MODE_V1_DIRECT, &direct_profile) ||
        !provider(&direct, &direct_profile)) failed = 1;
    event_cpu(&event, &cpu);
    if (!failed && (!invoke(&direct, &host_namespace, 0x1au, &event, &cpu,
            input, sizeof(input), &transaction, payload) ||
        transaction.result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        transaction.result.eflags_values != 0u)) failed = 1;
    if (!SetCurrentDirectoryW(original)) failed = 1;
    bx_ntvdm_host_namespace_v1_release(&host_namespace);

    if (!profile(BX_NTVDM_MUTATION_MODE_V1_READONLY, &readonly_profile) ||
        !provider(&readonly, &readonly_profile)) failed = 1;
    event_cpu(&event, &cpu);
    if (!failed && (!invoke(&readonly, 0, 0x1au, &event, &cpu, input,
            sizeof(input), &transaction, payload) ||
        transaction.result.cpu_delta.gpr16_values[0] != 5u ||
        transaction.result.eflags_values == 0u)) failed = 1;

    if (!profile(BX_NTVDM_MUTATION_MODE_V1_OVERLAY, &overlay_profile) ||
        !provider(&overlay, &overlay_profile)) failed = 1;
    event_cpu(&event, &cpu);
    if (!failed && bx_ntvdm_dem_drive_view_provider_v1_prepare_cwd(&overlay,
            0x1au, &event, &cpu, &action)) failed = 1;
    if (!profile(BX_NTVDM_MUTATION_MODE_V1_VIRTUAL, &virtual_profile) ||
        !provider(&virtual_view, &virtual_profile)) failed = 1;
    event_cpu(&event, &cpu);
    if (!failed && bx_ntvdm_dem_drive_view_provider_v1_prepare_cwd(&virtual_view,
            0x1au, &event, &cpu, &action)) failed = 1;
    return failed ? 1 : 0;
}