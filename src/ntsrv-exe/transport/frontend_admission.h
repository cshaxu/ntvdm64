#ifndef NTSRV_FRONTEND_ADMISSION_H
#define NTSRV_FRONTEND_ADMISSION_H
#include "ntsrv-exe/opennt/include/base_service.h"
/* Service-private producer boundary, not RPC. The creator owns and pins all
 * four resources until clear; registration compares these exact objects.
 * Called after trusted CreateProcess and before ResumeThread. */
DWORD broker_frontend_admit(OPENNT_BASE_CONNECTION *,DWORD,DWORD,
    HANDLE process,HANDLE capability,HANDLE retire,HANDLE restored);
void broker_frontend_clear_admission(OPENNT_BASE_CONNECTION *);
#endif
