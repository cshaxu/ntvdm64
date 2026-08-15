#include "bx_ntvdm_dem_cwd_service_v2.h"
#include "bx_ntvdm_cmd_current_dir_service.h"
#include "bx_ntvdm_cmd_comspec_bootstrap_service.h"

#include <string.h>
#include <windows.h>

static void event_cpu(bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_state_v1 *cpu, uint8_t selector, uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->vector = 6u; event->fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu->ds = 0x10u; cpu->esi = 0x20u; cpu->edx = 0x20u;
    (void)selector; (void)service;
}

static int profile(bx_ntvdm_mutation_profile_v1 *value)
{
    bx_ntvdm_mutation_profile_v1_initialize(value, BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(value,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu);
}

static int cwd(bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *space, uint8_t service,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    const uint8_t *input, uint64_t bytes, bx_ntvdm_multi_write_transaction_v1 *tx,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    bx_ntvdm_guest_gather_read_action_v1 gather;
    return bx_ntvdm_dem_cwd_service_v2_prepare(service, event, cpu, &gather) &&
        bx_ntvdm_dem_cwd_service_v2_complete(context, space, service, event, cpu,
            &gather, input, bytes, tx, payload);
}

int main(void)
{
    wchar_t original[MAX_PATH];
    uint8_t drive, root[67] = {0}, input[67] = {0}, payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD] = {0};
    uint8_t environment[] = { 'F','O','O','=', 'B','A','R', 0u, 0u };
    uint8_t comspec[BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES] = {0};
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    bx_ntvdm_host_namespace_v1 space;
    bx_ntvdm_mutation_profile_v1 mutation_profile;
    bx_ntvdm_dem_cwd_context_v1 context;
    bx_ntvdm_command_host_context_v1 projection;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_multi_write_transaction_v1 tx;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cmd_comspec_bootstrap_v1 bootstrap;
    int failed = 0;

    if (GetCurrentDirectoryW(MAX_PATH, original) == 0u || original[0] < L'A' ||
        original[0] > L'Z' || original[1] != L':') return 2;
    drive = (uint8_t)(original[0] - L'A');
    root[0] = (uint8_t)original[0]; root[1] = ':'; root[2] = '\\'; root[3] = 0u;
    memcpy(input, root, 4u);
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = snapshot.admitted_mask = UINT32_C(1) << drive;
    snapshot.types[drive] = DRIVE_FIXED;
    if (!bx_ntvdm_host_namespace_v1_initialize(&space, &snapshot) ||
        !profile(&mutation_profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&context, &mutation_profile)) return 3;

    event_cpu(&event, &cpu, 0x50u, 0x18u);
    if (!cwd(&context, &space, 0x18u, &event, &cpu, input, 67u, &tx, payload) ||
        tx.result.eflags_values != 0u) failed = 1;
    event_cpu(&event, &cpu, 0x50u, 0x1au); cpu.edx = drive;
    if (!cwd(&context, &space, 0x1au, &event, &cpu, input, 67u, &tx, payload) ||
        tx.result.eflags_values != 0u || tx.result.cpu_delta.gpr16_write_mask != 0u) failed = 1;
    event_cpu(&event, &cpu, 0x50u, 0x13u); cpu.eax = drive;
    memset(input, 0xa5, sizeof(input));
    if (!cwd(&context, &space, 0x13u, &event, &cpu, input, 71u, &tx, payload) ||
        tx.result.eflags_values != 0u || tx.writes.write_count != 2u ||
        memcmp(payload, root, 4u) != 0 || payload[67] != 2u || payload[68] != 0u) failed = 1;
    if (!bx_ntvdm_session_host_context_v1_set_environment(&context, environment,
            (uint32_t)sizeof(environment)) ||
        !bx_ntvdm_session_host_context_v1_project_command(&context, &projection) ||
        projection.selected_drive != drive || memcmp(projection.selected_directory, root, 4u) != 0 ||
        projection.environment_bytes != sizeof(environment)) failed = 1;

    event_cpu(&event, &cpu, 0x54u, 0x04u); cpu.eax = drive;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u,0xc4u,0x54u,0x04u}, 4u);
    if (!bx_ntvdm_cmd_current_dir_service_v1_prepare(UINT32_C(1) << drive,
            &projection, &event, &cpu, &window, &tx, payload) ||
        tx.writes.write_count != 1u || memcmp(payload, root, 4u) != 0 ||
        tx.result.eflags_values != 0u) failed = 1;

    memcpy(comspec, root, 3u); memcpy(comspec + 3u, "COMMAND.COM", 12u);
    bx_ntvdm_cmd_comspec_bootstrap_v1_initialize(&bootstrap);
    event_cpu(&event, &cpu, 0x54u, 0x02u);
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u,0xc4u,0x54u,0x02u}, 4u);
    {
        bx_ntvdm_guest_gather_read_action_v1 gather;
        if (!bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_comspec(&event, &cpu, &window,
                &bootstrap, &gather) || !bx_ntvdm_cmd_comspec_bootstrap_v1_complete_comspec(
                &event, &cpu, &gather, comspec, sizeof(comspec), &projection,
                &bootstrap, &tx.result) || bootstrap.environment_bytes < sizeof(environment) ||
            memcmp(bootstrap.environment, "COMSPEC=", 8u) != 0) failed = 1;
    }
    event_cpu(&event, &cpu, 0x54u, 0x0fu); cpu.es = 0x200u; cpu.ebx = 0x100u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u,0xc4u,0x54u,0x0fu}, 4u);
    if (!bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_environment(&event, &cpu, &window,
            &bootstrap, &tx, payload) || tx.writes.write_count != 1u ||
        tx.writes.writes[0].guest_physical_address != 0x2000u ||
        !bx_ntvdm_cmd_comspec_bootstrap_v1_complete_environment(&bootstrap, &tx)) failed = 1;

    bx_ntvdm_host_namespace_v1_release(&space);
    if (!SetCurrentDirectoryW(original)) return 4;
    return failed ? 1 : 0;
}
