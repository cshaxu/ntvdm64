#ifndef COMMON_WORKER_THREAD_INITIALIZE_H
#define COMMON_WORKER_THREAD_INITIALIZE_H

#include <windows.h>

/* Called by each image's entry before its original thread clients run. */
BOOL worker_thread_initialize(void);

#endif
