#include "mvdm_redirector_thread.h"

HANDLE mvdm_redirector_create_thread(LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes, DWORD (__cdecl *start_routine)(LPVOID),
    LPVOID parameter, DWORD flags, LPDWORD thread_id)
{
    (void)attributes;
    (void)stack_bytes;
    (void)start_routine;
    (void)parameter;
    (void)flags;
    if (thread_id != NULL) *thread_id = 0u;
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return NULL;
}
