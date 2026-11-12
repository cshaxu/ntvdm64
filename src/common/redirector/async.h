#ifndef MVDM_REDIRECTOR_ASYNC_H
#define MVDM_REDIRECTOR_ASYNC_H

/* Shared redirector/worker boundary. The worker retains the session-owned
 * request state; the original redirector calls it at its async boundaries. */
#include <windows.h>

typedef struct _DOS_ASYNC_NAMED_PIPE_INFO DOS_ASYNC_NAMED_PIPE_INFO;
typedef DOS_ASYNC_NAMED_PIPE_INFO *PDOS_ASYNC_NAMED_PIPE_INFO;

#ifdef MVDM_REDIRECTOR_WORKER_EXPORTS
#define MVDM_REDIRECTOR_WORKER_API __declspec(dllexport)
#else
#define MVDM_REDIRECTOR_WORKER_API __declspec(dllimport)
#endif

MVDM_REDIRECTOR_WORKER_API int mvdm_redirector_async_prepare(PDOS_ASYNC_NAMED_PIPE_INFO request,
    WORD segment, WORD offset, DWORD request_type, LPBYTE *buffer_out,
    WORD *length_out);
MVDM_REDIRECTOR_WORKER_API int mvdm_redirector_async_complete(PDOS_ASYNC_NAMED_PIPE_INFO request,
    DWORD byte_count, DWORD error_code);
MVDM_REDIRECTOR_WORKER_API int mvdm_redirector_async_completion_begin(
    PDOS_ASYNC_NAMED_PIPE_INFO request);
MVDM_REDIRECTOR_WORKER_API void mvdm_redirector_async_completion_end(
    PDOS_ASYNC_NAMED_PIPE_INFO request);
MVDM_REDIRECTOR_WORKER_API void mvdm_redirector_async_release(PDOS_ASYNC_NAMED_PIPE_INFO request);

#endif
