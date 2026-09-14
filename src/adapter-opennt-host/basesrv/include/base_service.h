#ifndef OPENNT_BASE_SERVICE_H
#define OPENNT_BASE_SERVICE_H
#include <windows.h>
/* Native composition boundary; opaque pointers never enter command records.
 * One service instance per process, matching original BaseSrv globals.
 * Call only after RPC identity authentication. All operations serialized by
 * the service; transport guarantees connection rundown follows active calls. */
typedef struct OPENNT_BASE_SERVICE OPENNT_BASE_SERVICE;
typedef struct OPENNT_BASE_CONNECTION OPENNT_BASE_CONNECTION;
OPENNT_BASE_SERVICE *OpenNtBaseServiceStart(void);
BOOL OpenNtBaseServiceStop(OPENNT_BASE_SERVICE *);
DWORD OpenNtBaseServiceConnect(OPENNT_BASE_SERVICE *,HANDLE,OPENNT_BASE_CONNECTION **,DWORD *);
DWORD OpenNtBaseServiceDisconnect(OPENNT_BASE_CONNECTION *);
BOOL OpenNtBaseServicePeer(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation);
/* For authenticated in-flight calls only. Caller closes the returned handle;
 * retaining it does not extend the RPC context or registration lifetime. */
DWORD OpenNtBaseServiceRetainPeer(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,HANDLE *);
DWORD OpenNtBaseServiceFirst(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,DWORD *);
#endif
