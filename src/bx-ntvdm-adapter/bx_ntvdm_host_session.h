#ifndef BX_NTVDM_HOST_SESSION_H
#define BX_NTVDM_HOST_SESSION_H

#include <stdint.h>

#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_bulk_result_transaction.h"
#include "bx_ntvdm_guest_read_action_v1.h"
#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_multi_write_transaction.h"
#include "bx_ntvdm_host_service_plane.h"
#include "bx_ntvdm_observation_transaction_abi.h"
#include "bx_ntvdm_startup_snapshot_abi.h"
#include "bx_ntvdm_startup_plan_abi.h"

#define BX_NTVDM_HOST_SESSION_MAGIC 0x4258534eu
#define BX_NTVDM_HOST_SESSION_VERSION 1u

enum bx_ntvdm_host_pending_operation_kind_v1 {
    BX_NTVDM_HOST_PENDING_NONE = 0u,
    BX_NTVDM_HOST_PENDING_WRITE = 1u,
    BX_NTVDM_HOST_PENDING_OBSERVATION = 2u,
    BX_NTVDM_HOST_PENDING_STARTUP_SNAPSHOT = 3u
    ,BX_NTVDM_HOST_PENDING_GUEST_READ = 4u,
    BX_NTVDM_HOST_PENDING_MULTI_WRITE = 5u,
    BX_NTVDM_HOST_PENDING_GUEST_GATHER_READ = 6u,
    BX_NTVDM_HOST_PENDING_BULK_RESULT = 7u
};

/* Adapter-local only: identifies which already-admitted typed consumer may
 * complete the opaque read bytes.  It is never a Bochs or guest ABI field. */
enum bx_ntvdm_host_guest_read_consumer_v1 {
    BX_NTVDM_HOST_GUEST_READ_NONE = 0u,
    BX_NTVDM_HOST_GUEST_READ_DEM_DTA = 1u,
    BX_NTVDM_HOST_GUEST_READ_SEARCH = 2u,
    BX_NTVDM_HOST_GUEST_READ_DEM_FILE_OPEN = 3u
};

/* Same-island session state. It is not a Bochs/guest ABI and its payload or
 * output pointers are never stored in a versioned record. One operation is
 * bound to one copied #UD boundary and is consumed or cancelled exactly once. */
typedef struct bx_ntvdm_host_session_v1 {
    uint32_t magic;
    uint32_t version;
    bx_ntvdm_session_resources_v1 resources;
    bx_ntvdm_host_service_payloads_v1 payloads;
    uint64_t aperture_bytes;
    uint32_t pending_kind;
    uint32_t write_consumed;
    bx_ntvdm_exception_event_v1 pending_boundary;
    bx_ntvdm_cpu_state_v1 pending_cpu_before;
    bx_ntvdm_host_transaction_v1 pending_transaction;
    const uint8_t *pending_payload;
    uint64_t pending_payload_bytes;
    uint64_t pending_resume_rip;
    bx_ntvdm_observation_transaction_v1 pending_observation;
    uint8_t *pending_output;
    uint64_t pending_output_bytes;
    bx_ntvdm_startup_snapshot_transaction_v1 pending_startup_snapshot;
    bx_ntvdm_guest_read_action_v1 pending_guest_read;
    uint32_t pending_guest_read_consumer;
    uint8_t pending_guest_read_bytes[BX_NTVDM_GUEST_READ_ACTION_V1_MAX_BYTES];
    bx_ntvdm_guest_gather_read_action_v1 pending_guest_gather_read;
    uint8_t pending_guest_gather_read_bytes[
        BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES];
    bx_ntvdm_multi_write_transaction_v1 pending_multi_write;
    bx_ntvdm_bulk_result_transaction_v1 pending_bulk_result;
    uint8_t pending_bulk_result_bytes[
        BX_NTVDM_BULK_RESULT_TRANSACTION_V1_MAX_PAYLOAD];
} bx_ntvdm_host_session_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_host_session_v1_initialize(bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_session_resources_v1 *resources,
    const bx_ntvdm_host_service_payloads_v1 *payloads,
    uint64_t aperture_bytes);

