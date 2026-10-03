#ifndef FRONTEND_NATIVE_REQUEST_CLIENT_H
#define FRONTEND_NATIVE_REQUEST_CLIENT_H
#include "common/codec/native_launch.h"
/* Submit only: no input pump, renderer or backend. The returned process
 * reference is diagnostic; NTSRV's receipt and result own direct completion. */
DWORD run16_native_request_submit(HANDLE,const run16_native_start *,HANDLE *,HANDLE *,DWORD *);
/* target_completed is broker-authenticated, including final-I/O error returns;
 * it is false for unfinished worker failure, stale receipt or RPC failure. */
DWORD run16_native_request_finish(DWORD request,DWORD *exit_code,DWORD *target_completed);
DWORD run16_native_request_resume(HANDLE);
#endif
