#ifndef FRONTEND_NATIVE_REQUEST_PROTOCOL_H
#define FRONTEND_NATIVE_REQUEST_PROTOCOL_H
#include <windows.h>
#include <stdint.h>
/* Single frontend-client implementation. A completed transfer wins over peer
 * death when both are signaled. Pending cancellation is drained before return.
 * This is not worker-base's strict-peer-first frontend exchange contract. */
DWORD frontend_request_transfer(HANDLE,HANDLE,HANDLE,HANDLE,BOOL,void *,DWORD);
#include "interface/frontend_protocol.h"
/* The peer is an authenticated process reference; stop may be NULL at client. */
#endif
