#ifndef NTKVM_WORKER_CONSOLE_CLIENT_H
#define NTKVM_WORKER_CONSOLE_CLIENT_H
#include <windows.h>
#include "console_io.h"
#include "console_video.h"

/* Local client state, never a wire record. worker-base owns the implementation;
 * NTVDM/NTCON embed it, serialize calls and own all borrowed HANDLEs. No
 * renderer, Console ownership, guest state or task scheduling lives here. */
typedef struct ntkvm_worker_client {
    HANDLE pipe,peer,cancel,event;
    DWORD generation,sequence,failure,video_serial;
} ntkvm_worker_client;

/* Borrow pipe/peer/cancel; own only the overlapped event. Init requires zero
 * event/counters. Dispose requires no in-flight calls and never closes borrowed
 * handles. Callers retain their existing locks and endpoint teardown order. */
DWORD ntkvm_worker_client_init(ntkvm_worker_client *,HANDLE,HANDLE,HANDLE,DWORD);
void ntkvm_worker_client_dispose(ntkvm_worker_client *);
/* One attempt only; waiting/retry and backend state transitions stay local. */
DWORD ntkvm_worker_activate(ntkvm_worker_client *,DWORD,BOOL);
/* Encode/send one atomic key batch. Return raw server status/count so the
 * original API and native all-or-error caller retain their distinct contracts. */
DWORD ntkvm_worker_prepend_keys(ntkvm_worker_client *,const INPUT_RECORD *,DWORD,console_io_reply *);

/* Exchange returns transport status; call also interprets the server result.
 * Server errors such as BUSY are retryable; framing/I/O failures are sticky. */
DWORD ntkvm_worker_exchange(ntkvm_worker_client *,console_io_request *,console_io_reply *);
DWORD ntkvm_worker_call(ntkvm_worker_client *,console_io_request *,console_io_reply *);
/* Caller holds its endpoint lock through the complete frame transaction.
 * NULL description retires graphics; backend-specific validation stays local. */
DWORD ntkvm_worker_video(ntkvm_worker_client *,const console_video_description *,const void *);
/* Copy a bounded worker Console title; the caller owns the endpoint lock.
 * A successful reply means NTKVM accepted the title for this active route. */
DWORD ntkvm_worker_publish_title(ntkvm_worker_client *,const char *);
BOOL ntkvm_worker_decode_input(const console_io_input *,INPUT_RECORD *);
#endif
