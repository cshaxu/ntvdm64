#ifndef FRONTEND_NATIVE_REQUEST_PROTOCOL_H
#define FRONTEND_NATIVE_REQUEST_PROTOCOL_H
#include <windows.h>
#include <stdint.h>
/* Completion receipt is presentation-only; the process remains the result owner. */
#define NATIVE_REQUEST_VERSION 2u
typedef struct native_request_header { uint32_t version,bytes; } native_request_header;
typedef struct native_request_reply { uint32_t version,error;uint64_t target,receipt; } native_request_reply;
/* The peer is an authenticated process reference; stop may be NULL at client. */
DWORD frontend_request_transfer(HANDLE,HANDLE,HANDLE,HANDLE,BOOL,void *,DWORD);
#endif
