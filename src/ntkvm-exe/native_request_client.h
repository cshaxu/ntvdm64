#ifndef FRONTEND_NATIVE_REQUEST_CLIENT_H
#define FRONTEND_NATIVE_REQUEST_CLIENT_H
#include "interface/native_launch.h"
/* Submit only: no input pump, renderer, backend or target-lifetime ownership.
 * Caller owns the returned actual target handle and waits for its result. */
DWORD run16_native_worker_request_submit(HANDLE,HANDLE,const run16_native_start *,HANDLE *,HANDLE *);
DWORD run16_native_worker_request_begin(HANDLE,HANDLE,const run16_native_start *,HANDLE *,HANDLE *,HANDLE *);
DWORD run16_native_worker_request_finish(HANDLE,HANDLE,HANDLE);
DWORD run16_native_worker_request_resume(HANDLE,HANDLE);
#endif
