#ifndef BX_NTVDM_ADAPTER_RUNTIME_H
#define BX_NTVDM_ADAPTER_RUNTIME_H

#include <stdint.h>
#include <wchar.h>

#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_guest_read_action_v1.h"
#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_host_transaction_abi.h"
#include "bx_ntvdm_bulk_result_transaction.h"
#include "bx_ntvdm_multi_write_transaction.h"
#include "bx_ntvdm_observation_transaction_abi.h"
#include "bx_ntvdm_startup_snapshot_abi.h"
#include "bx_ntvdm_startup_plan_abi.h"
#include "bx_ntvdm_host_drive_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Adapter-local installation diagnostics. These values classify lifecycle
 * admission only; they are never a BOP, guest, Bochs, or CLI result ABI. */
typedef enum bx_ntvdm_adapter_install_diagnostic_v1 {
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_NONE = 0,
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_ENVIRONMENT,
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_PROFILE_OR_IMAGE,
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_LAUNCH_DECLARATION,
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_SNAPSHOT_EVIDENCE_ROOT,
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_SESSION_RESOURCES,
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_HOST_DRIVE_SNAPSHOT,
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_HOST_SESSION,
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_MACHINE_OBSERVATION,
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_MACHINE_TRIGGER,
    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_MACHINE_SNAPSHOT_TRIGGER
} bx_ntvdm_adapter_install_diagnostic_v1;

/* Same-island diagnostic snapshot. It contains lifecycle state only: no
 * selector, service, guest address, payload, pointer, or mutable session
 * object crosses this copy boundary. */
typedef struct bx_ntvdm_adapter_runtime_diagnostic_state_v1 {
    uint32_t version;
    uint32_t installed;
    uint32_t has_boot_namespace_provider;
    uint32_t pending_kind;
} bx_ntvdm_adapter_runtime_diagnostic_state_v1;

#define BX_NTVDM_ADAPTER_RUNTIME_DIAGNOSTIC_STATE_V1_VERSION 1u

/* Installs exactly one process-local session from independently verified CLI
 * identities. This is adapter lifecycle code, not a Bochs or guest ABI. */
int bx_ntvdm_adapter_runtime_v1_install(const wchar_t *profile_path,
    const wchar_t *byob_root);

/* Installs only when both inherited adapter environment values are present.
 * Returns 1 when installed, 0 when absent, and -1 when explicitly malformed. */
int bx_ntvdm_adapter_runtime_v1_install_from_environment(void);

/* Same-island diagnostic state for a most-recent failed installation. Reset
 * and every successful installation return it to NONE. It changes no install
 * return value or control flow. */
bx_ntvdm_adapter_install_diagnostic_v1
bx_ntvdm_adapter_runtime_v1_install_diagnostic(void);

/* Returns the session's adapter-local host-drive snapshot.  It is immutable
 * after installation and is not a guest-visible drive service. */
int bx_ntvdm_adapter_runtime_v1_copy_host_drive_snapshot(
    bx_ntvdm_host_drive_snapshot_v1 *out);

/* Copies lifecycle state without dispatching, taking, clearing, queuing or
 * resetting the process-local session. This is never a guest or Bochs ABI. */
int bx_ntvdm_adapter_runtime_v1_copy_diagnostic_state(
    bx_ntvdm_adapter_runtime_diagnostic_state_v1 *out);

int bx_ntvdm_adapter_runtime_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result);

/* v2 retains v1 service results through a checked conversion and adds only
 * the separately admitted CF-only result field. */
int bx_ntvdm_adapter_runtime_v2_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

/* v3 is the generic two-phase outcome transport. Until a source-closed
 * consumer requests a read it is an exact v2-to-action conversion. */
int bx_ntvdm_adapter_runtime_v3_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action);
int bx_ntvdm_adapter_runtime_v3_take_pending_guest_read(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_guest_read_action_v1 *action, uint8_t **bytes);
int bx_ntvdm_adapter_runtime_v3_complete_guest_read(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_cpu_result_v2 *result);

