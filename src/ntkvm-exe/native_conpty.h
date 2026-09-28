#ifndef NTKVM_NATIVE_CONPTY_H
#define NTKVM_NATIVE_CONPTY_H
#include "native_launch.h"

typedef struct ntkvm_conpty ntkvm_conpty;
/* Runs on the output reader. Consume/copy bytes promptly; never synchronously
 * write terminal replies here (that would couple the two pipe directions). */
typedef DWORD (*ntkvm_conpty_output)(void *,const BYTE *,DWORD);
DWORD ntkvm_conpty_open(COORD,ntkvm_conpty_output,void *,ntkvm_conpty **);
/* Optional frontend-owned stable notification, duplicated by the resource. */
/* Cursor inheritance requires a seeded terminal and concurrent reply service;
 * it does not copy that terminal's cells into the Windows Console buffer. */
DWORD ntkvm_conpty_open_events(COORD,ntkvm_conpty_output,void *,HANDLE,HANDLE,BOOL,ntkvm_conpty **);
DWORD ntkvm_conpty_launch(ntkvm_conpty *,const run16_native_start *,PROCESS_INFORMATION *);
/* A failed write reports its delivered prefix. It cannot be replayed.
 * ERROR_NO_MORE_ITEMS with zero delivered means previously observed clean EOF,
 * not a failed write or permission to replay input already in the backend. */
DWORD ntkvm_conpty_write(ntkvm_conpty *,const void *,DWORD,DWORD *);
DWORD ntkvm_conpty_resize(ntkvm_conpty *,COORD);
/* End new process admission and drop the frontend's keepalive reference.
 * Existing clients continue, but new launches through this HPCON are refused.
 * Natural EOF after the last client, not direct-target exit, ends the backend. */
DWORD ntkvm_conpty_release(ntkvm_conpty *);
HANDLE ntkvm_conpty_ended(ntkvm_conpty *); /* Borrowed event, not target completion. */
DWORD ntkvm_conpty_error(ntkvm_conpty *);
void ntkvm_conpty_cancel(ntkvm_conpty *);
/* Caller first joins users of this object. Output stays drained during close. */
void ntkvm_conpty_close(ntkvm_conpty *);
#endif
