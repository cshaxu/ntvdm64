#ifndef NTSRV_WORKER_SPAWN_H
#define NTSRV_WORKER_SPAWN_H
#include "ntsrv-exe/opennt/include/base_service.h"
/* Service-private creation of a previously admitted product worker. Its
 * caller supplies trusted package/configuration values, never an RPC image. */
DWORD broker_worker_start(OPENNT_BASE_CONNECTION *,DWORD,DWORD,uint64_t,
    PCWSTR,PWSTR,void *,DWORD,const STARTUPINFOW *,HANDLE *);
typedef DWORD (*broker_worker_admit)(void *,HANDLE);
DWORD broker_worker_start_admitted(OPENNT_BASE_CONNECTION *,DWORD,DWORD,uint64_t,
    PCWSTR,PWSTR,void *,DWORD,const STARTUPINFOW *,broker_worker_admit,void *,HANDLE *);
#endif
