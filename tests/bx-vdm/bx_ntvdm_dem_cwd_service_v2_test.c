#include "bx_ntvdm_dem_cwd_service_v2.h"

#include <string.h>

static void event_cpu(bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_state_v1 *cpu)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->vector = 6u; event->fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu->ds = 0x10u; cpu->esi = 0x20u; cpu->edx = 0x20u;
}

static int profile(uint32_t mode, bx_ntvdm_mutation_profile_v1 *value)
{
    bx_ntvdm_mutation_profile_v1_initialize(value, mode);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(value,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu);
}

static int invoke(bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t service,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    const uint8_t *input, uint64_t input_bytes,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t *payload)
{
    bx_ntvdm_guest_gather_read_action_v1 read;
    return bx_ntvdm_dem_cwd_service_v2_prepare(service, event, cpu, &read) &&
        bx_ntvdm_dem_cwd_service_v2_complete(context, host_namespace, service,
            event, cpu, &read, input, input_bytes, transaction, payload);
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 direct_profile, readonly_profile, overlay_profile,
        virtual_profile;
    bx_ntvdm_dem_cwd_context_v1 direct, readonly, overlay, virtual_context;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    uint8_t input[71] = {0}, payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD] = {0};
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    bx_ntvdm_host_namespace_v1 host_namespace;
    uint8_t drive;
    int failed = 0;

    drive = 2u;
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = snapshot.admitted_mask = UINT32_C(1) << drive;
    snapshot.types[drive] = DRIVE_FIXED;
    if (!bx_ntvdm_host_namespace_v1_initialize(&host_namespace, &snapshot) ||
        !profile(BX_NTVDM_MUTATION_MODE_V1_DIRECT, &direct_profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&direct, &direct_profile)) return 2;
    event_cpu(&event, &cpu);
    memcpy(input, "C:\\", 4u);
    if (!invoke(&direct, &host_namespace, 0x18u, &event, &cpu, input, 67u,
            &transaction, payload) || transaction.result.eflags_values != 0u) failed = 1;
    event_cpu(&event, &cpu); cpu.eax = drive;
    memset(input, 0xa5, sizeof(input));
    if (!invoke(&direct, &host_namespace, 0x13u, &event, &cpu, input, 71u,
            &transaction, payload) || transaction.writes.write_count != 2u ||
        memcmp(payload, "C:\\", 4u) != 0 || payload[67] != 2u ||
        payload[68] != 0u) failed = 1;
    event_cpu(&event, &cpu); cpu.edx = drive;
    memcpy(input, "C:\\", 4u);
    if (!invoke(&direct, &host_namespace, 0x1au, &event, &cpu, input, 67u,
            &transaction, payload) || transaction.result.eflags_values != 0u) failed = 1;
    event_cpu(&event, &cpu); cpu.edx = drive + 1u;
    if (!invoke(&direct, &host_namespace, 0x1au, &event, &cpu, input, 67u,
            &transaction, payload) || transaction.result.cpu_delta.gpr16_values[0] != 1u ||
        transaction.result.eflags_values == 0u) failed = 1;
    bx_ntvdm_host_namespace_v1_release(&host_namespace);

    if (!profile(BX_NTVDM_MUTATION_MODE_V1_READONLY, &readonly_profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&readonly, &readonly_profile)) failed = 1;
    event_cpu(&event, &cpu); memcpy(input, "C:\\", 4u);
    if (!invoke(&readonly, 0, 0x18u, &event, &cpu, input, 67u,
            &transaction, payload) || transaction.result.cpu_delta.gpr16_values[0] != 5u ||
        transaction.result.eflags_values == 0u) failed = 1;

    if (!profile(BX_NTVDM_MUTATION_MODE_V1_OVERLAY, &overlay_profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&overlay, &overlay_profile)) failed = 1;
    event_cpu(&event, &cpu); memcpy(input, "C:\\", 4u);
    if (!invoke(&overlay, 0, 0x18u, &event, &cpu, input, 67u,
            &transaction, payload) || transaction.result.eflags_values != 0u) failed = 1;

    /* Virtual is an explicitly selected profile, not a fallback for a
     * missing real C:.  It owns its own CWD state and therefore requires no
     * host namespace lookup during a set/query round trip. */
    if (!profile(BX_NTVDM_MUTATION_MODE_V1_VIRTUAL, &virtual_profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&virtual_context, &virtual_profile))
        failed = 1;
    event_cpu(&event, &cpu); memcpy(input, "C:\\VIRT", sizeof("C:\\VIRT"));
    if (!invoke(&virtual_context, 0, 0x18u, &event, &cpu, input, 67u,
            &transaction, payload) || transaction.result.eflags_values != 0u) failed = 1;
    event_cpu(&event, &cpu); cpu.eax = drive;
    memset(input, 0xa5, sizeof(input));
    if (!invoke(&virtual_context, 0, 0x13u, &event, &cpu, input, 71u,
            &transaction, payload) || transaction.writes.write_count != 2u ||
        memcmp(payload, "C:\\VIRT", 7u) != 0 || payload[67] != 2u ||
        payload[68] != 0u) failed = 1;
    return failed ? 1 : 0;
}
