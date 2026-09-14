/* Private local registry binding. Never serialize these records. */
#ifndef OPENNT_BASE_PROCESS_H
#define OPENNT_BASE_PROCESS_H
#include <base_server.h>
typedef struct OPENNT_BASE_PROCESS_REGISTRY {
    CRITICAL_SECTION Lock;
    LIST_ENTRY Processes;
    ULONG NextSequence;
    ULONG Pins;
} OPENNT_BASE_PROCESS_REGISTRY;
BOOL OpenNtBaseInitializeProcessRegistry(OPENNT_BASE_PROCESS_REGISTRY *);
BOOL OpenNtBaseDestroyProcessRegistry(OPENNT_BASE_PROCESS_REGISTRY *);
OPENNT_BASE_PROCESS_REGISTRY *OpenNtBaseBindProcessRegistry(OPENNT_BASE_PROCESS_REGISTRY *);
/* Trusted registration only, after RPC authentication; borrows record storage
 * until removal, owns a same-access duplicate of process; never widens rights.
 * PID is derived, never supplied. Query and synchronize rights are required.
 * Caller must serialize registration/removal and service use as designed. */
BOOL OpenNtBaseRegisterProcess(OPENNT_BASE_PROCESS_REGISTRY *, PCSR_PROCESS, HANDLE);
BOOL OpenNtBaseRemoveProcess(OPENNT_BASE_PROCESS_REGISTRY *, PCSR_PROCESS);
/* Trusted owner only: retain a same-access handle without holding the registry
 * lock during external I/O. A retained handle does not keep registration valid;
 * repeat this generation lookup before accepting results. Caller closes it. */
BOOL OpenNtBaseRetainRegisteredProcess(OPENNT_BASE_PROCESS_REGISTRY *,DWORD,ULONG,HANDLE *);
#endif
