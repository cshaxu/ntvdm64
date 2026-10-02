#ifndef FRONTEND_NATIVE_REQUEST_CLIENT_H
#define FRONTEND_NATIVE_REQUEST_CLIENT_H
#include "interface/native_launch.h"
/* Submit only: no input pump, renderer or backend. The returned process
 * reference is diagnostic; NTSRV's receipt and result own direct completion. */
DWORD run16_native_request_submit(HANDLE,const run16_native_start *,HANDLE *,HANDLE *,DWORD *);
DWORD run16_native_request_finish(DWORD request,DWORD *exit_code);
DWORD run16_native_request_resume(HANDLE);
#endif
