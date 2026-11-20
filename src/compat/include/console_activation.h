#ifndef COMPAT_CONSOLE_ACTIVATION_H
#define COMPAT_CONSOLE_ACTIVATION_H

#include <windows.h>

struct compat_console_endpoint;
struct mvdm_mouse_bridge;

typedef enum compat_console_activation_operation {
    CONSOLE_ACTIVATION_OPEN,
    CONSOLE_ACTIVATION_CHECK_PAUSE,
    CONSOLE_ACTIVATION_CLOSE,
    CONSOLE_ACTIVATION_BIND_INPUT,
    CONSOLE_ACTIVATION_UNBIND_INPUT
} compat_console_activation_operation;

/* The endpoint lock protects the request and the borrowed handles. The
 * product observes the native request event and supplies the broker result. */
typedef struct compat_console_activation {
    struct compat_console_endpoint *endpoint;
    struct mvdm_mouse_bridge *mouse;
    HANDLE ready, stop, failure_event;
    HANDLE request, completed;
    HANDLE released_pipe, released_peer;
    BOOL release_pending;
    compat_console_activation_operation operation;
    DWORD status;
} compat_console_activation;

DWORD compat_console_activate(compat_console_activation *, BOOL active);
BOOL ntvdm_console_set_active(BOOL active);

#endif
