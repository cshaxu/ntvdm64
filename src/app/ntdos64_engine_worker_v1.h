#ifndef NTDOS64_ENGINE_WORKER_V1_H
#define NTDOS64_ENGINE_WORKER_V1_H

/* CLI-owned worker/event orchestration.  HANDLE is deliberately confined to
 * this outer layer; the engine receives only its copied request and fixed
 * cancellation reason. */

#include <stdint.h>
#include <windows.h>

#include "bx_ntvdm_engine_contract_v1.h"

int ntdos64_engine_worker_v1_run(const struct bx_ntvdm_engine_request_v1 *request,
    HANDLE cancellation_event, struct bx_ntvdm_engine_result_v1 *result_out,
    uint32_t *cancellation_accepted_out);

#endif
