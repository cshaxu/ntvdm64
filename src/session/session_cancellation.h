#ifndef APP_SESSION_CANCELLATION_V1_H
#define APP_SESSION_CANCELLATION_V1_H

#include <stdint.h>

enum app_session_cancellation_reason_v1 {
    APP_SESSION_CANCELLATION_V1_NONE = 0u,
    APP_SESSION_CANCELLATION_V1_USER_REQUEST = 1u
};

#ifdef __cplusplus
extern "C" {
#endif

int app_session_cancellation_v1_activate(void);
void app_session_cancellation_v1_deactivate(void);
int app_session_cancellation_v1_request(uint32_t reason);
uint32_t app_session_cancellation_v1_requested_reason(void);

#if defined(RUNTIME_CANCELLATION_TESTING)
void app_session_cancellation_v1_test_poll_mark(void);
uint32_t app_session_cancellation_v1_test_poll_count(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
