#include "bx_ntvdm_host_session.h"

#include <limits.h>
#include <string.h>

static int bx_ntvdm_host_session_payloads_valid(
    const bx_ntvdm_session_resources_v1 *resources,
    const bx_ntvdm_host_service_payloads_v1 *payloads)
{
    return resources != 0 && payloads != 0 &&
        payloads->ntio_bytes != 0 &&
        payloads->ntio_byte_count == resources->resources[0].byte_count &&
        payloads->ntdos_bytes != 0 &&
        payloads->ntdos_byte_count == resources->resources[1].byte_count &&
        payloads->command_bytes != 0 &&
        payloads->command_byte_count == resources->resources[2].byte_count;
}

static void bx_ntvdm_host_session_clear_pending(
    bx_ntvdm_host_session_v1 *session)
{
    if (session == 0) return;
    session->pending_kind = BX_NTVDM_HOST_PENDING_NONE;
    memset(&session->pending_boundary, 0, sizeof(session->pending_boundary));
    memset(&session->pending_cpu_before, 0, sizeof(session->pending_cpu_before));
    memset(&session->pending_transaction, 0, sizeof(session->pending_transaction));
    session->pending_payload = 0;
    session->pending_payload_bytes = 0u;
    session->pending_resume_rip = 0u;
    memset(&session->pending_observation, 0, sizeof(session->pending_observation));
    memset(&session->pending_startup_snapshot, 0,
        sizeof(session->pending_startup_snapshot));
    memset(&session->pending_guest_read, 0, sizeof(session->pending_guest_read));
    session->pending_guest_read_consumer = BX_NTVDM_HOST_GUEST_READ_NONE;
    memset(session->pending_guest_read_bytes, 0,
        sizeof(session->pending_guest_read_bytes));
    memset(&session->pending_guest_gather_read, 0,
        sizeof(session->pending_guest_gather_read));
    memset(session->pending_guest_gather_read_bytes, 0,
        sizeof(session->pending_guest_gather_read_bytes));
    memset(&session->pending_multi_write, 0, sizeof(session->pending_multi_write));
    memset(&session->pending_bulk_result, 0, sizeof(session->pending_bulk_result));
    /* A successful bulk take returns this same-island arena to the Bochs
     * caller.  Clearing its metadata must not invalidate the returned bytes;
     * the next queue overwrites the declared span, and no pending record can
     * expose a cancelled span. */
    session->pending_output = 0;
    session->pending_output_bytes = 0u;
}

static int bx_ntvdm_host_session_boundary_matches(
    const bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before)
{
    return session != 0 && boundary != 0 && cpu_before != 0 &&
        bx_ntvdm_exception_event_v1_valid(boundary) &&
        bx_ntvdm_cpu_state_v1_valid(cpu_before) &&
        memcmp(&session->pending_boundary, boundary, sizeof(*boundary)) == 0 &&
        memcmp(&session->pending_cpu_before, cpu_before, sizeof(*cpu_before)) == 0;
}

int bx_ntvdm_host_session_v1_initialize(bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_session_resources_v1 *resources,
    const bx_ntvdm_host_service_payloads_v1 *payloads,
    uint64_t aperture_bytes)
{
    if (session == 0 || !bx_ntvdm_session_resources_v1_valid(resources) ||
        !bx_ntvdm_host_session_payloads_valid(resources, payloads) ||
        aperture_bytes == 0u) return 0;
    memset(session, 0, sizeof(*session));
    session->magic = BX_NTVDM_HOST_SESSION_MAGIC;
    session->version = BX_NTVDM_HOST_SESSION_VERSION;
    session->resources = *resources;
    session->payloads = *payloads;
    session->aperture_bytes = aperture_bytes;
    return 1;
}

