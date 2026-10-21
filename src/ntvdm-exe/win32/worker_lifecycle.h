#ifndef NTVDM_WORKER_LIFECYCLE_H
#define NTVDM_WORKER_LIFECYCLE_H
#include <windows.h>
#include "ntvdm-exe/session/session.h"
DWORD ntvdm_worker_lifecycle_begin(session *owner,BOOL character_io);
DWORD ntvdm_worker_lifecycle_activate(void);
void NtvdmActivateLifecycle(void);
void ntvdm_worker_lifecycle_end(void);
/* Before original whole-worker cleanup; recursive calls by its owner are safe. */
void ntvdm_worker_lifecycle_authorize_exit(void);
DWORD WINAPI NtvdmAuthorizeExit(DWORD exit_code);
/* Caller owns the duplicate until its I/O producers have stopped. */
DWORD ntvdm_worker_lifecycle_fault_event(HANDLE *);
#endif
