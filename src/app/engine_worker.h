#ifndef APP_ENGINE_WORKER_V1_H
#define APP_ENGINE_WORKER_V1_H

/* CLI-owned worker/event orchestration.  HANDLE is deliberately confined to
 * this outer layer; the engine receives only its copied request and fixed
 * cancellation reason. */

#include <stdint.h>
#include <windows.h>

#include "engine_contract.h"

int app_engine_worker_v1_run(const struct runtime_engine_request_v1 *request,
    HANDLE cancellation_event, struct runtime_engine_result_v1 *result_out,
    uint32_t *cancellation_accepted_out);

#endif
