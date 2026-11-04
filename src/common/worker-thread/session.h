#ifndef COMMON_WORKER_THREAD_SESSION_H
#define COMMON_WORKER_THREAD_SESSION_H
#include <stdint.h>
struct session;
enum session_thread_binding_owner {
    SESSION_THREAD_BINDING_UNSPECIFIED = 0u,
    SESSION_THREAD_BINDING_SOFTPC_ENTRY = 1u,
    SESSION_THREAD_BINDING_ORIGINAL_WORKER = 2u
};
struct session *__cdecl session_thread_current(void);
int __cdecl session_thread_bind_owned_source(struct session *, uint32_t, const char *);
int __cdecl session_thread_finish(struct session *);
#endif
