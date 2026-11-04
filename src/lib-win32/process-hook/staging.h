#ifndef PROCESS_HOOK_STAGING_H
#define PROCESS_HOOK_STAGING_H
#include <windows.h>
/* Only a newly created, suspended, uncommitted child is accepted by policy;
 * its creator owns rollback on failure. The payload is an opaque byte span. */
typedef DWORD (*process_hook_payload_consumer)(const void *,DWORD,DWORD,void *);
DWORD process_hook_stage_process(HANDLE child,const char *dll,const void *payload,DWORD bytes);
/* The receiver must link this library and call stage_read/restore itself after
 * the Windows loader has finished. Restore releases the payload allocation.
 * No DLL is added to the target's imports. Same-width suspended targets only. */
DWORD process_hook_stage_payload(HANDLE child,const void *payload,DWORD bytes);
DWORD process_hook_stage_read(process_hook_payload_consumer consume,void *context,BOOL *found);
DWORD process_hook_stage_restore(void);
#endif
