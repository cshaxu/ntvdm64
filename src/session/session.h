#ifndef APP_SESSION_V1_H
#define APP_SESSION_V1_H

#include <stdint.h>

#define APP_SESSION_V1_MAGIC UINT32_C(0x4e535631)
#define APP_SESSION_V1_VERSION UINT32_C(1)
#define APP_SESSION_V1_MAX_TEARDOWNS 8u

enum app_session_v1_state {
    APP_SESSION_V1_INACTIVE = 0u,
    APP_SESSION_V1_ACTIVE = 1u,
    APP_SESSION_V1_COMPLETED = 2u
};

typedef void (*app_session_v1_teardown_fn)(void);

typedef struct app_session_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t state;
    uint32_t identity;
    uint32_t epoch;
    uint32_t completion_code;
    uint32_t teardown_count;
    uint32_t reserved0;
    app_session_v1_teardown_fn teardowns[APP_SESSION_V1_MAX_TEARDOWNS];
} app_session_v1;

void app_session_v1_initialize(app_session_v1 *session);
int app_session_v1_valid(const app_session_v1 *session);
int app_session_v1_activate(app_session_v1 *session);
int app_session_v1_register_teardown(app_session_v1 *session,
    app_session_v1_teardown_fn teardown);
void app_session_v1_complete(app_session_v1 *session,
    uint32_t completion_code);
void app_session_v1_reset(app_session_v1 *session);

#endif
