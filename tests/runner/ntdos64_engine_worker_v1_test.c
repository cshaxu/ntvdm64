#include "engine_worker_v1.h"
#include "bx_ntvdm_cancellation_controller_v1.h"

#include <string.h>

static volatile LONG engine_started;
static volatile LONG cancellation_calls;
static int cancellation_mode;

int bx_ntvdm_engine_run_v1(const struct bx_ntvdm_engine_request_v1 *request,
    struct bx_ntvdm_engine_result_v1 *result)
{
    if (!bx_ntvdm_engine_request_v1_valid(request)) return 0;
    InterlockedExchange(&engine_started, 1);
    if (cancellation_mode) {
        while (InterlockedCompareExchange(&cancellation_calls, 0, 0) == 0) Sleep(1u);
        return bx_ntvdm_engine_result_v1_set(result,
            BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION,
            BX_NTVDM_CANCELLATION_V1_USER_REQUEST);
    }
    return bx_ntvdm_engine_result_v1_set(result,
        BX_NTVDM_ENGINE_TERMINAL_V1_EXECUTION_BUDGET, 77u);
}

int bx_ntvdm_engine_request_cancellation_v1(uint32_t reason)
{
    if (!cancellation_mode || reason != BX_NTVDM_CANCELLATION_V1_USER_REQUEST ||
        InterlockedCompareExchange(&engine_started, 0, 0) == 0) return 0;
    return InterlockedIncrement(&cancellation_calls) == 1;
}

static void descriptor_set(uint16_t *out, const char *text, uint32_t *chars)
{
    uint32_t index = 0u;
    while (text[index] != '\0') out[index] = (uint16_t)(unsigned char)text[index++];
    *chars = index;
}

static void request_set(struct bx_ntvdm_engine_request_v1 *request)
{
    bx_ntvdm_engine_request_v1_clear(request);
    descriptor_set(request->profile_descriptor, "profile", &request->profile_descriptor_chars);
    descriptor_set(request->root_descriptor, "root", &request->root_descriptor_chars);
    descriptor_set(request->launch_descriptor, "launch", &request->launch_descriptor_chars);
    request->instruction_tick_budget = UINT64_C(1);
}

int main(void)
{
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_engine_result_v1 result;
    HANDLE event;
    uint32_t cancellation_accepted;
    request_set(&request);
    event = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (event == NULL) return 1;

    cancellation_mode = 0;
    engine_started = 0;
    cancellation_calls = 0;
    if (!ntdos64_engine_worker_v1_run(&request, event, &result,
            &cancellation_accepted) || cancellation_accepted != 0u ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_EXECUTION_BUDGET ||
        result.detail_code != 77u || cancellation_calls != 0) return 2;

    if (!SetEvent(event)) return 3;
    cancellation_mode = 1;
    engine_started = 0;
    cancellation_calls = 0;
    if (!ntdos64_engine_worker_v1_run(&request, event, &result,
            &cancellation_accepted) || cancellation_accepted != 1u ||
        cancellation_calls != 1 ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION ||
        result.detail_code != BX_NTVDM_CANCELLATION_V1_USER_REQUEST) return 4;
    CloseHandle(event);
    if (ntdos64_engine_worker_v1_run(&request, NULL, &result,
            &cancellation_accepted)) return 5;
    return 0;
}