/* A recognized but not-yet-implemented service remains pass-through.  The
 * only admitted ready result is a preflighted transaction; it has not written
 * guest memory or altered CPU state. */
int bx_ntvdm_host_session_v1_dispatch(bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result);

/* Transfers one prepared write only at its original boundary and clears it.
 * A mismatched boundary cancels the operation. The payload remains valid only
 * while the session is alive. */
int bx_ntvdm_host_session_v1_take_pending_write(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_host_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes,
    uint64_t *resume_rip);

/* Same-island only. Queues one preflighted observation with an adapter-owned
 * output span. It neither reads guest memory nor changes CPU state. */
int bx_ntvdm_host_session_v1_queue_observation(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_observation_transaction_v1 *transaction,
    uint8_t *output, uint64_t output_bytes);

/* Transfers one queued observation only at its original boundary and clears
 * it. A mismatched boundary cancels the operation before exposing output. */
int bx_ntvdm_host_session_v1_take_pending_observation(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_observation_transaction_v1 *transaction,
    uint8_t **output, uint64_t *output_bytes);

/* Same-island only. Snapshot is a distinct all-or-nothing operation kind;
 * queueing cannot read memory or alter guest CPU state. */
int bx_ntvdm_host_session_v1_queue_startup_snapshot(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    uint8_t *output, uint64_t output_bytes);

/* Transfers a snapshot only at its copied boundary, then clears it. A failed
 * same-kind take cancels it without exposing a partial output span. */
int bx_ntvdm_host_session_v1_take_pending_startup_snapshot(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    uint8_t **output, uint64_t *output_bytes);

/* Queues exactly one opaque bounded input read at an already copied boundary.
 * Bytes remain session-owned until the matching completion consumes them. */
int bx_ntvdm_host_session_v1_queue_guest_read(bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_read_action_v1 *action, uint32_t consumer);
int bx_ntvdm_host_session_v1_take_pending_guest_read(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_guest_read_action_v1 *action, uint8_t **bytes);
/* Reads the adapter-local completion owner without changing pending state.
 * This is deliberately absent from the Bochs-facing guest-read ABI. */
int bx_ntvdm_host_session_v1_pending_guest_read_consumer(
    const bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, uint32_t *consumer);
int bx_ntvdm_host_session_v1_complete_guest_read(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_cpu_result_v2 *result);

/* Generic multi-range counterpart to the one-range read. It carries no
 * consumer interpretation; callers bind any meaning outside this session. */
int bx_ntvdm_host_session_v1_queue_guest_gather_read(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_host_session_v1_take_pending_guest_gather_read(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_guest_gather_read_action_v1 *action, uint8_t **bytes);
int bx_ntvdm_host_session_v1_complete_guest_gather_read(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_cpu_result_v2 *result);

int bx_ntvdm_host_session_v1_queue_multi_write(bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_multi_write_transaction_v1 *transaction,
    const uint8_t *payload, uint64_t payload_bytes);
int bx_ntvdm_host_session_v1_take_pending_multi_write(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes);

/* Generic one-range result transfer. The session copies nonzero payload into
 * its fixed arena, binds it to one copied boundary, and exposes it once.  The
 * borrowed bytes remain valid until the next bulk queue or session teardown. */
int bx_ntvdm_host_session_v1_queue_bulk_result(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_bulk_result_transaction_v1 *transaction,
    const uint8_t *payload, uint64_t payload_bytes);
int bx_ntvdm_host_session_v1_take_pending_bulk_result(
    bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_bulk_result_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes);

/* Constructs, but does not queue or execute, the one same-island startup
 * plan from the already identity-checked NTIO resource. The caller supplies
 * the source-admitted entry state and capture/restore range; no host pointer
 * enters the plan itself. */
int bx_ntvdm_host_session_v1_prepare_startup_plan(
    const bx_ntvdm_host_session_v1 *session,
    const bx_ntvdm_cpu_state_v1 *entry_cpu,
    uint64_t guest_load_address, uint64_t preserved_state_address,
    uint64_t preserved_state_bytes, bx_ntvdm_startup_plan_v1 *plan,
    const uint8_t **payload, uint64_t *payload_bytes);

#ifdef __cplusplus
}
#endif

#endif
