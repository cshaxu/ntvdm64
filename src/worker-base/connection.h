#ifndef WORKER_BASE_CONNECTION_H
#define WORKER_BASE_CONNECTION_H
#include <windows.h>
/* Worker-side broker lifetime binding. The caller owns CsrPortHeap and its
 * backend state; it must disconnect before destroying that storage. */
DWORD worker_base_connect(void);
void worker_base_disconnect(void);
/* Resolve a copied frontend capability through NTSRV. The returned process
 * handle is wait-only and belongs to the caller; an event or PID alone is
 * never proof that a worker is still attached to its original root. */
DWORD worker_base_retain_frontend_root(HANDLE capability,HANDLE *process);
#endif
