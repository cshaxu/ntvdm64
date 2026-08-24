#ifndef NTDOS64_SESSION_V1_H
#define NTDOS64_SESSION_V1_H

#include <stdint.h>

#define NTDOS64_SESSION_V1_MAGIC UINT32_C(0x4e535631)
#define NTDOS64_SESSION_V1_VERSION UINT32_C(1)
#define NTDOS64_SESSION_V1_MAX_TEARDOWNS 8u

enum ntdos64_session_v1_state {
    NTDOS64_SESSION_V1_INACTIVE = 0u,
    NTDOS64_SESSION_V1_ACTIVE = 1u,
    NTDOS64_SESSION_V1_COMPLETED = 2u
};

typedef void (*ntdos64_session_v1_teardown_fn)(void);

typedef struct ntdos64_session_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t state;
    uint32_t identity;
    uint32_t epoch;
    uint32_t completion_code;
    uint32_t teardown_count;
    uint32_t reserved0;
    ntdos64_session_v1_teardown_fn teardowns[NTDOS64_SESSION_V1_MAX_TEARDOWNS];
} ntdos64_session_v1;

void ntdos64_session_v1_initialize(ntdos64_session_v1 *session);
int ntdos64_session_v1_valid(const ntdos64_session_v1 *session);
int ntdos64_session_v1_activate(ntdos64_session_v1 *session);
int ntdos64_session_v1_register_teardown(ntdos64_session_v1 *session,
    ntdos64_session_v1_teardown_fn teardown);
void ntdos64_session_v1_complete(ntdos64_session_v1 *session,
    uint32_t completion_code);
void ntdos64_session_v1_reset(ntdos64_session_v1 *session);

#endif
