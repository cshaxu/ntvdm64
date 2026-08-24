#include "ntdos64_engine_worker_v1.h"

#include "session/ntdos64_session_cancellation_v1.h"

#include <string.h>

struct ntdos64_engine_worker_v1_context {
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_engine_result_v1 result;
    int run_ok;
};

static DWORD WINAPI ntdos64_engine_worker_v1_main(void *parameter)
{
    struct ntdos64_engine_worker_v1_context *context =
        (struct ntdos64_engine_worker_v1_context *)parameter;
    context->run_ok = bx_ntvdm_engine_run_v1(&context->request, &context->result);
    return 0u;
}

int ntdos64_engine_worker_v1_run(const struct bx_ntvdm_engine_request_v1 *request,
    HANDLE cancellation_event, struct bx_ntvdm_engine_result_v1 *result_out,
    uint32_t *cancellation_accepted_out)
{
    struct ntdos64_engine_worker_v1_context context;
    HANDLE worker;
    HANDLE waits[2];
    DWORD waited;
    uint32_t cancellation_accepted = 0u;

    if (request == NULL || cancellation_event == NULL || result_out == NULL ||
        cancellation_accepted_out == NULL || !bx_ntvdm_engine_request_v1_valid(request)) return 0;
    memset(&context, 0, sizeof(context));
    context.request = *request;
    bx_ntvdm_engine_result_v1_clear(&context.result);
    worker = CreateThread(NULL, 0u, ntdos64_engine_worker_v1_main, &context, 0u, NULL);
    if (worker == NULL) return 0;
    waits[0] = worker;
    waits[1] = cancellation_event;
    for (;;) {
        waited = WaitForMultipleObjects(2u, waits, FALSE, INFINITE);
        if (waited == WAIT_OBJECT_0) break;
        if (waited != WAIT_OBJECT_0 + 1u) {
            WaitForSingleObject(worker, INFINITE);
            CloseHandle(worker);
            return 0;
        }
        if (!cancellation_accepted && bx_ntvdm_engine_request_cancellation_v1(
            NTDOS64_SESSION_CANCELLATION_V1_USER_REQUEST)) cancellation_accepted = 1u;
        /* A manual-reset event remains signaled.  Once the typed request has
         * been accepted, wait only for the owned worker's joined result.  If
         * it was already inactive, the next wait observes worker completion. */
        if (cancellation_accepted) {
            if (WaitForSingleObject(worker, INFINITE) != WAIT_OBJECT_0) {
                CloseHandle(worker);
                return 0;
            }
            break;
        }
        if (WaitForSingleObject(worker, 0u) == WAIT_OBJECT_0) break;
        Sleep(1u);
    }
    CloseHandle(worker);
    if (!context.run_ok || !bx_ntvdm_engine_result_v1_valid(&context.result)) return 0;
    *result_out = context.result;
    *cancellation_accepted_out = cancellation_accepted;
    return 1;
}
