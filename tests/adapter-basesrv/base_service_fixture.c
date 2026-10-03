/* Test-only access to the actual archive's private copied queue/take seam.
 * Do not embed a second service translation. All service modules are selected
 * from the same production archive; no substitute policy or public API. */
#include <service_internal.h>
#include <stdio.h>

typedef struct IO_TRANSITION_TEST {
    OPENNT_BASE_CONNECTION *worker;
    DWORD pid,generation,action,error;
    HANDLE entered;
} IO_TRANSITION_TEST;
static DWORD WINAPI fixture_io_transition(void *context)
{
    IO_TRANSITION_TEST *test=context;
    SetEvent(test->entered);
    test->error=OpenNtBaseServiceWorkerIoTransition(test->worker,test->pid,
        test->generation,test->action);
    return test->error;
}

/* Trusted fixture seeds two admitted associations, not a replacement broker.
 * Production registration/selection authentication is tested separately. All
 * grants, waits and disconnect acknowledgements below run the actual archive. */
int fixture_io_authority(void)
{
    OPENNT_BASE_SERVICE *service=OpenNtBaseServiceStart();
    OPENNT_BASE_CONNECTION *root=NULL,*workers[2]={NULL,NULL};
    OPENNT_FRONTEND_ROUTE *routes[2]={NULL,NULL};
    PROCESS_INFORMATION children[2]={{0},{0}};
    STARTUPINFOW startup={sizeof(startup)};
    DWORD root_generation=0,generations[2]={0,0},index,request;
    HANDLE endpoint=NULL,release_capability=NULL;
    WCHAR image[MAX_PATH],command[MAX_PATH+8];
    HANDLE release_thread=NULL,acquire_thread=NULL;
    IO_TRANSITION_TEST release={0},acquire={0};
    int result=1;
#define IO_CHECK(value) do { if(!(value)) { fprintf(stderr,"FAIL I/O authority line %d\n",__LINE__);goto cleanup; } } while(0)
    IO_CHECK(service && GetModuleFileNameW(NULL,image,MAX_PATH));
    IO_CHECK(!OpenNtBaseServiceConnect(service,GetCurrentProcess(),&root,&root_generation));
    root->frontend_capability=CreateEventW(NULL,TRUE,FALSE,NULL);
    IO_CHECK(root->frontend_capability);
    for(index=0;index<2;++index) {
        swprintf_s(command,ARRAYSIZE(command),L"\"%s\"",image);
        IO_CHECK(CreateProcessW(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
            NULL,NULL,&startup,&children[index]));
        IO_CHECK(!OpenNtBaseServiceConnect(service,children[index].hProcess,&workers[index],&generations[index]));
        workers[index]->native_worker=TRUE;
        IO_CHECK(OpenNtBaseServiceWorkerIoReleaseEvent(workers[index],children[index].dwProcessId,
            generations[index],&release_capability)==ERROR_ACCESS_DENIED && !release_capability);
        workers[index]->native_root=root_generation;
        IO_CHECK(OpenNtBaseServiceWorkerIoReleaseEvent(workers[index],children[index].dwProcessId,
            generations[index]+1,&release_capability)==ERROR_ACCESS_DENIED && !release_capability);
        IO_CHECK(!OpenNtBaseServiceWorkerIoReleaseEvent(workers[index],children[index].dwProcessId,
            generations[index],&release_capability));
        IO_CHECK(release_capability && !SetEvent(release_capability) && GetLastError()==ERROR_ACCESS_DENIED);
        CloseHandle(release_capability);release_capability=NULL;
        IO_CHECK(workers[index]->worker_io_release);
        routes[index]=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*routes[index]));
        IO_CHECK(routes[index]);
        routes[index]->root=root;routes[index]->native_worker=TRUE;
        IO_CHECK(DuplicateHandle(GetCurrentProcess(),children[index].hProcess,GetCurrentProcess(),
            &routes[index]->worker,SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0));
        InsertTailList(&service->frontend_routes,&routes[index]->link);
    }
    IO_CHECK(OpenNtBaseServiceWorkerIoTransition(workers[0],children[0].dwProcessId,generations[0],
        WORKER_IO_ACQUIRE)==ERROR_ACCESS_DENIED);
    IO_CHECK(!root->frontend_io_route);
    EnterCriticalSection(&service->lock);
    index=service_authorize_worker_io(workers[0],children[0].dwProcessId);
    LeaveCriticalSection(&service->lock);
    IO_CHECK(!index);
    IO_CHECK(!OpenNtBaseServiceWorkerIoTransition(workers[0],children[0].dwProcessId,generations[0],WORKER_IO_ACQUIRE));
    IO_CHECK(root->frontend_io_route==routes[0]);
    IO_CHECK(OpenNtBaseServiceWorkerIoTransition(workers[1],children[1].dwProcessId,generations[1],
        WORKER_IO_ACQUIRE)==ERROR_ACCESS_DENIED);
    IO_CHECK(WaitForSingleObject(workers[0]->worker_io_release,0)==WAIT_TIMEOUT);
    EnterCriticalSection(&service->lock);
    index=service_authorize_worker_io(workers[1],children[1].dwProcessId);
    LeaveCriticalSection(&service->lock);
    IO_CHECK(!index && WaitForSingleObject(workers[0]->worker_io_release,0)==WAIT_OBJECT_0);
    acquire.worker=workers[1];acquire.pid=children[1].dwProcessId;
    acquire.generation=generations[1];acquire.action=WORKER_IO_ACQUIRE;
    acquire.entered=CreateEventW(NULL,TRUE,FALSE,NULL);IO_CHECK(acquire.entered);
    acquire_thread=CreateThread(NULL,0,fixture_io_transition,&acquire,0,NULL);IO_CHECK(acquire_thread);
    IO_CHECK(WaitForSingleObject(acquire.entered,5000)==WAIT_OBJECT_0);
    IO_CHECK(WaitForSingleObject(acquire_thread,25)==WAIT_TIMEOUT);
    IO_CHECK(WaitForSingleObject(workers[0]->worker_io_release,0)==WAIT_TIMEOUT);
    IO_CHECK(root->frontend_io_route==routes[0]);
    IO_CHECK(!OpenNtBaseServiceWorkerIoTransition(workers[0],children[0].dwProcessId,generations[0],WORKER_IO_RELEASE_BEGIN));
    IO_CHECK(WaitForSingleObject(root->frontend_capability,0)==WAIT_OBJECT_0);
    request=MAXDWORD;
    IO_CHECK(!OpenNtBaseServiceFrontendRequest(root,GetCurrentProcessId(),root_generation,&request,&endpoint));
    IO_CHECK(!request && !endpoint && root->frontend_io_route==routes[0]);
    release.worker=workers[0];release.pid=children[0].dwProcessId;
    release.generation=generations[0];release.action=WORKER_IO_RELEASED;
    release.entered=CreateEventW(NULL,TRUE,FALSE,NULL);IO_CHECK(release.entered);
    release_thread=CreateThread(NULL,0,fixture_io_transition,&release,0,NULL);IO_CHECK(release_thread);
    IO_CHECK(WaitForSingleObject(release.entered,5000)==WAIT_OBJECT_0);
    IO_CHECK(WaitForSingleObject(release_thread,25)==WAIT_TIMEOUT);
    IO_CHECK(WaitForSingleObject(acquire_thread,0)==WAIT_TIMEOUT);
    IO_CHECK(root->frontend_io_route==routes[0]);
    IO_CHECK(OpenNtBaseServiceFrontendIoDisconnected(root,GetCurrentProcessId(),root_generation+1)==ERROR_ACCESS_DENIED);
    IO_CHECK(!OpenNtBaseServiceFrontendIoDisconnected(root,GetCurrentProcessId(),root_generation));
    IO_CHECK(WaitForSingleObject(release_thread,5000)==WAIT_OBJECT_0 && !release.error);
    IO_CHECK(WaitForSingleObject(acquire_thread,5000)==WAIT_OBJECT_0 && !acquire.error);
    IO_CHECK(root->frontend_io_route==routes[1] && !routes[0]->io_requested);
    IO_CHECK(OpenNtBaseServiceWorkerIoTransition(workers[0],children[0].dwProcessId,generations[0],
        WORKER_IO_ACQUIRE)==ERROR_ACCESS_DENIED);
    /* A finished physical connection is not an unaccepted DOS startup.
     * Launcher rundown must retain the resident worker/root association. */
    routes[0]->native_worker=FALSE;routes[0]->request=generations[0];
    service_clear_frontend(workers[0]);
    IO_CHECK(routes[0]->root==root);
    {
        DWORD pending=MAXDWORD,tasks=MAXDWORD;
        /* The second fixture route has no real pipe provider. Remove only
         * its unfulfilled grant to inspect the retained idle association. */
        routes[1]->io_requested=FALSE;
        IO_CHECK(!OpenNtBaseServiceFrontendUsage(root,GetCurrentProcessId(),root_generation,&pending,&tasks));
        IO_CHECK(!pending && !tasks);
    }
    result=0;
