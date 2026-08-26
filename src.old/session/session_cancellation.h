#ifndef APP_SESSION_CANCELLATION_H
#define APP_SESSION_CANCELLATION_H

#include <stdint.h>

enum app_session_cancellation_reason {
    APP_SESSION_CANCELLATION_NONE = 0u,
    APP_SESSION_CANCELLATION_USER_REQUEST = 1u
};

#ifdef __cplusplus
extern "C" {
#endif

int app_session_cancellation_activate(void);
void app_session_cancellation_deactivate(void);
int app_session_cancellation_request(uint32_t reason);
uint32_t app_session_cancellation_requested_reason(void);

#if defined(RUNTIME_CANCELLATION_TESTING)
void app_session_cancellation_test_poll_mark(void);
uint32_t app_session_cancellation_test_poll_count(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
