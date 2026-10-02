#ifndef NTSRV_NATIVE_CONTROL_H
#define NTSRV_NATIVE_CONTROL_H
#include <windows.h>
#include "interface/frontend_protocol.h"

/* Broker-owned worker reply validation. No launcher control-channel owner. */
DWORD broker_native_reply_status(const native_request_reply *reply,BOOL launch);
DWORD broker_native_completion_status(const native_request_completion *reply);
#endif
