#ifndef MVDM_REDIRECTOR_ASYNC_H
#define MVDM_REDIRECTOR_ASYNC_H

/* Private to mvdm-host/vdmredir.  It intentionally has no adapter or product
 * ABI: the matching mirror calls it at its original async request boundaries. */
#include <windows.h>

typedef struct _DOS_ASYNC_NAMED_PIPE_INFO DOS_ASYNC_NAMED_PIPE_INFO;
typedef DOS_ASYNC_NAMED_PIPE_INFO *PDOS_ASYNC_NAMED_PIPE_INFO;

int mvdm_redirector_async_prepare(PDOS_ASYNC_NAMED_PIPE_INFO request,
    WORD segment, WORD offset, DWORD request_type, LPBYTE *buffer_out,
    WORD *length_out);
int mvdm_redirector_async_complete(PDOS_ASYNC_NAMED_PIPE_INFO request,
    DWORD byte_count, DWORD error_code);
void mvdm_redirector_async_release(PDOS_ASYNC_NAMED_PIPE_INFO request);
void mvdm_redirector_async_worker_begin(void);
void mvdm_redirector_async_worker_request_stop(void);
int mvdm_redirector_async_worker_stop_requested(void);

#endif
