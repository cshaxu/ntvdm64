/* Test-only access to the actual archive's private copied queue/take seam.
 * Do not embed a second service translation. All service modules are selected
 * from the same production archive; no substitute policy or public API. */
#include <service_internal.h>
#include <stdio.h>

int fixture_parent_resume_origin(void)
{
    OPENNT_BASE_SERVICE *service=OpenNtBaseServiceStart();
    OPENNT_BASE_CONNECTION *root=NULL,*parent=NULL,*caller=NULL;
    HANDLE self=NULL,native=NULL,dos=NULL,again=NULL;
    HANDLE target=NULL,receipt=NULL;DWORD request=0;
    PROCESS_INFORMATION children[2]={{0}};STARTUPINFOW startup={sizeof(startup)};
    WCHAR image[MAX_PATH],command[MAX_PATH+32];DWORD i;
    DWORD rg=0,pg=0,cg=0;BOOL required=FALSE;
    OPENNT_BASE_WORKER_WATCH parent_watch={0};
#define ORIGIN_CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL origin %d\n",__LINE__);return 1;}}while(0)
    self=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,GetCurrentProcessId());
    ORIGIN_CHECK(service && self);
    ORIGIN_CHECK(!OpenNtBaseServiceConnect(service,self,&root,&rg));
    ORIGIN_CHECK(GetModuleFileNameW(NULL,image,ARRAYSIZE(image)));
    for(i=0;i<2;++i) {
        ORIGIN_CHECK(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --reservation-child",image)>0);
        ORIGIN_CHECK(CreateProcessW(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
            NULL,NULL,&startup,&children[i]));
    }
    ORIGIN_CHECK(!OpenNtBaseServiceConnect(service,children[0].hProcess,&parent,&pg));
    ORIGIN_CHECK(!OpenNtBaseServiceConnect(service,children[1].hProcess,&caller,&cg));
    EnterCriticalSection(&service->lock);
    root->frontend_capability=CreateEventW(NULL,TRUE,FALSE,NULL);
    ORIGIN_CHECK(root->frontend_capability);
    ORIGIN_CHECK(!service_acquire_console_context(root,NULL,pg,&native));
    ORIGIN_CHECK(!service_acquire_console_context(root,NULL,rg,&dos));
    ORIGIN_CHECK(!service_acquire_console_context(root,NULL,pg,&again));
    LeaveCriticalSection(&service->lock);
    ORIGIN_CHECK(!OpenNtBaseServiceBindConsoleContext(caller,children[1].dwProcessId,cg,native));
    ORIGIN_CHECK(caller->execution_worker_generation==pg);
    EnterCriticalSection(&service->lock);
    parent->native_worker=TRUE;parent->native_inflight=1;
    parent_watch.process=parent->process;parent_watch.service=service;
    parent_watch.kind=OPENNT_BASE_WORKER_NATIVE;
    parent_watch.frontend_root_generation=rg;
    InsertTailList(&service->worker_watches,&parent_watch.link);
    ORIGIN_CHECK(service_worker_root(parent)==rg && service_root_has_worker(root));
    ORIGIN_CHECK(service_prepare_parent_resume(caller,rg,&required)==ERROR_INVALID_STATE);
    caller->dos_completion_read=TRUE;
    ORIGIN_CHECK(service_prepare_parent_resume(caller,rg+1,&required)==ERROR_INVALID_STATE);
    ORIGIN_CHECK(!service_prepare_parent_resume(caller,rg,&required) && required);
    ORIGIN_CHECK(caller->selected_native_generation==pg);
    /* A completed PIF leaves the launcher on the child's Console. Resume
     * restores only that launcher's association to its authenticated parent. */
    caller->retained_frontend_root=rg;
    caller->console=(HANDLE)(ULONG_PTR)0x712;
    ORIGIN_CHECK(!service_prepare_parent_resume(caller,rg,&required) && required);
    ORIGIN_CHECK(caller->console==parent->console);
    ORIGIN_CHECK(!OpenNtBaseServiceRetainCommandWorker(caller,children[1].dwProcessId,cg,&target));
    ORIGIN_CHECK(GetProcessId(target)==children[0].dwProcessId);
    CloseHandle(target);target=NULL;
    caller->console=(HANDLE)(ULONG_PTR)0x712;
    caller->dos_completion_read=FALSE;
    ORIGIN_CHECK(service_prepare_parent_resume(caller,rg,&required)==ERROR_INVALID_STATE);
    ORIGIN_CHECK(caller->console==(HANDLE)(ULONG_PTR)0x712);
    ORIGIN_CHECK(OpenNtBaseServiceRetainCommandWorker(caller,children[1].dwProcessId,cg,&target)==ERROR_PROCESS_ABORTED && !target);
    caller->dos_completion_read=TRUE;
    caller->execution_worker_generation=rg;
    ORIGIN_CHECK(OpenNtBaseServiceRetainCommandWorker(caller,children[1].dwProcessId,cg,&target)==ERROR_PROCESS_ABORTED && !target);
    caller->execution_worker_generation=pg;
    caller->retained_frontend_root=rg+1;
    ORIGIN_CHECK(service_prepare_parent_resume(caller,rg+1,&required)==ERROR_INVALID_STATE);
    ORIGIN_CHECK(caller->console==(HANDLE)(ULONG_PTR)0x712);
    ORIGIN_CHECK(OpenNtBaseServiceRetainCommandWorker(caller,children[1].dwProcessId,cg,&target)!=ERROR_SUCCESS && !target);
    caller->retained_frontend_root=rg;
    caller->console=NULL;
    parent->worker_failed=TRUE;
    ORIGIN_CHECK(service_prepare_parent_resume(caller,rg,&required)==ERROR_PROCESS_ABORTED);
    parent->worker_failed=FALSE;parent->native_worker=FALSE;
    caller->retained_frontend_root=0;
    caller->selected_native_generation=0;
    LeaveCriticalSection(&service->lock);
    /* Same root/Console but another origin must not reuse the native locator. */
    ORIGIN_CHECK(!OpenNtBaseServiceBindConsoleContext(caller,children[1].dwProcessId,cg,dos));
    ORIGIN_CHECK(caller->execution_worker_generation==rg);
    EnterCriticalSection(&service->lock);
    root->process.fVDM=TRUE;
    ORIGIN_CHECK(!service_prepare_parent_resume(caller,rg,&required) && !required);
    ORIGIN_CHECK(!OpenNtBaseServiceSubmitNativeRequest(caller,children[1].dwProcessId,cg,
        root->frontend_capability,0,NULL,&target,&receipt,&request));
    ORIGIN_CHECK(!target && !receipt && !request && !caller->dos_completion_read);
    ORIGIN_CHECK(OpenNtBaseServiceSubmitNativeRequest(caller,children[1].dwProcessId,cg,
        root->frontend_capability,0,NULL,&target,&receipt,&request)==ERROR_INVALID_STATE);
    caller->dos_completion_read=TRUE;
    root->process.fVDM=FALSE;
    caller->execution_worker_generation=MAXDWORD;
    ORIGIN_CHECK(service_prepare_parent_resume(caller,rg,&required)==ERROR_PROCESS_ABORTED);
    RemoveEntryList(&parent_watch.link);
    ORIGIN_CHECK(!service_worker_root(parent) && !service_root_has_worker(root));
    LeaveCriticalSection(&service->lock);
    ORIGIN_CHECK(OpenNtBaseServiceBindConsoleContext(caller,children[1].dwProcessId,cg+1,again)==ERROR_ACCESS_DENIED);
    CloseHandle(native);CloseHandle(dos);CloseHandle(again);
    ORIGIN_CHECK(!OpenNtBaseServiceDisconnect(caller));
    ORIGIN_CHECK(!OpenNtBaseServiceDisconnect(parent));
    ORIGIN_CHECK(!OpenNtBaseServiceDisconnect(root));
    ORIGIN_CHECK(OpenNtBaseServiceIsEmpty(service) && OpenNtBaseServiceStop(service));
    CloseHandle(self);
    for(i=0;i<2;++i){ORIGIN_CHECK(TerminateProcess(children[i].hProcess,0));CloseHandle(children[i].hProcess);CloseHandle(children[i].hThread);}
    puts("PASS parent origin: authenticated bind, no premature/foreign resume, native grant, DOS no-op, failed/stale origin");
    return 0;
#undef ORIGIN_CHECK
}

