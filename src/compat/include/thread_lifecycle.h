#ifndef COMPAT_THREAD_LIFECYCLE_H
#define COMPAT_THREAD_LIFECYCLE_H

#include "lib-win32/thread/thread.h"

/* Each consuming image installs a process-lifetime table before invoking
 * original code. Capture and finish retain ownership at the product boundary;
 * the compatibility bridge owns no session state. */
typedef struct opennt_thread_lifecycle {
    void (*capture)(win32_thread_hooks *hooks);
    void (*finish)(void);
} opennt_thread_lifecycle;

BOOL opennt_thread_install_lifecycle(const opennt_thread_lifecycle *lifecycle);

#endif