int bx_ntvdm_host_session_v1_dispatch(bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result)
{
    bx_ntvdm_host_service_request_v1 request;
    bx_ntvdm_host_service_plane_result_v1 prepared;
    bx_ntvdm_host_transaction_v1 transaction;

    if (result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window)) return 0;
    bx_ntvdm_exception_result_v1_pass_through(result);
    if (session == 0 || session->magic != BX_NTVDM_HOST_SESSION_MAGIC ||
        session->version != BX_NTVDM_HOST_SESSION_VERSION ||
        !bx_ntvdm_session_resources_v1_valid(&session->resources) ||
        !bx_ntvdm_host_session_payloads_valid(&session->resources,
            &session->payloads) || session->aperture_bytes == 0u)
        return 1;
    bx_ntvdm_host_session_clear_pending(session);
    if (session->write_consumed != 0u) return 1;
    if (event->vector != 6u ||
        !bx_ntvdm_host_service_catalog_v1_lookup(window, &request)) return 1;
    if (event->fault_rip > UINT64_MAX - 4u) return 1;
    prepared = bx_ntvdm_host_service_plane_v1_prepare(&request, cpu_before,
        &session->resources, &session->payloads, session->aperture_bytes,
        &transaction);
    if (prepared != BX_NTVDM_HOST_SERVICE_PLANE_READY) return 1;
    session->pending_kind = BX_NTVDM_HOST_PENDING_WRITE;
    session->pending_boundary = *event;
    session->pending_cpu_before = *cpu_before;
    session->pending_transaction = transaction;
    session->pending_payload = session->payloads.ntdos_bytes;
    session->pending_payload_bytes = session->payloads.ntdos_byte_count;
    session->pending_resume_rip = event->fault_rip + 4u;
    return bx_ntvdm_exception_result_v1_resume(result,
        session->pending_resume_rip);
}

int bx_ntvdm_host_session_v1_take_pending_write(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_host_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes,
    uint64_t *resume_rip)
{
    if (session == 0 || transaction == 0 || payload == 0 ||
        payload_bytes == 0 || resume_rip == 0 ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_WRITE ||
        !bx_ntvdm_host_session_boundary_matches(session, boundary, cpu_before) ||
        !bx_ntvdm_host_transaction_v1_preflight(&session->pending_transaction,
            session->aperture_bytes, session->pending_payload_bytes) ||
        session->pending_payload == 0 || session->pending_payload_bytes == 0u) {
        if (session != 0 && session->pending_kind == BX_NTVDM_HOST_PENDING_WRITE)
            bx_ntvdm_host_session_clear_pending(session);
        return 0;
    }
    *transaction = session->pending_transaction;
    *payload = session->pending_payload;
    *payload_bytes = session->pending_payload_bytes;
    *resume_rip = session->pending_resume_rip;
    session->write_consumed = 1u;
    bx_ntvdm_host_session_clear_pending(session);
    return 1;
}

int bx_ntvdm_host_session_v1_queue_observation(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_observation_transaction_v1 *transaction,
    uint8_t *output, uint64_t output_bytes)
{
    if (session == 0 || transaction == 0 || output == 0 ||
        session->magic != BX_NTVDM_HOST_SESSION_MAGIC ||
        session->version != BX_NTVDM_HOST_SESSION_VERSION ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_NONE ||
        !bx_ntvdm_observation_transaction_v1_preflight(transaction,
            session->aperture_bytes, output_bytes)) return 0;
    session->pending_kind = BX_NTVDM_HOST_PENDING_OBSERVATION;
    session->pending_boundary = transaction->boundary;
    session->pending_cpu_before = transaction->cpu_before;
    session->pending_observation = *transaction;
    session->pending_output = output;
    session->pending_output_bytes = output_bytes;
    return 1;
}

int bx_ntvdm_host_session_v1_take_pending_observation(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_observation_transaction_v1 *transaction,
    uint8_t **output, uint64_t *output_bytes)
{
    if (session == 0 || transaction == 0 || output == 0 || output_bytes == 0 ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_OBSERVATION ||
        !bx_ntvdm_host_session_boundary_matches(session, boundary, cpu_before) ||
        !bx_ntvdm_observation_transaction_v1_preflight(&session->pending_observation,
            session->aperture_bytes, session->pending_output_bytes) ||
        session->pending_output == 0) {
        if (session != 0 && session->pending_kind == BX_NTVDM_HOST_PENDING_OBSERVATION)
            bx_ntvdm_host_session_clear_pending(session);
        return 0;
    }
    *transaction = session->pending_observation;
    *output = session->pending_output;
    *output_bytes = session->pending_output_bytes;
    bx_ntvdm_host_session_clear_pending(session);
    return 1;
}