/* Projection/close fixture: seed the existing admitted GUI record boundary,
 * bind a real suspended target through the production provider, then model
 * its already-tested startup transfer. No substitute close implementation. */
int fixture_management_gui(void)
{
    OPENNT_BASE_SERVICE *service=OpenNtBaseServiceStart();
    OPENNT_BASE_CONNECTION *worker=NULL;
    OPENNT_BASE_WIN32RECORD *record=NULL;
    OPENNT_BASE_WORKER_INFO *tree=NULL;
    PROCESS_INFORMATION carrier={0},target={0};STARTUPINFOW startup={sizeof(startup)};
    WCHAR image[MAX_PATH],command[MAX_PATH+32];
    HANDLE receipt=NULL;
    DWORD generation=0,result=1;uint32_t count=0;uint64_t epoch=0;
    OPENNT_BASE_MANAGEMENT_KEY key={0};
#define GUI_CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL management-gui line %d\n",__LINE__);goto cleanup;}}while(0)
    GUI_CHECK(service && GetModuleFileNameW(NULL,image,ARRAYSIZE(image)));
    swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --reservation-child",image);
    GUI_CHECK(CreateProcessW(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
        NULL,NULL,&startup,&carrier));
    swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --reservation-child",image);
    GUI_CHECK(CreateProcessW(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
        NULL,NULL,&startup,&target));
    GUI_CHECK(!OpenNtBaseServiceConnect(service,carrier.hProcess,&worker,&generation));
    receipt=CreateEventW(NULL,TRUE,FALSE,NULL);GUI_CHECK(receipt);
    record=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*record));GUI_CHECK(record);
    record->request=17;record->gui=TRUE;lstrcpyW(record->image,L"fixture-gui.exe");
    EnterCriticalSection(&service->lock);
    worker->native_worker=TRUE;
    InsertTailList(&worker->win32records,&record->link);
    LeaveCriticalSection(&service->lock);
    GUI_CHECK(!OpenNtBaseServiceBindNativeTarget(worker,carrier.dwProcessId,generation,17,target.hProcess,receipt));
    EnterCriticalSection(&service->lock);
    RemoveEntryList(&record->link);InsertTailList(&service->gui_records,&record->link);
    LeaveCriticalSection(&service->lock);
    GUI_CHECK(!OpenNtBaseServiceSnapshotCopy(service,&epoch,&tree,&count) && count==1);
    GUI_CHECK(tree[0].key.category==MANAGEMENT_GUI_TARGET && tree[0].key.generation==generation &&
        tree[0].key.object==17 && !tree[0].parent.category && !tree[0].depth &&
        tree[0].process_id==target.dwProcessId && tree[0].kind==
        (sizeof(void *)==8 ? MANAGEMENT_KIND_WIN64 : MANAGEMENT_KIND_WIN32) &&
        tree[0].actions==MANAGEMENT_CAN_CLOSE && tree[0].started_filetime && tree[0].image[0]);
    key=tree[0].key;++key.object;
    GUI_CHECK(OpenNtBaseServiceCloseManagementNode(service,&key)==ERROR_NOT_FOUND);
    GUI_CHECK(WaitForSingleObject(target.hProcess,0)==WAIT_TIMEOUT);
    key=tree[0].key;
    GUI_CHECK(!OpenNtBaseServiceCloseManagementNode(service,&key));
    GUI_CHECK(WaitForSingleObject(target.hProcess,5000)==WAIT_OBJECT_0);
    GUI_CHECK(WaitForSingleObject(carrier.hProcess,0)==WAIT_TIMEOUT);
    GUI_CHECK(WaitForSingleObject(receipt,0)==WAIT_TIMEOUT); /* Close isn't fake completion. */
    EnterCriticalSection(&service->lock);service_prune_gui_records(service);LeaveCriticalSection(&service->lock);
    record=NULL;
    GUI_CHECK(WaitForSingleObject(receipt,0)==WAIT_OBJECT_0);
    GUI_CHECK(OpenNtBaseServiceCloseManagementNode(service,&key)==ERROR_NOT_FOUND);
    HeapFree(GetProcessHeap(),0,tree);tree=NULL;
    GUI_CHECK(!OpenNtBaseServiceSnapshotCopy(service,&epoch,&tree,&count) && !count && !tree);
    result=0;
