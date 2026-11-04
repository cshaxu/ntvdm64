#ifndef PROCESS_HOOK_PAYLOAD_H
#define PROCESS_HOOK_PAYLOAD_H
#include <windows.h>
/* A freshly-created suspended process is borrowed, and the caller must abort
 * it on failure. The receiver owns removal after successful delivery. A target
 * may hold at most one direct payload; creation and take occur before the
 * target is handed to application code and are not concurrent. The data is not
 * an authentication token; its consumer must validate the copied contents. */
typedef DWORD (*process_hook_payload_reader)(const void *,DWORD,void *);
DWORD process_hook_payload_write(HANDLE child,const void *payload,DWORD bytes);
DWORD process_hook_payload_take(process_hook_payload_reader consume,void *context,BOOL *found);
#endif
