#ifndef NTDOS64_SESSION_CANCELLATION_V1_H
#define NTDOS64_SESSION_CANCELLATION_V1_H

#include <stdint.h>

enum ntdos64_session_cancellation_reason_v1 {
    NTDOS64_SESSION_CANCELLATION_V1_NONE = 0u,
    NTDOS64_SESSION_CANCELLATION_V1_USER_REQUEST = 1u
};

#ifdef __cplusplus
extern "C" {
#endif

int ntdos64_session_cancellation_v1_activate(void);
void ntdos64_session_cancellation_v1_deactivate(void);
int ntdos64_session_cancellation_v1_request(uint32_t reason);
uint32_t ntdos64_session_cancellation_v1_requested_reason(void);

#if defined(BX_NTVDM_CANCELLATION_TESTING)
void ntdos64_session_cancellation_v1_test_poll_mark(void);
uint32_t ntdos64_session_cancellation_v1_test_poll_count(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