cleanup:
    if(tree)HeapFree(GetProcessHeap(),0,tree);
    if(target.hProcess){TerminateProcess(target.hProcess,ERROR_CANCELLED);WaitForSingleObject(target.hProcess,5000);}
    if(service){EnterCriticalSection(&service->lock);service_prune_gui_records(service);LeaveCriticalSection(&service->lock);}
    if(worker)OpenNtBaseServiceDisconnect(worker);
    if(service)OpenNtBaseServiceStop(service);
    if(receipt)CloseHandle(receipt);
    if(target.hThread)CloseHandle(target.hThread);if(target.hProcess)CloseHandle(target.hProcess);
    if(carrier.hProcess){TerminateProcess(carrier.hProcess,ERROR_CANCELLED);WaitForSingleObject(carrier.hProcess,5000);}
    if(carrier.hThread)CloseHandle(carrier.hThread);if(carrier.hProcess)CloseHandle(carrier.hProcess);
    if(!result)puts("PASS management GUI: real pinned target, independent row, stale key, carrier survives, actual exit owns completion");
    return result;
#undef GUI_CHECK
}

/* Trusted fixture inserts only the existing watch record; all deadline,
 * cancellation and shutdown decisions execute the production archive. */
int fixture_shared_worker_residency(void)
{
    OPENNT_BASE_SERVICE *service=OpenNtBaseServiceStart();
    OPENNT_BASE_CONNECTION *worker=NULL,*root=NULL;
    OPENNT_BASE_WORKER_WATCH watch={0};
    PROCESS_INFORMATION child={0};STARTUPINFOW startup={sizeof(startup)};
    WCHAR image[MAX_PATH],command[MAX_PATH+32];
    DWORD generation=0,root_generation=0,index;ULONGLONG due=0;
#define RETIRE_CHECK(value) do {if(!(value)){fprintf(stderr,"FAIL unbound line %d\n",__LINE__);return 1;}}while(0)
    RETIRE_CHECK(service && GetModuleFileNameW(NULL,image,ARRAYSIZE(image)));
    swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --reservation-child",image);
    RETIRE_CHECK(CreateProcessW(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
        NULL,NULL,&startup,&child));
    RETIRE_CHECK(!OpenNtBaseServiceConnect(service,child.hProcess,&worker,&generation));
    watch.service=service;watch.kind=OPENNT_BASE_WORKER_NATIVE;
    watch.process.ProcessHandle=child.hProcess;watch.process.SequenceNumber=generation;
    watch.shutdown=CreateEventW(NULL,TRUE,FALSE,NULL);RETIRE_CHECK(watch.shutdown);
    EnterCriticalSection(&service->lock);
    worker->native_worker=TRUE;
    InsertTailList(&service->worker_watches,&watch.link);
    LeaveCriticalSection(&service->lock);
    RETIRE_CHECK(!service_next_frontend_deadline_at(service,100,&due) && !due);
    RETIRE_CHECK(!service_retire_expired_frontends_at(service,10099));
    RETIRE_CHECK(WaitForSingleObject(watch.shutdown,0)==WAIT_TIMEOUT);
    EnterCriticalSection(&service->lock);
    worker->native_inflight=1; /* Existing direct execution suppresses idle grace. */
    LeaveCriticalSection(&service->lock);
    RETIRE_CHECK(!service_next_frontend_deadline_at(service,10100,&due) && !due);
    RETIRE_CHECK(!service_retire_expired_frontends_at(service,20000));
    RETIRE_CHECK(WaitForSingleObject(watch.shutdown,0)==WAIT_TIMEOUT);
    EnterCriticalSection(&service->lock);worker->native_inflight=0;LeaveCriticalSection(&service->lock);
    RETIRE_CHECK(!service_next_frontend_deadline_at(service,30000,&due) && !due);
    RETIRE_CHECK(!service_retire_expired_frontends_at(service,39999));
    RETIRE_CHECK(WaitForSingleObject(watch.shutdown,0)==WAIT_TIMEOUT);
    RETIRE_CHECK(!service_retire_expired_frontends_at(service,40000));
    RETIRE_CHECK(WaitForSingleObject(watch.shutdown,0)==WAIT_TIMEOUT);
    RETIRE_CHECK(!service_retire_expired_frontends_at(service,86400000));
    RETIRE_CHECK(!service_next_frontend_deadline_at(service,86400000,&due) && !due);
    RETIRE_CHECK(WaitForSingleObject(watch.shutdown,0)==WAIT_TIMEOUT);
    /* Shared WOW also has no app-idle retirement. Its original task/kernel
     * exit still owns a separate WOW lifetime, not this service timer. */
    EnterCriticalSection(&service->lock);
    watch.kind=OPENNT_BASE_WORKER_WOW;watch.wow=TRUE;worker->native_worker=FALSE;
    LeaveCriticalSection(&service->lock);
    RETIRE_CHECK(!service_retire_expired_frontends_at(service,172800000));
    RETIRE_CHECK(!service_next_frontend_deadline_at(service,172800000,&due) && !due);
    RETIRE_CHECK(WaitForSingleObject(watch.shutdown,0)==WAIT_TIMEOUT);
    RETIRE_CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
    /* Paired DOS/native relationship decisions read this same watch, even
     * when its Console identity does not match the root's Console. */
    RETIRE_CHECK(!OpenNtBaseServiceConnect(service,GetCurrentProcess(),&root,&root_generation));
    root->frontend_capability=CreateEventW(NULL,TRUE,FALSE,NULL);
    RETIRE_CHECK(root->frontend_capability);
    for(index=0;index<2;++index) {
        EnterCriticalSection(&service->lock);
        watch.wow=FALSE;watch.kind=index ? OPENNT_BASE_WORKER_NATIVE : OPENNT_BASE_WORKER_DOS;
        watch.console=(HANDLE)123;root->console=(HANDLE)456;
        root->frontend_closing=FALSE;
        RETIRE_CHECK(!service_bind_worker_root(service,child.hProcess,root));
        RETIRE_CHECK(watch.frontend_root_generation==root_generation && service_root_has_worker(root));
        LeaveCriticalSection(&service->lock);
        RETIRE_CHECK(!service_retire_expired_frontends_at(service,172800001+index));
        RETIRE_CHECK(WaitForSingleObject(watch.shutdown,0)==WAIT_TIMEOUT);
        EnterCriticalSection(&service->lock);root->frontend_closing=TRUE;LeaveCriticalSection(&service->lock);
        RETIRE_CHECK(!service_retire_expired_frontends_at(service,172800003+index));
        RETIRE_CHECK(WaitForSingleObject(watch.shutdown,0)==WAIT_OBJECT_0);
        RETIRE_CHECK(ResetEvent(watch.shutdown));
    }
    /* Native final-empty completion uses the same root authority, but only
     * its creating launcher may retire a self-created Console. Borrowed and
     * nested Console sessions remain resident. No original PIF is replaced. */
    EnterCriticalSection(&service->lock);
    worker->native_worker=TRUE;worker->console=root->console;
    worker->retained_frontend_root=root_generation;root->frontend_closing=FALSE;
    root->frontend_creator_generation=generation+1;
    service_retire_completed_root(worker,generation);
    RETIRE_CHECK(!root->frontend_closing);
    root->frontend_creator_generation=generation;root->frontend_borrowed=TRUE;
    service_retire_completed_root(worker,generation);
    RETIRE_CHECK(!root->frontend_closing);
    root->frontend_borrowed=FALSE;
    service_retire_completed_root(worker,generation);
    RETIRE_CHECK(root->frontend_closing);
    LeaveCriticalSection(&service->lock);
    EnterCriticalSection(&service->lock);RemoveEntryList(&watch.link);LeaveCriticalSection(&service->lock);
    RETIRE_CHECK(!OpenNtBaseServiceDisconnect(root));
    RETIRE_CHECK(!OpenNtBaseServiceDisconnect(worker));
    RETIRE_CHECK(OpenNtBaseServiceIsEmpty(service) && OpenNtBaseServiceStop(service));
    RETIRE_CHECK(TerminateProcess(child.hProcess,0));
    CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(watch.shutdown);
    puts("PASS shared GUI carrier: native/WOW residency; DOS/native common root authority and loss shutdown");
    return 0;
#undef RETIRE_CHECK
}

