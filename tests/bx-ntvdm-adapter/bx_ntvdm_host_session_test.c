#include <stdint.h>

#include "bx_ntvdm_host_session.h"

#include <string.h>

int main(void)
{
    static const uint8_t ntio[] = { 1u };
    static const uint8_t ntdos[] = { 0xe9u, 0xa9u, 0x51u };
    static const uint8_t command[] = { 2u };
    const uint8_t code[] = { 0xc4u, 0xc4u, 0x50u, 0x11u, 0xf4u };
    bx_ntvdm_session_resources_v1 resources;
    bx_ntvdm_host_service_payloads_v1 payloads;
    bx_ntvdm_host_session_v1 session;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_exception_result_v1 result;
    bx_ntvdm_host_transaction_v1 transaction;
    bx_ntvdm_observation_transaction_v1 observation;
    bx_ntvdm_startup_snapshot_range_v1 snapshot_ranges[2] = {
        { 21u, 0u, { 0x40u, 4u } }, { 22u, 0u, { 0x714u, 4u } }
    };
    bx_ntvdm_startup_snapshot_transaction_v1 snapshot;
    bx_ntvdm_startup_plan_v1 startup_plan;
    bx_ntvdm_cpu_state_v1 startup_entry;
    const uint8_t *startup_payload;
    uint64_t startup_payload_bytes;
    bx_ntvdm_guest_range range = { 0x700u, 4u };
    bx_ntvdm_cpu_state_v1 mismatch;
    bx_ntvdm_guest_read_action_v1 read_action;
    bx_ntvdm_guest_gather_read_action_v1 gather_action;
    bx_ntvdm_guest_range gather_ranges[2] = {{0x740u, 128u}, {0x800u, 2u}};
    bx_ntvdm_cpu_result_v2 read_result;
    bx_ntvdm_multi_write_transaction_v1 multi_write;
    bx_ntvdm_bulk_result_transaction_v1 bulk_result;
    bx_ntvdm_cpu_result_v2 bulk_cpu_result;
    uint8_t bulk_payload[] = { 0x31u, 0x32u, 0x33u };
    uint8_t multi_write_payload[] = { 0x41u, 0x42u, 0x43u };
    const uint8_t *payload = 0;
    uint8_t observation_output[4] = { 0u };
    uint8_t snapshot_output[8] = { 0u };
    uint8_t *observation_bytes = 0;
    uint32_t consumer = BX_NTVDM_HOST_GUEST_READ_NONE;
    uint64_t payload_bytes = 0u;
    uint64_t resume_rip = 0u;

    bx_ntvdm_session_resources_v1_initialize(&resources);
    resources.resources[0].kind = BX_NTVDM_SESSION_RESOURCE_NTIO;
    resources.resources[0].byte_count = sizeof(ntio);
    resources.resources[1].kind = BX_NTVDM_SESSION_RESOURCE_NTDOS;
    resources.resources[1].byte_count = sizeof(ntdos);
    resources.resources[2].kind = BX_NTVDM_SESSION_RESOURCE_COMMAND;
    resources.resources[2].byte_count = sizeof(command);
    payloads.ntio_bytes = ntio; payloads.ntio_byte_count = sizeof(ntio);
    payloads.ntdos_bytes = ntdos; payloads.ntdos_byte_count = sizeof(ntdos);
    payloads.command_bytes = command; payloads.command_byte_count = sizeof(command);
    if (!bx_ntvdm_host_session_v1_initialize(&session, &resources, &payloads,
        0x100000u)) return 1;
    bx_ntvdm_cpu_state_v1_initialize(&startup_entry,
        BX_NTVDM_CPU_EXECUTION_REAL);
    startup_entry.cs = 0x70u;
    if (!bx_ntvdm_host_session_v1_prepare_startup_plan(&session,
        &startup_entry, 0x700u, 0x714u, 4u, &startup_plan, &startup_payload,
        &startup_payload_bytes) || startup_payload != ntio ||
        startup_payload_bytes != sizeof(ntio) ||
        startup_plan.payload_write.guest_physical_address != 0x700u)
        return 18;
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event); event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.cpu_id = 0u; event.vector = 6u; event.error_code = 0u;
    event.reserved0 = 0u; event.fault_rip = 0x7c00u;
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    state.edi = 0x08b0u;
    bx_ntvdm_instruction_window_v1_capture(&window, code, sizeof(code));
    if (!bx_ntvdm_host_session_v1_dispatch(&session, &event, &state, &window,
        &result) || result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
        result.resume_rip != 0x7c04u) return 2;
    mismatch = state;
    mismatch.eax = 1u;
    if (bx_ntvdm_host_session_v1_take_pending_write(&session, &event, &mismatch,
        &transaction, &payload, &payload_bytes, &resume_rip)) return 3;
    if (!bx_ntvdm_host_session_v1_dispatch(&session, &event, &state, &window,
        &result) || result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME) return 4;
    if (!bx_ntvdm_host_session_v1_take_pending_write(&session, &event, &state,
        &transaction, &payload,
        &payload_bytes, &resume_rip) || payload != ntdos ||
        payload_bytes != sizeof(ntdos) || resume_rip != 0x7c04u ||
        transaction.guest_write.guest_physical_address != 0x8b00u) return 5;
    if (bx_ntvdm_host_session_v1_take_pending_write(&session, &event, &state,
        &transaction, &payload,
        &payload_bytes, &resume_rip)) return 6;
    bx_ntvdm_observation_transaction_v1_initialize(&observation, &event, &state,
        &range);
    if (!bx_ntvdm_host_session_v1_queue_observation(&session, &observation,
        observation_output, sizeof(observation_output))) return 7;
    if (bx_ntvdm_host_session_v1_take_pending_write(&session, &event, &state,
        &transaction, &payload, &payload_bytes, &resume_rip)) return 8;
    mismatch = state;
    mismatch.eax = 1u;
    if (bx_ntvdm_host_session_v1_take_pending_observation(&session, &event,
        &mismatch, &observation, &observation_bytes, &payload_bytes)) return 9;
    if (!bx_ntvdm_host_session_v1_queue_observation(&session, &observation,
        observation_output, sizeof(observation_output)) ||
        !bx_ntvdm_host_session_v1_take_pending_observation(&session, &event,
        &state, &observation, &observation_bytes, &payload_bytes) ||
        observation_bytes != observation_output || payload_bytes != sizeof(observation_output) ||
        observation.guest_read.address != range.address) return 10;
    if (bx_ntvdm_host_session_v1_take_pending_observation(&session, &event,
        &state, &observation, &observation_bytes, &payload_bytes)) return 11;
    bx_ntvdm_startup_snapshot_transaction_v1_initialize(&snapshot, &event,
        &state, snapshot_ranges, 2u);
    if (!bx_ntvdm_host_session_v1_queue_startup_snapshot(&session, &snapshot,
        snapshot_output, sizeof(snapshot_output))) return 12;
    if (bx_ntvdm_host_session_v1_take_pending_observation(&session, &event,
        &state, &observation, &observation_bytes, &payload_bytes)) return 13;
    mismatch = state;
    mismatch.eax = 1u;
    if (bx_ntvdm_host_session_v1_take_pending_startup_snapshot(&session,
        &event, &mismatch, &snapshot, &observation_bytes, &payload_bytes)) return 14;
    if (!bx_ntvdm_host_session_v1_queue_startup_snapshot(&session, &snapshot,
        snapshot_output, sizeof(snapshot_output)) ||
        !bx_ntvdm_host_session_v1_take_pending_startup_snapshot(&session,
        &event, &state, &snapshot, &observation_bytes, &payload_bytes) ||
        observation_bytes != snapshot_output || payload_bytes != sizeof(snapshot_output) ||
        snapshot.range_count != 2u) return 15;
    if (bx_ntvdm_host_session_v1_take_pending_startup_snapshot(&session,
        &event, &state, &snapshot, &observation_bytes, &payload_bytes)) return 16;
    if (!bx_ntvdm_guest_read_action_v1_need_read(&read_action, 0x740u, 128u) ||
        bx_ntvdm_host_session_v1_queue_guest_read(&session, &event, &state,
            &read_action, BX_NTVDM_HOST_GUEST_READ_NONE) ||
        !bx_ntvdm_host_session_v1_queue_guest_read(&session, &event, &state,
            &read_action, BX_NTVDM_HOST_GUEST_READ_DEM_DTA) ||
        !bx_ntvdm_host_session_v1_pending_guest_read_consumer(&session,
            &event, &state, &consumer) ||
        consumer != BX_NTVDM_HOST_GUEST_READ_DEM_DTA ||
        !bx_ntvdm_host_session_v1_take_pending_guest_read(
            &session, &event, &state, &read_action, &observation_bytes) ||
        observation_bytes == 0) return 19;
    bx_ntvdm_cpu_result_v2_pass_through(&read_result);
    mismatch = state; mismatch.eax = 1u;
    if (bx_ntvdm_host_session_v1_complete_guest_read(&session, &event,
        &mismatch, &read_result)) return 20;
    if (!bx_ntvdm_host_session_v1_queue_guest_read(&session, &event, &state,
        &read_action, BX_NTVDM_HOST_GUEST_READ_DEM_DTA) || !bx_ntvdm_cpu_result_v2_resume(&read_result, 0x7c04u) ||
        !bx_ntvdm_host_session_v1_complete_guest_read(&session, &event, &state,
            &read_result)) return 21;
    if (!bx_ntvdm_guest_gather_read_action_v1_need_read(&gather_action,
            gather_ranges, 2u) ||
        !bx_ntvdm_host_session_v1_queue_guest_gather_read(&session, &event,
            &state, &gather_action) ||
        !bx_ntvdm_host_session_v1_take_pending_guest_gather_read(&session,
            &event, &state, &gather_action, &observation_bytes) ||
        observation_bytes == 0 || gather_action.total_bytes != 130u) return 26;
    mismatch = state; mismatch.eax = 1u;
    if (bx_ntvdm_host_session_v1_complete_guest_gather_read(&session, &event,
        &mismatch, &read_result)) return 27;
    if (!bx_ntvdm_host_session_v1_queue_guest_gather_read(&session, &event,
            &state, &gather_action) || !bx_ntvdm_cpu_result_v2_resume(
            &read_result, 0x7c04u) ||
        !bx_ntvdm_host_session_v1_complete_guest_gather_read(&session, &event,
            &state, &read_result)) return 28;
    bx_ntvdm_multi_write_transaction_v1_initialize(&multi_write, &event,
        &state);
    if (!bx_ntvdm_multi_write_v1_add(&multi_write.writes, 0x800u,
        sizeof(multi_write_payload), 0u) ||
        !bx_ntvdm_cpu_result_v2_resume(&multi_write.result, 0x7c04u) ||
        !bx_ntvdm_host_session_v1_queue_multi_write(&session, &multi_write,
            multi_write_payload, sizeof(multi_write_payload))) return 22;
    mismatch = state;
    mismatch.eax = 1u;
    if (bx_ntvdm_host_session_v1_take_pending_multi_write(&session, &event,
        &mismatch, &multi_write, &payload, &payload_bytes)) return 23;
    if (!bx_ntvdm_host_session_v1_queue_multi_write(&session, &multi_write,
        multi_write_payload, sizeof(multi_write_payload)) ||
        !bx_ntvdm_host_session_v1_take_pending_multi_write(&session, &event,
        &state, &multi_write, &payload, &payload_bytes) ||
        payload != multi_write_payload ||
        payload_bytes != sizeof(multi_write_payload) ||
        multi_write.writes.write_count != 1u) return 24;
    if (bx_ntvdm_host_session_v1_take_pending_multi_write(&session, &event,
        &state, &multi_write, &payload, &payload_bytes)) return 25;
    if (!bx_ntvdm_cpu_result_v2_resume(&bulk_cpu_result, 0x7c04u)) return 29;
    bx_ntvdm_bulk_result_transaction_v1_initialize(&bulk_result, &event,
        &state);
    if (!bx_ntvdm_bulk_result_transaction_v1_prepare(&bulk_result, 0x880u,
            sizeof(bulk_payload), &bulk_cpu_result) ||
        !bx_ntvdm_host_session_v1_queue_bulk_result(&session, &bulk_result,
            bulk_payload, sizeof(bulk_payload))) return 30;
    mismatch = state;
    mismatch.eax = 1u;
    if (bx_ntvdm_host_session_v1_take_pending_bulk_result(&session, &event,
        &mismatch, &bulk_result, &payload, &payload_bytes)) return 31;
    if (!bx_ntvdm_host_session_v1_queue_bulk_result(&session, &bulk_result,
            bulk_payload, sizeof(bulk_payload)) ||
        !bx_ntvdm_host_session_v1_take_pending_bulk_result(&session, &event,
            &state, &bulk_result, &payload, &payload_bytes) ||
        payload == bulk_payload || payload_bytes != sizeof(bulk_payload) ||
        memcmp(payload, bulk_payload, sizeof(bulk_payload)) != 0) return 32;
    bx_ntvdm_bulk_result_transaction_v1_initialize(&bulk_result, &event,
        &state);
    if (!bx_ntvdm_bulk_result_transaction_v1_prepare(&bulk_result, 0u, 0u,
            &bulk_cpu_result) || !bx_ntvdm_host_session_v1_queue_bulk_result(
            &session, &bulk_result, 0, 0u) ||
        !bx_ntvdm_host_session_v1_take_pending_bulk_result(&session, &event,
            &state, &bulk_result, &payload, &payload_bytes) ||
        payload_bytes != 0u) return 33;
    event.vector = 13u;
    if (!bx_ntvdm_host_session_v1_dispatch(&session, &event, &state, &window,
        &result) || result.disposition != BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH)
        return 17;
    return 0;
}