int bx_ntvdm_host_session_v1_queue_startup_snapshot(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    uint8_t *output, uint64_t output_bytes)
{
    if (session == 0 || transaction == 0 || output == 0 ||
        session->magic != BX_NTVDM_HOST_SESSION_MAGIC ||
        session->version != BX_NTVDM_HOST_SESSION_VERSION ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_NONE ||
        !bx_ntvdm_startup_snapshot_transaction_v1_preflight(transaction,
            session->aperture_bytes, output_bytes)) return 0;
    session->pending_kind = BX_NTVDM_HOST_PENDING_STARTUP_SNAPSHOT;
    session->pending_boundary = transaction->boundary;
    session->pending_cpu_before = transaction->cpu_before;
    session->pending_startup_snapshot = *transaction;
    session->pending_output = output;
    session->pending_output_bytes = output_bytes;
    return 1;
}

int bx_ntvdm_host_session_v1_take_pending_startup_snapshot(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    uint8_t **output, uint64_t *output_bytes)
{
    if (session == 0 || transaction == 0 || output == 0 || output_bytes == 0 ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_STARTUP_SNAPSHOT ||
        !bx_ntvdm_host_session_boundary_matches(session, boundary, cpu_before) ||
        !bx_ntvdm_startup_snapshot_transaction_v1_preflight(
            &session->pending_startup_snapshot, session->aperture_bytes,
            session->pending_output_bytes) || session->pending_output == 0) {
        if (session != 0 &&
            session->pending_kind == BX_NTVDM_HOST_PENDING_STARTUP_SNAPSHOT)
            bx_ntvdm_host_session_clear_pending(session);
        return 0;
    }
    *transaction = session->pending_startup_snapshot;
    *output = session->pending_output;
    *output_bytes = session->pending_output_bytes;
    bx_ntvdm_host_session_clear_pending(session);
    return 1;
}

int bx_ntvdm_host_session_v1_queue_guest_read(bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_read_action_v1 *action, uint32_t consumer)
{
    if (session == 0 || boundary == 0 || cpu_before == 0 || action == 0 ||
        session->magic != BX_NTVDM_HOST_SESSION_MAGIC ||
        session->version != BX_NTVDM_HOST_SESSION_VERSION ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_NONE ||
        !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_guest_read_action_v1_valid(action) ||
        (consumer != BX_NTVDM_HOST_GUEST_READ_DEM_DTA &&
         consumer != BX_NTVDM_HOST_GUEST_READ_SEARCH &&
         consumer != BX_NTVDM_HOST_GUEST_READ_DEM_FILE_OPEN) ||
        action->disposition != BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ ||
        !bx_ntvdm_guest_range_within(session->aperture_bytes,
            &action->guest_read)) return 0;
    session->pending_kind = BX_NTVDM_HOST_PENDING_GUEST_READ;
    session->pending_boundary = *boundary;
    session->pending_cpu_before = *cpu_before;
    session->pending_guest_read = *action;
    session->pending_guest_read_consumer = consumer;
    return 1;
}

int bx_ntvdm_host_session_v1_take_pending_guest_read(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_guest_read_action_v1 *action, uint8_t **bytes)
{
    if (session == 0 || action == 0 || bytes == 0 ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_GUEST_READ ||
        !bx_ntvdm_host_session_boundary_matches(session, boundary, cpu_before) ||
        !bx_ntvdm_guest_read_action_v1_valid(&session->pending_guest_read)) {
        if (session != 0 && session->pending_kind == BX_NTVDM_HOST_PENDING_GUEST_READ)
            bx_ntvdm_host_session_clear_pending(session);
        return 0;
    }
    *action = session->pending_guest_read;
    *bytes = session->pending_guest_read_bytes;
    return 1;
}

int bx_ntvdm_host_session_v1_pending_guest_read_consumer(
    const bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t *consumer)
{
    if (session == 0 || consumer == 0 ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_GUEST_READ ||
        !bx_ntvdm_host_session_boundary_matches(session, boundary, cpu_before) ||
        (session->pending_guest_read_consumer != BX_NTVDM_HOST_GUEST_READ_DEM_DTA &&
         session->pending_guest_read_consumer != BX_NTVDM_HOST_GUEST_READ_SEARCH &&
         session->pending_guest_read_consumer != BX_NTVDM_HOST_GUEST_READ_DEM_FILE_OPEN))
        return 0;
    *consumer = session->pending_guest_read_consumer;
    return 1;
}

