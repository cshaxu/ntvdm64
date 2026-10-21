#ifndef LIB_BASE_SYNC_INTERFACE_H
#define LIB_BASE_SYNC_INTERFACE_H

#include "lib/types/types_interface.h"

typedef struct base_sync_mutex base_sync_mutex;

/* Blocking mutual exclusion. A live mutex is required for lock/unlock.
 * Recursive locking is forbidden; only the owning thread may unlock.
 * Destroy requires no owner or waiters; destroying NULL is harmless. */
lib_status base_sync_mutex_create(base_sync_mutex **out_mutex);
void base_sync_mutex_destroy(base_sync_mutex *mutex);
void base_sync_mutex_lock(base_sync_mutex *mutex);
void base_sync_mutex_unlock(base_sync_mutex *mutex);

#endif
