#include "lib/types/win32/scalar.h"
#include "lib/base/sync_interface.h"

#include "lib/types/win32/sync.h"

struct base_sync_mutex { lib_win32_critical_section gate; };

lib_status base_sync_mutex_create(base_sync_mutex **out_mutex)
{
    base_sync_mutex *mutex;
    if (out_mutex == LIB_NULL) return LIB_STATUS_INVALID_ARGUMENT;
    *out_mutex = LIB_NULL;
    mutex = lib_allocate(sizeof(*mutex));
    if (mutex == LIB_NULL) return LIB_STATUS_NO_MEMORY;
    if (!lib_win32_initialize_critical_section_and_spin_count(&mutex->gate, 0u)) {
        lib_release(mutex);
        return LIB_STATUS_IO_ERROR;
    }
    *out_mutex = mutex;
    return LIB_STATUS_OK;
}

void base_sync_mutex_destroy(base_sync_mutex *mutex)
{
    if (mutex == LIB_NULL) return;
    lib_win32_delete_critical_section(&mutex->gate);
    lib_release(mutex);
}

void base_sync_mutex_lock(base_sync_mutex *mutex)
{ lib_win32_enter_critical_section(&mutex->gate); }
void base_sync_mutex_unlock(base_sync_mutex *mutex)
{ lib_win32_leave_critical_section(&mutex->gate); }
