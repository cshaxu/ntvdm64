#ifndef NTCON_WORKER_CONSOLE_CLIENT_H
#define NTCON_WORKER_CONSOLE_CLIENT_H
#include <windows.h>
#include "common/protocol/console_io.h"
#include "common/protocol/console_video.h"

/* Local client state, never a wire record. common owns the implementation;
 * NTVDM/NTVWM embed it, serialize calls and own all borrowed HANDLEs. No
 * renderer, Console ownership, guest state or task scheduling lives here. */
typedef struct ntcon_worker_client {
    HANDLE pipe,peer,cancel,event;
    DWORD generation,sequence,failure,video_serial;
} ntcon_worker_client;

/* Borrow pipe/peer/cancel; own only the overlapped event. Init requires zero
 * event/counters. Dispose requires no in-flight calls and never closes borrowed
 * handles. Callers retain their existing locks and endpoint teardown order. */
DWORD ntcon_worker_client_init(ntcon_worker_client *,HANDLE,HANDLE,HANDLE,DWORD);
void ntcon_worker_client_dispose(ntcon_worker_client *);
/* One attempt only; waiting/retry and backend state transitions stay local. */
DWORD ntcon_worker_activate(ntcon_worker_client *,DWORD,BOOL);
/* Encode/send one atomic key batch. Return raw server status/count so the
 * original API and native all-or-error caller retain their distinct contracts. */
DWORD ntcon_worker_prepend_keys(ntcon_worker_client *,const INPUT_RECORD *,DWORD,console_io_reply *);

/* Exchange returns transport status; call also interprets the server result.
 * Server errors such as BUSY are retryable; framing/I/O failures are sticky. */
DWORD ntcon_worker_exchange(ntcon_worker_client *,console_io_request *,console_io_reply *);
DWORD ntcon_worker_call(ntcon_worker_client *,console_io_request *,console_io_reply *);
/* Caller holds its endpoint lock through the complete frame transaction.
 * NULL description retires graphics; backend-specific validation stays local. */
DWORD ntcon_worker_video(ntcon_worker_client *,const console_video_description *,const void *);
/* Copy a bounded worker Console title; the caller owns the endpoint lock.
 * A successful reply means NTCON accepted the title for this active route. */
DWORD ntcon_worker_publish_title(ntcon_worker_client *,const char *);
BOOL ntcon_worker_decode_input(const console_io_input *,INPUT_RECORD *);
#endif