cleanup:
    if(release_capability)CloseHandle(release_capability);
    /* No client/guest or production shutdown policy is replaced. The only
     * terminated processes are this fixture's never-resumed child images. */
    if(release_thread){WaitForSingleObject(release_thread,12000);CloseHandle(release_thread);}
    if(acquire_thread){WaitForSingleObject(acquire_thread,12000);CloseHandle(acquire_thread);}
    if(release.entered)CloseHandle(release.entered);
    if(acquire.entered)CloseHandle(acquire.entered);
    for(index=0;index<2;++index) {
        if(workers[index])OpenNtBaseServiceDisconnect(workers[index]);
        if(children[index].hProcess) {
            TerminateProcess(children[index].hProcess,0);WaitForSingleObject(children[index].hProcess,5000);
            CloseHandle(children[index].hThread);CloseHandle(children[index].hProcess);
        }
    }
    if(root)OpenNtBaseServiceDisconnect(root);
    if(service)OpenNtBaseServiceStop(service);
    if(!result)puts("PASS broker I/O authority: acquisition cannot evict; admitted handoff waits for both close acknowledgements; parent cannot reacquire without resume grant");
    return result;
#undef IO_CHECK
}

DWORD fixture_queue_native_command(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE capability,DWORD bytes,const BYTE *payload)
{
    return service_queue_native_command(connection,pid,generation,capability,L"fixture.exe",bytes,payload);
}