int bx_ntvdm_host_session_v1_complete_guest_read(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_cpu_result_v2 *result)
{
    int accepted = session != 0 && result != 0 &&
        session->pending_kind == BX_NTVDM_HOST_PENDING_GUEST_READ &&
        bx_ntvdm_host_session_boundary_matches(session, boundary, cpu_before) &&
        bx_ntvdm_cpu_result_v2_valid(result);
    if (session != 0 && session->pending_kind == BX_NTVDM_HOST_PENDING_GUEST_READ)
        bx_ntvdm_host_session_clear_pending(session);
    return accepted;
}

int bx_ntvdm_host_session_v1_queue_guest_gather_read(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action)
{
    uint32_t index;
    if (session == 0 || boundary == 0 || cpu_before == 0 || action == 0 ||
        session->magic != BX_NTVDM_HOST_SESSION_MAGIC ||
        session->version != BX_NTVDM_HOST_SESSION_VERSION ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_NONE ||
        !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_guest_gather_read_action_v1_valid(action) ||
        action->disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ)
        return 0;
    for (index = 0u; index < action->range_count; ++index) {
        if (!bx_ntvdm_guest_range_within(session->aperture_bytes,
                &action->ranges[index])) return 0;
    }
    session->pending_kind = BX_NTVDM_HOST_PENDING_GUEST_GATHER_READ;
    session->pending_boundary = *boundary;
    session->pending_cpu_before = *cpu_before;
    session->pending_guest_gather_read = *action;
    return 1;
}

int bx_ntvdm_host_session_v1_take_pending_guest_gather_read(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_guest_gather_read_action_v1 *action, uint8_t **bytes)
{
    if (session == 0 || action == 0 || bytes == 0 ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_GUEST_GATHER_READ ||
        !bx_ntvdm_host_session_boundary_matches(session, boundary, cpu_before) ||
        !bx_ntvdm_guest_gather_read_action_v1_valid(
            &session->pending_guest_gather_read)) {
        if (session != 0 &&
            session->pending_kind == BX_NTVDM_HOST_PENDING_GUEST_GATHER_READ)
            bx_ntvdm_host_session_clear_pending(session);
        return 0;
    }
    *action = session->pending_guest_gather_read;
    *bytes = session->pending_guest_gather_read_bytes;
    return 1;
}

int bx_ntvdm_host_session_v1_complete_guest_gather_read(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_cpu_result_v2 *result)
{
    int accepted = session != 0 && result != 0 &&
        session->pending_kind == BX_NTVDM_HOST_PENDING_GUEST_GATHER_READ &&
        bx_ntvdm_host_session_boundary_matches(session, boundary, cpu_before) &&
        bx_ntvdm_cpu_result_v2_valid(result);
    if (session != 0 &&
        session->pending_kind == BX_NTVDM_HOST_PENDING_GUEST_GATHER_READ)
        bx_ntvdm_host_session_clear_pending(session);
    return accepted;
}

int bx_ntvdm_host_session_v1_queue_multi_write(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_multi_write_transaction_v1 *transaction,
    const uint8_t *payload, uint64_t payload_bytes)
{
    if (session == 0 || transaction == 0 || payload == 0 ||
        session->magic != BX_NTVDM_HOST_SESSION_MAGIC ||
        session->version != BX_NTVDM_HOST_SESSION_VERSION ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_NONE ||
        !bx_ntvdm_multi_write_transaction_v1_preflight(transaction,
            session->aperture_bytes, payload_bytes))
        return 0;
    session->pending_kind = BX_NTVDM_HOST_PENDING_MULTI_WRITE;
    session->pending_boundary = transaction->boundary;
    session->pending_cpu_before = transaction->cpu_before;
    session->pending_multi_write = *transaction;
    session->pending_payload = payload;
    session->pending_payload_bytes = payload_bytes;
    return 1;
}

