/* Private local registry binding. Never serialize these records. */
#ifndef OPENNT_BASE_PROCESS_H
#define OPENNT_BASE_PROCESS_H
#include <windows.h>
struct _CSR_PROCESS;
struct _CSR_THREAD;
struct _CSR_API_MSG;
typedef struct OPENNT_BASE_MESSAGE_VIEW {
    const void *capture;
    ULONG length,count;
    const ULONG_PTR *offsets;
    HANDLE caller;
} OPENNT_BASE_MESSAGE_VIEW;
/* Borrowed fields of the original record, only used under its registry lock. */
typedef struct OPENNT_BASE_PROCESS_VIEW {
    LIST_ENTRY *link;
    HANDLE *pid, *handle;
    ULONG *sequence;
} OPENNT_BASE_PROCESS_VIEW;
typedef struct OPENNT_BASE_PROCESS_PROVIDER {
    struct _CSR_PROCESS *(*record)(LIST_ENTRY *);
    void (*view)(struct _CSR_PROCESS *, OPENNT_BASE_PROCESS_VIEW *);
    /* Borrow original request fields; authenticated process owner pins them. */
    BOOL (*thread_view)(struct _CSR_THREAD *, HANDLE *, ULONG **);
    void (*message_view)(const struct _CSR_API_MSG *, OPENNT_BASE_MESSAGE_VIEW *);
} OPENNT_BASE_PROCESS_PROVIDER;
typedef struct OPENNT_BASE_PROCESS_REGISTRY {
    const OPENNT_BASE_PROCESS_PROVIDER *Provider;
    CRITICAL_SECTION Lock;
    LIST_ENTRY Processes;
    ULONG NextSequence;
    ULONG Pins;
} OPENNT_BASE_PROCESS_REGISTRY;
BOOL OpenNtBaseInitializeProcessRegistry(OPENNT_BASE_PROCESS_REGISTRY *, const OPENNT_BASE_PROCESS_PROVIDER *);
BOOL OpenNtBaseDestroyProcessRegistry(OPENNT_BASE_PROCESS_REGISTRY *);
OPENNT_BASE_PROCESS_REGISTRY *OpenNtBaseBindProcessRegistry(OPENNT_BASE_PROCESS_REGISTRY *);
/* Trusted registration only; borrows record storage
 * until removal, owns a same-access duplicate of process; never widens rights.
 * PID is derived, never supplied. Query and synchronize rights are required.
 * Caller must serialize registration/removal and service use as designed. */
BOOL OpenNtBaseRegisterProcess(OPENNT_BASE_PROCESS_REGISTRY *, struct _CSR_PROCESS *, HANDLE);
BOOL OpenNtBaseRemoveProcess(OPENNT_BASE_PROCESS_REGISTRY *, struct _CSR_PROCESS *);
/* Snapshot only: caller provides higher-level serialization when using this
 * for product lifetime decisions.  A registered process is never inferred
 * idle from its lack of current requests. */
BOOL OpenNtBaseProcessRegistryIsEmpty(OPENNT_BASE_PROCESS_REGISTRY *);
/* Trusted owner only: retain a same-access handle without holding the registry
 * lock during external I/O. A retained handle does not keep registration valid;
 * repeat this generation lookup before accepting results. Caller closes it. */
BOOL OpenNtBaseRetainRegisteredProcess(OPENNT_BASE_PROCESS_REGISTRY *,DWORD,ULONG,HANDLE *);
#endif
