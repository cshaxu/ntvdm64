/* Original BaseSrv initialization/dispatch plus finite registered caller
 * binding. No test provider, command state machine or CSR runtime. */
#include "basesrv.h"
#include <base_service.h>
#include <base_process.h>
#include <base_dispatch.h>
struct OPENNT_BASE_SERVICE {
    OPENNT_BASE_PROCESS_REGISTRY registry;
    CRITICAL_SECTION lock;
};
struct OPENNT_BASE_CONNECTION {
    OPENNT_BASE_SERVICE *service;
    CSR_PROCESS process;
};
/* Original guarded USER hook is absent in standalone CLI composition. */
PFNNOTIFYPROCESSCREATE UserNotifyProcessCreate=NULL;
OPENNT_BASE_SERVICE *OpenNtBaseServiceStart(void)
{
    OPENNT_BASE_SERVICE *service=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*service));
    if (!service) return NULL;
    if (!InitializeCriticalSectionEx(&service->lock,0,0)) {
        HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    if (!OpenNtBaseInitializeProcessRegistry(&service->registry)) {
        DeleteCriticalSection(&service->lock); HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    BaseSrvVDMInit();
    return service;
}
BOOL OpenNtBaseServiceStop(OPENNT_BASE_SERVICE *service)
{
    /* Transport has stopped and joined all calls/rundowns before stop. */
    if (!service || !OpenNtBaseDestroyProcessRegistry(&service->registry)) return FALSE;
    DeleteCriticalSection(&service->lock);
    HeapFree(GetProcessHeap(),0,service);
    return TRUE;
}
DWORD OpenNtBaseServiceConnect(OPENNT_BASE_SERVICE *service,HANDLE process,
    OPENNT_BASE_CONNECTION **output,DWORD *generation)
{
    OPENNT_BASE_CONNECTION *connection;
    DWORD error=0;
    if (!service || !output || !generation) return ERROR_INVALID_PARAMETER;
    *output=NULL; *generation=0;
    connection=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*connection));
    if (!connection) return ERROR_NOT_ENOUGH_MEMORY;
    connection->service=service;
    EnterCriticalSection(&service->lock);
    if (!OpenNtBaseRegisterProcess(&service->registry,&connection->process,process)) error=GetLastError();
    else { *output=connection; *generation=connection->process.SequenceNumber; }
    LeaveCriticalSection(&service->lock);
    if (error) HeapFree(GetProcessHeap(),0,connection);
    return error;
}
DWORD OpenNtBaseServiceDisconnect(OPENNT_BASE_CONNECTION *connection)
{
    OPENNT_BASE_SERVICE *service;
    DWORD error=0;
    if (!connection) return ERROR_INVALID_PARAMETER;
    service=connection->service;
    EnterCriticalSection(&service->lock);
    if (!OpenNtBaseRemoveProcess(&service->registry,&connection->process)) error=GetLastError();
    LeaveCriticalSection(&service->lock);
    if (!error) HeapFree(GetProcessHeap(),0,connection);
    return error;
}
BOOL OpenNtBaseServicePeer(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation)
{
    return connection && (DWORD)connection->process.ClientId.UniqueProcess==pid &&
        connection->process.SequenceNumber==generation;
}
DWORD OpenNtBaseServiceFirst(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,DWORD *first)
{
    BASE_API_MSG message={0};
    CSR_THREAD thread={0};
    PCSR_THREAD previousThread;
    OPENNT_BASE_PROCESS_REGISTRY *previousRegistry;
    NTSTATUS status;
    if (!connection || !first) return ERROR_INVALID_PARAMETER;
    *first=0;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    previousThread=OpenNtBaseBindServerRequestThread(&thread);
    previousRegistry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_IS_FIRST,sizeof(message.u.IsFirstVDM));
    OpenNtBaseBindProcessRegistry(previousRegistry);
    OpenNtBaseBindServerRequestThread(previousThread);
    LeaveCriticalSection(&connection->service->lock);
    if (status) return RtlNtStatusToDosError(status);
    *first=message.u.IsFirstVDM.FirstVDM;
    return 0;
}