/* v4 is a separately versioned, profile-admitted mechanical gather path.
 * Its record contains only generic ranges and an unconsumed resume candidate. */
int bx_ntvdm_adapter_runtime_v4_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_adapter_runtime_v4_take_pending_guest_gather_read(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_guest_gather_read_action_v1 *action, uint8_t **bytes);
int bx_ntvdm_adapter_runtime_v4_complete_guest_gather_read(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_cpu_result_v2 *result);

/* Same-island only: obtains and clears the one prepared payload. */
int bx_ntvdm_adapter_runtime_v1_take_pending_write(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_host_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes,
    uint64_t *resume_rip);

/* Same-island only: obtains and clears one opaque atomic multi-write. */
int bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes);

/* Same-island only: obtains one validated large generic result.  This ABI
 * remains service-neutral; the caller receives no file/token/path metadata. */
int bx_ntvdm_adapter_runtime_v1_take_pending_bulk_result(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_bulk_result_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes);

/* Same-island only. The identifier selects a range already admitted by the
 * installed BYOB profile; this call does not decode a BOP or read memory. */
int bx_ntvdm_adapter_runtime_v1_prepare_observation(uint32_t id,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    uint8_t *output, uint64_t output_bytes);

/* Same-island only. The returned transaction remains tied to the original
 * copied boundary and must be consumed by a generic memory reader. */
int bx_ntvdm_adapter_runtime_v1_take_pending_observation(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_observation_transaction_v1 *transaction,
    uint8_t **output, uint64_t *output_bytes);

/* Same-island only: takes one prepared atomic startup snapshot. */
int bx_ntvdm_adapter_runtime_v1_take_pending_startup_snapshot(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    uint8_t **output, uint64_t *output_bytes);

/* Returns the sole profile-admitted cold-start plan and same-island payload.
 * It is default-off and performs no Bochs or guest mutation. */
int bx_ntvdm_adapter_runtime_v1_prepare_profile_startup_plan(
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes);

/* Resolves the default-off process request into one opaque execution plan.
 * Return values are: 1 ready, 0 absent or disabled, -1 rejected.  This is
 * lifecycle selection only; it does not mutate a machine or its memory. */
int bx_ntvdm_adapter_runtime_v1_prepare_execution_plan_from_environment(
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes);

/* Installs and validates a profile-selected deferred startup session before a
 * CPU loop. It prepares no plan and does not access guest memory. Returns 1
 * armed, 0 absent/disabled, or -1 rejected. */
int bx_ntvdm_adapter_runtime_v1_prepare_deferred_startup_from_environment(void);

/* Takes the opaque startup plan once only after an exact copied profile
 * rendezvous. It has no BOP, firmware, DOS, or host-capability semantics. */
int bx_ntvdm_adapter_runtime_v1_take_deferred_startup_plan(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes);

/* Attempts only the installed profile-owned neutral trigger. The output span
 * is adapter-owned; callers receive it only through take_pending_observation. */
int bx_ntvdm_adapter_runtime_v1_prepare_profile_trigger(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    uint64_t *resume_rip);

/* Completes only the adapter-owned output from one profile trigger after its
 * generic memory copy. The digest is opaque evidence, not guest semantics. */
int bx_ntvdm_adapter_runtime_v1_complete_profile_observation(
    const bx_ntvdm_observation_transaction_v1 *transaction,
    const uint8_t *output, uint64_t output_bytes, uint64_t *digest);

/* Completes one copied profile snapshot and reports only its ordered opaque
 * digest. It does not decode firmware fields or expose Bochs memory. */
int bx_ntvdm_adapter_runtime_v1_complete_profile_startup_snapshot(
    const bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    const uint8_t *output, uint64_t output_bytes, uint64_t *digest);

void bx_ntvdm_adapter_runtime_v1_reset(void);

#ifdef __cplusplus
}
#endif

#endif
