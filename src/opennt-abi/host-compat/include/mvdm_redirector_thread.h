#ifndef MVDM_REDIRECTOR_THREAD_H
#define MVDM_REDIRECTOR_THREAD_H

/* DIVERGENCE(ADAPTER-WIN32-REDIR-001): original Redirector async workers
 * declare a cdecl `DWORD (LPVOID)` start routine, while modern CreateThread
 * requires LPTHREAD_START_ROUTINE/WINAPI.  S3 deliberately retains the
 * original asynchronous failure direction; S4 owns a session-worker thunk. */
#include <windows.h>

HANDLE mvdm_redirector_create_thread(LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes, DWORD (__cdecl *start_routine)(LPVOID),
    LPVOID parameter, DWORD flags, LPDWORD thread_id);

#undef CreateThread
#define CreateThread(attributes, stack_bytes, start_routine, parameter, flags, thread_id) \
    mvdm_redirector_create_thread((attributes), (stack_bytes), (start_routine), \
        (parameter), (flags), (thread_id))

#endif
