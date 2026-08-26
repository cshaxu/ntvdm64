#ifndef APP_SESSION_H
#define APP_SESSION_H

#include <stdint.h>

#define APP_SESSION_MAGIC UINT32_C(0x4e535631)
#define APP_SESSION_VERSION UINT32_C(1)
#define APP_SESSION_MAX_TEARDOWNS 8u

enum app_session_state {
    APP_SESSION_INACTIVE = 0u,
    APP_SESSION_ACTIVE = 1u,
    APP_SESSION_COMPLETED = 2u
};

typedef void (*app_session_teardown_fn)(void);

typedef struct app_session {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t state;
    uint32_t identity;
    uint32_t epoch;
    uint32_t completion_code;
    uint32_t teardown_count;
    uint32_t reserved0;
    app_session_teardown_fn teardowns[APP_SESSION_MAX_TEARDOWNS];
} app_session;

void app_session_initialize(app_session *session);
int app_session_valid(const app_session *session);
int app_session_activate(app_session *session);
int app_session_register_teardown(app_session *session,
    app_session_teardown_fn teardown);
void app_session_complete(app_session *session,
    uint32_t completion_code);
void app_session_reset(app_session *session);

#endif
