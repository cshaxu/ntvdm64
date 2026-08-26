#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>

#define SESSION_MAGIC UINT32_C(0x53455353)
#define SESSION_ABI_VERSION UINT32_C(1)
#define SESSION_MAX_TEARDOWNS 8u

enum session_state {
    SESSION_STATE_READY = 0u,
    SESSION_STATE_ACTIVE = 1u,
    SESSION_STATE_CANCELLED = 2u,
    SESSION_STATE_COMPLETED = 3u
};

enum session_cancellation_reason {
    SESSION_CANCELLATION_NONE = 0u,
    SESSION_CANCELLATION_REQUESTED = 1u
};

typedef void (*session_teardown_fn)(void *context);

typedef struct session_teardown {
    session_teardown_fn function;
    void *context;
} session_teardown;

typedef struct session {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t state;
    uint32_t identity;
    uint32_t epoch;
    uint32_t completion_code;
    uint32_t cancellation_reason;
    uint32_t teardown_count;
    volatile long binding_count;
    session_teardown teardowns[SESSION_MAX_TEARDOWNS];
} session;

#ifdef __cplusplus
extern "C" {
#endif

void session_initialize(session *instance, uint32_t identity);
int session_valid(const session *instance);
int session_activate(session *instance);
int session_register_teardown(session *instance, session_teardown_fn function,
    void *context);
int session_request_cancellation(session *instance, uint32_t reason);
void session_complete(session *instance, uint32_t completion_code);
int session_dispose(session *instance);

int session_thread_bind(session *instance);
int session_thread_unbind(session *instance);
session *session_thread_current(void);

#ifdef __cplusplus
}
#endif

#endif
