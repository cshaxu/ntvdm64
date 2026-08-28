#ifndef ADAPTER_MVDM_HOST_OUT_WIN32_THREAD_START_COMPAT_H
#define ADAPTER_MVDM_HOST_OUT_WIN32_THREAD_START_COMPAT_H

#include <windows.h>

/* DIVERGENCE(ADAPTER-WIN32-019): Original SoftPC nt_timer.c declares the
 * HeartBeatThread entry with the historical cdecl form.  That is ABI-safe on
 * x64 but incompatible with modern x86 CreateThread's WINAPI callback form.
 * Preserve the original call expression and callback signature by selecting
 * this translation-unit-local source-shaped bridge; its only work is the
 * cdecl-to-WINAPI call boundary. */
typedef DWORD (__cdecl *OPENNT_CDECL_THREAD_START_ROUTINE)(LPVOID parameter);

HANDLE opennt_create_cdecl_thread(
    LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes,
    OPENNT_CDECL_THREAD_START_ROUTINE start_routine,
    LPVOID parameter,
    DWORD flags,
    LPDWORD thread_id);

#define CreateThread(attributes, stack_bytes, start_routine, parameter, flags, thread_id) \
    opennt_create_cdecl_thread((attributes), (stack_bytes), \
        (OPENNT_CDECL_THREAD_START_ROUTINE)(start_routine), (parameter), (flags), (thread_id))

#endif