int bx_ntvdm_host_session_v1_take_pending_multi_write(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes)
{
    int accepted = session != 0 && transaction != 0 && payload != 0 &&
        payload_bytes != 0 &&
        session->pending_kind == BX_NTVDM_HOST_PENDING_MULTI_WRITE &&
        bx_ntvdm_host_session_boundary_matches(session, boundary, cpu_before) &&
        session->pending_payload != 0 &&
        bx_ntvdm_multi_write_transaction_v1_preflight(
            &session->pending_multi_write, session->aperture_bytes,
            session->pending_payload_bytes);

    if (accepted) {
        *transaction = session->pending_multi_write;
        *payload = session->pending_payload;
        *payload_bytes = session->pending_payload_bytes;
    }
    if (session != 0 &&
        session->pending_kind == BX_NTVDM_HOST_PENDING_MULTI_WRITE)
        bx_ntvdm_host_session_clear_pending(session);
    return accepted;
}

int bx_ntvdm_host_session_v1_queue_bulk_result(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_bulk_result_transaction_v1 *transaction,
    const uint8_t *payload, uint64_t payload_bytes)
{
    if (session == 0 || transaction == 0 ||
        (payload_bytes != 0u && payload == 0) ||
        session->magic != BX_NTVDM_HOST_SESSION_MAGIC ||
        session->version != BX_NTVDM_HOST_SESSION_VERSION ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_NONE ||
        !bx_ntvdm_bulk_result_transaction_v1_preflight(transaction,
            session->aperture_bytes, payload_bytes)) return 0;
    session->pending_kind = BX_NTVDM_HOST_PENDING_BULK_RESULT;
    session->pending_boundary = transaction->boundary;
    session->pending_cpu_before = transaction->cpu_before;
    session->pending_bulk_result = *transaction;
    if (payload_bytes != 0u)
        memcpy(session->pending_bulk_result_bytes, payload, (size_t)payload_bytes);
    return 1;
}

int bx_ntvdm_host_session_v1_take_pending_bulk_result(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_bulk_result_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes)
{
    int accepted = session != 0 && transaction != 0 && payload != 0 &&
        payload_bytes != 0 &&
        session->pending_kind == BX_NTVDM_HOST_PENDING_BULK_RESULT &&
        bx_ntvdm_host_session_boundary_matches(session, boundary, cpu_before) &&
        bx_ntvdm_bulk_result_transaction_v1_preflight(
            &session->pending_bulk_result, session->aperture_bytes,
            session->pending_bulk_result.payload_bytes);
    if (accepted) {
        *transaction = session->pending_bulk_result;
        *payload = session->pending_bulk_result_bytes;
        *payload_bytes = session->pending_bulk_result.payload_bytes;
    }
    if (session != 0 &&
        session->pending_kind == BX_NTVDM_HOST_PENDING_BULK_RESULT)
        bx_ntvdm_host_session_clear_pending(session);
    return accepted;
}

int bx_ntvdm_host_session_v1_prepare_startup_plan(
    const bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_cpu_state_v1 *entry_cpu,
    uint64_t guest_load_address, uint64_t preserved_state_address,
    uint64_t preserved_state_bytes, bx_ntvdm_startup_plan_v1 *plan,
    const uint8_t **payload, uint64_t *payload_bytes)
{
    bx_ntvdm_guest_write_v1 write;
    if (session == 0 || entry_cpu == 0 || plan == 0 || payload == 0 ||
        payload_bytes == 0 || session->magic != BX_NTVDM_HOST_SESSION_MAGIC ||
        session->version != BX_NTVDM_HOST_SESSION_VERSION ||
        session->pending_kind != BX_NTVDM_HOST_PENDING_NONE ||
        !bx_ntvdm_session_resources_v1_valid(&session->resources) ||
        !bx_ntvdm_host_session_payloads_valid(&session->resources,
            &session->payloads) || session->payloads.ntio_bytes == 0 ||
        session->payloads.ntio_byte_count == 0u)
        return 0;
    bx_ntvdm_guest_write_v1_initialize(&write, guest_load_address,
        session->payloads.ntio_byte_count, 0u);
    bx_ntvdm_startup_plan_v1_initialize(plan, &write, entry_cpu,
        preserved_state_address, preserved_state_bytes);
    if (!bx_ntvdm_startup_plan_v1_preflight(plan, session->aperture_bytes,
            session->payloads.ntio_byte_count)) return 0;
    *payload = session->payloads.ntio_bytes;
    *payload_bytes = session->payloads.ntio_byte_count;
    return 1;
}
