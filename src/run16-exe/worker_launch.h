#ifndef RUN16_WORKER_LAUNCH_H
#define RUN16_WORKER_LAUNCH_H
#include <windows.h>
#include <stdint.h>

/* Shared launcher creation transaction. Success returns a suspended worker
 * already pinned by NTSRV. Caller performs kind-specific registration before
 * ResumeThread. No target streams or running-target lifetime Job is inherited. */
DWORD run16_worker_prepare(uint64_t reservation,PCWSTR image,PWSTR command,
    void *environment,DWORD flags,const STARTUPINFOW *,PROCESS_INFORMATION *);
#endif
