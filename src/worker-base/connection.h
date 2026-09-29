#ifndef WORKER_BASE_CONNECTION_H
#define WORKER_BASE_CONNECTION_H
#include <windows.h>
/* Worker-side broker lifetime binding. The caller owns CsrPortHeap and its
 * backend state; it must disconnect before destroying that storage. */
DWORD worker_base_connect(void);
void worker_base_disconnect(void);
#endif