/* Trust only the fixture's prepared process identities. The production
 * rundown must preserve cancellation before delivery for either worker kind,
 * and release an already closed physical route rather than retaining it. */
int fixture_route_cancellation(void)
{
    DWORD kind,phase;
    for(kind=0;kind<2;++kind)for(phase=0;phase<2;++phase) {
        OPENNT_BASE_SERVICE *service=OpenNtBaseServiceStart();
        OPENNT_BASE_CONNECTION *root=NULL;
        OPENNT_FRONTEND_ROUTE *route;
        DWORD generation;
#define CANCEL_CHECK(value) do {if(!(value)){fprintf(stderr,"FAIL cancellation kind=%lu phase=%lu line=%d\n",kind,phase,__LINE__);return 1;}}while(0)
        CANCEL_CHECK(service && !OpenNtBaseServiceConnect(service,GetCurrentProcess(),&root,&generation));
        root->frontend_capability=CreateEventW(NULL,TRUE,FALSE,NULL);
        CANCEL_CHECK(root->frontend_capability);
        route=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*route));
        CANCEL_CHECK(route);
        route->root=root;route->native_worker=kind!=0;
        route->io_worker_closed=route->io_frontend_closed=phase!=0;
        CANCEL_CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
            &route->worker,SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0));
        route->ready=CreateEventW(NULL,TRUE,FALSE,NULL);CANCEL_CHECK(route->ready);
        InsertTailList(&service->frontend_routes,&route->link);
        EnterCriticalSection(&service->lock);
        service_clear_frontend(root);
        if(phase)CANCEL_CHECK(IsListEmpty(&service->frontend_routes));
        else {
            CANCEL_CHECK(!IsListEmpty(&service->frontend_routes));
            CANCEL_CHECK(!route->root && !route->pipe && !route->ready);
            CANCEL_CHECK(service_authorize_worker_io(root,GetCurrentProcessId())==ERROR_PIPE_NOT_CONNECTED);
            /* The cancellation tombstone holds the same live process until
             * its owning rundown; an idle prune must not discard the proof. */
            service_prune_cancelled_frontends(service);
            CANCEL_CHECK(!IsListEmpty(&service->frontend_routes));
            service_delete_frontend(route);
        }
        LeaveCriticalSection(&service->lock);
        CANCEL_CHECK(!OpenNtBaseServiceDisconnect(root));
        CANCEL_CHECK(OpenNtBaseServiceIsEmpty(service) && OpenNtBaseServiceStop(service));
