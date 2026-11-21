#ifndef MVDM_REDIRECTOR_ASYNC_H
#define MVDM_REDIRECTOR_ASYNC_H

/* The original redirector holds a request; compat owns its guest staging
 * until completion or release. */
#include <windows.h>


#ifdef MVDM_REDIRECTOR_WORKER_EXPORTS
#define MVDM_REDIRECTOR_WORKER_API __declspec(dllexport)
#else
#define MVDM_REDIRECTOR_WORKER_API __declspec(dllimport)
#endif

MVDM_REDIRECTOR_WORKER_API int mvdm_redirector_async_prepare(void **state_out,
    DWORD *guest_buffer_out, DWORD *anr_out, BOOL *type2_out, DWORD *semaphore_out,
    WORD segment, WORD offset, int is_read, int type2, LPBYTE *buffer_out,
    WORD *length_out);
MVDM_REDIRECTOR_WORKER_API int mvdm_redirector_async_complete(void *opaque_state,
    DWORD byte_count, DWORD error_code);
MVDM_REDIRECTOR_WORKER_API void mvdm_redirector_async_release(void **state);

#endif
