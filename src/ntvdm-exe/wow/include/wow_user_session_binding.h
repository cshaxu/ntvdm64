#ifndef WOW_USER_SESSION_BINDING_H
#define WOW_USER_SESSION_BINDING_H

#include "ntvdm-exe/session/session.h"
#include "wow_user_runtime.h"

/* ADAPTER-WOW-049's bridge to the worker's existing per-thread session
 * lifecycle. It establishes a USER binding for the already-bound worker
 * thread and every subsequently created original worker thread. It neither
 * creates a session nor selects/initializes a WOW task. */
typedef struct wow_user_session_binding {
    session *session;
    wow_user_runtime *runtime;
    BOOL registered;
} wow_user_session_binding;

BOOL WINAPI wow_user_session_attach(wow_user_session_binding *, session *,
    wow_user_runtime *);
BOOL WINAPI wow_user_session_detach(wow_user_session_binding *);

#endif