#undef CANCEL_CHECK
    }
    puts("PASS paired route cancellation: pending identity retained, closed lease removed, no cross-root acquisition");
    return 0;
}

static void WINAPI fixture_worker_cleanup_seen(void *context)
{
    (void)SetEvent((HANDLE)context);
}

int fixture_prepared_native_root_loss(void)
{
    OPENNT_BASE_SERVICE *service=OpenNtBaseServiceStart();
    OPENNT_BASE_CONNECTION *root=NULL,*worker=NULL;
    PROCESS_INFORMATION child={0};STARTUPINFOW startup={sizeof(startup)};
    WCHAR image[MAX_PATH],command[MAX_PATH+32];
    DWORD root_generation=0,worker_generation=0,member=GetCurrentProcessId();
    uint64_t reservation=0;HANDLE shutdown=NULL,cleanup_seen=NULL;
#define PREPARED_CHECK(value) do {if(!(value)){fprintf(stderr,"FAIL prepared root loss line=%d\n",__LINE__);return 1;}}while(0)
    PREPARED_CHECK(service);
    cleanup_seen=CreateEventW(NULL,TRUE,FALSE,NULL);PREPARED_CHECK(cleanup_seen);
    PREPARED_CHECK(OpenNtBaseServiceConfigureEmptyNotify(service,fixture_worker_cleanup_seen,cleanup_seen));
    PREPARED_CHECK(!OpenNtBaseServiceConnect(service,GetCurrentProcess(),&root,&root_generation));
    root->frontend_capability=CreateEventW(NULL,TRUE,FALSE,NULL);
    PREPARED_CHECK(root->frontend_capability);
    PREPARED_CHECK(!OpenNtBaseServiceReportConsoleMembers(root,member,root_generation,1,&member));
    PREPARED_CHECK(!OpenNtBaseServiceCreateNativeReservation(root,member,root_generation,&reservation));
    PREPARED_CHECK(GetModuleFileNameW(NULL,image,ARRAYSIZE(image)));
    swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --reservation-child",image);
    PREPARED_CHECK(CreateProcessW(NULL,command,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,
        NULL,NULL,&startup,&child));
    PREPARED_CHECK(!OpenNtBaseServicePrepareWorker(root,member,root_generation,reservation,child.hProcess));
    PREPARED_CHECK(!OpenNtBaseServiceRequestFrontend(root,member,root_generation,root->frontend_capability));
    EnterCriticalSection(&service->lock);
    service_clear_frontend(root);
    LeaveCriticalSection(&service->lock);
    /* Connect must consume the canceled exact-process grant, not the still
     * valid Console membership fallback. No substitute worker is selected. */
    PREPARED_CHECK(!OpenNtBaseServiceConnect(service,child.hProcess,&worker,&worker_generation));
    PREPARED_CHECK(!OpenNtBaseServiceWorkerShutdownEvent(worker,child.dwProcessId,worker_generation,&shutdown));
    PREPARED_CHECK(WaitForSingleObject(shutdown,0)==WAIT_OBJECT_0);
    PREPARED_CHECK(!service_worker_root(worker));
    CloseHandle(shutdown);
    PREPARED_CHECK(TerminateProcess(child.hProcess,ERROR_CANCELLED));
    PREPARED_CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
    PREPARED_CHECK(WaitForSingleObject(cleanup_seen,5000)==WAIT_OBJECT_0);
    PREPARED_CHECK(!OpenNtBaseServiceDisconnect(worker));
    PREPARED_CHECK(!OpenNtBaseServiceDisconnect(root));
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    PREPARED_CHECK(OpenNtBaseServiceIsEmpty(service) && OpenNtBaseServiceStop(service));
    CloseHandle(cleanup_seen);
    puts("PASS prepared native root loss: actual claim/Connect consumes cancellation, shutdown already signaled, no adoption");
    return 0;
#undef PREPARED_CHECK
}

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
    OPENNT_BASE_WORKER_WATCH watches[2]={{0},{0}};
    BOOL watched[2]={FALSE,FALSE};
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
        watches[index].process=workers[index]->process;watches[index].service=service;
        watches[index].kind=OPENNT_BASE_WORKER_NATIVE;
        watches[index].frontend_root_generation=root_generation;
        InsertTailList(&service->worker_watches,&watches[index].link);watched[index]=TRUE;
        IO_CHECK(service_worker_root(workers[index])==root_generation && service_root_has_worker(root));
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
    IO_CHECK(OpenNtBaseServiceWorkerIoTransition(workers[0],children[0].dwProcessId,
        generations[0],WORKER_IO_RELEASE_BEGIN)==ERROR_ACCESS_DENIED);
    {
        OPENNT_BASE_WIN32RECORD *parent,*child;
        DWORD decision=99;
        parent=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*parent));
        IO_CHECK(parent);parent->request=11;
        InsertTailList(&workers[0]->win32records,&parent->link);
        child=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*child));
        IO_CHECK(child);child->request=12;
        InsertTailList(&workers[0]->win32records,&child->link);
        IO_CHECK(OpenNtBaseServiceWorkerIoCheckpoint(workers[0],children[0].dwProcessId,
            generations[0]+1,WORKER_IO_CHECKPOINT_COMPLETE,12,&decision)==ERROR_ACCESS_DENIED);
        IO_CHECK(decision==WORKER_IO_KEEP);
        IO_CHECK(OpenNtBaseServiceWorkerIoCheckpoint(workers[0],children[0].dwProcessId,
            generations[0],WORKER_IO_CHECKPOINT_COMPLETE,99,&decision)==ERROR_INVALID_STATE);
        IO_CHECK(!OpenNtBaseServiceWorkerIoCheckpoint(workers[0],children[0].dwProcessId,
            generations[0],WORKER_IO_CHECKPOINT_COMPLETE,12,&decision));
        IO_CHECK(decision==WORKER_IO_KEEP && !routes[0]->io_release_ordered);
        IO_CHECK(OpenNtBaseServiceWorkerIoCheckpoint(workers[0],children[0].dwProcessId,
            generations[0],WORKER_IO_CHECKPOINT_PAUSE,0,&decision)==ERROR_ACCESS_DENIED);
        /* Only the last direct completion can receive the service release
         * instruction. It does not yet notify the frontend to close. */
        child->completed=TRUE;
        IO_CHECK(!OpenNtBaseServiceWorkerIoCheckpoint(workers[0],children[0].dwProcessId,
            generations[0],WORKER_IO_CHECKPOINT_COMPLETE,11,&decision));
        IO_CHECK(decision==WORKER_IO_RELEASE && routes[0]->io_release_ordered &&
            !routes[0]->io_releasing && !routes[0]->io_frontend_closed);
        EnterCriticalSection(&service->lock);
        index=service_authorize_worker_io(workers[0],children[0].dwProcessId);
        LeaveCriticalSection(&service->lock);
        IO_CHECK(index==ERROR_BUSY);
        IO_CHECK(OpenNtBaseServiceWorkerIoTransition(workers[0],children[0].dwProcessId,
            generations[0],WORKER_IO_ACQUIRE)==ERROR_BUSY);
        IO_CHECK(root->frontend_io_route==routes[0] && !routes[0]->io_releasing);
    }
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
        if(watched[index])RemoveEntryList(&watches[index].link);
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