DWORD fixture_take_native_command(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,DWORD capacity,BYTE *payload,DWORD *bytes,HANDLE *sender,
    HANDLE *execution,HANDLE *frontend,DWORD *request)
{
    DWORD caller_generation=0;
    return service_take_native_command(connection,pid,generation,capacity,payload,bytes,
        sender,execution,frontend,request,&caller_generation);
}

/* Fault injection changes only the local reference rights of the actual
 * notification object. The real production publisher/decision runs under
 * its existing lock; the owned handle is restored before leaving the seam. */
DWORD fixture_frontend_notification_denied(OPENNT_BASE_CONNECTION *root,
    DWORD pid,DWORD generation,DWORD nonce,BOOL decision,BOOL *closing)
{
    HANDLE original,readonly=NULL;
    DWORD error;
    if(!root || !closing)return ERROR_INVALID_PARAMETER;
    *closing=FALSE;
    EnterCriticalSection(&root->service->lock);
    original=root->frontend_capability;
    if(!DuplicateHandle(GetCurrentProcess(),original,GetCurrentProcess(),
        &readonly,SYNCHRONIZE,FALSE,0))error=GetLastError();
    else {
        root->frontend_capability=readonly;
        error=decision ? OpenNtBaseServiceFrontendJoinDecision(root,pid,generation,nonce,FALSE) :
            service_refresh_frontend_work(root);
        *closing=root->frontend_closing;
        root->frontend_capability=original;
        CloseHandle(readonly);
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}
