#ifndef RUNTIME_STARTUP_SESSION_ENVIRONMENT_H
#define RUNTIME_STARTUP_SESSION_ENVIRONMENT_H

#include <stdint.h>

#include "startup_plan_abi.h"
#include "startup_session.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Loads the CLI's immutable child-environment resource set into a caller-owned
 * adapter-local session. It performs no Bochs action or host-service dispatch. */
int runtime_startup_session_v1_install_from_environment(
    runtime_startup_session_v1 *session);

/* Same-island lifecycle selection for the CLI's immutable child environment.
 * Returns 1 for a prepared plan, 0 when no plan is requested, and -1 only for
 * a malformed partial environment or an impossible prepared-session state. */
int runtime_startup_session_v1_prepare_from_environment(
    runtime_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes);

int runtime_startup_session_v1_copy_host_drive_inventory_from_environment(
    uint8_t out_types[26]);

#ifdef __cplusplus
}
#endif

#endif
