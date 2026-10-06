/* Unit identity/rundown contract, not transport or admission proof. Seed two
 * trusted worker watches with real process objects; link production archive.
 * Real Bind/Hook/RPC acceptance is covered by verify-native-observation. */
#include <service_internal.h>
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL %u: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void)
{
    OPENNT_BASE_SERVICE *service=OpenNtBaseServiceStart();
    OPENNT_BASE_WORKER_WATCH watch={0},replacement={0};HANDLE self;
    WCHAR image[MAX_PATH],line[MAX_PATH+32];STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION child={0},foreign={0};uint64_t first=0,replayed=0;
    common_task_trace_node *rows=NULL;uint32_t count=0,flags=0;
    OPENNT_BASE_MANAGEMENT_KEY key;
    CHECK(service);
    self=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,GetCurrentProcessId());CHECK(self);
    watch.service=service;watch.kind=OPENNT_BASE_WORKER_NATIVE;
    watch.process.ProcessHandle=self;watch.process.SequenceNumber=7;
    EnterCriticalSection(&service->lock);
    InsertTailList(&service->worker_watches,&watch.link);
    service_observation_bind(service,7,1,self);
    LeaveCriticalSection(&service->lock);
    key=(OPENNT_BASE_MANAGEMENT_KEY){service->management_epoch,MANAGEMENT_WORKER,7,0};
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));swprintf_s(line,ARRAYSIZE(line),L"\"%s\"",image);
    CHECK(CreateProcessW(image,line,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&startup,&child));
    CHECK(!OpenNtBaseServiceObserveNativeCreation(service,self,child.hProcess,CREATE_SUSPENDED,&first) && first);
    CHECK(!OpenNtBaseServiceObserveNativeCreation(service,self,child.hProcess,CREATE_SUSPENDED,&replayed) && replayed==first);
    CHECK(!OpenNtBaseServiceTaskTrace(service,&key,&flags,&rows,&count) && count==2);
    CHECK(rows[1].node==first && rows[1].parent==rows[0].node && rows[1].relation==TASK_TRACE_OBSERVED);
    CHECK(rows[0].entered_order && rows[1].entered_order>rows[0].entered_order &&
        rows[0].created_filetime && rows[1].created_filetime);
    CHECK(rows[1].flags&TASK_TRACE_REQUESTED_SUSPENDED);HeapFree(GetProcessHeap(),0,rows);rows=NULL;
    {
        OPENNT_BASE_WORKER_INFO *tree=NULL;uint64_t epoch=0;uint32_t actual=0;
        CHECK(!OpenNtBaseServiceSnapshotCopy(service,&epoch,&tree,&actual) && actual==3);
        CHECK(tree[0].key.category==MANAGEMENT_NATIVE_TASK && !tree[0].depth && !tree[0].actions);
        CHECK(tree[1].key.category==MANAGEMENT_NATIVE_TASK && tree[1].depth==1 &&
            tree[1].parent.object==tree[0].key.object && !tree[1].actions);
        CHECK(tree[2].key.category==MANAGEMENT_WORKER && tree[2].key.generation==7);
        CHECK(OpenNtBaseServiceCloseManagementNode(service,&tree[1].key)==ERROR_NOT_SUPPORTED);
        HeapFree(GetProcessHeap(),0,tree);
    }
    {
        /* Trusted projection timestamps, not OS creation/execution proof. */
        OPENNT_BASE_WORKER_WATCH seeded[5]={0};OPENNT_BASE_WORKER_INFO *tree=NULL;
        const DWORD kinds[5]={OPENNT_BASE_WORKER_NATIVE,OPENNT_BASE_WORKER_DOS,
            OPENNT_BASE_WORKER_DOS,OPENNT_BASE_WORKER_DOS,OPENNT_BASE_WORKER_NATIVE};
        const DWORD times[5]={25,30,20,10,5};uint64_t epoch=0;uint32_t actual=0;
        EnterCriticalSection(&service->lock);
        for(uint32_t index=0;index<5;++index) {
            seeded[index].service=service;seeded[index].kind=kinds[index];seeded[index].wow=index==2;
            seeded[index].process.ProcessHandle=self;seeded[index].process.SequenceNumber=20+index;
            seeded[index].started.dwLowDateTime=times[index];InitializeListHead(&seeded[index].management_labels);
            InsertTailList(&service->worker_watches,&seeded[index].link);
        }
        LeaveCriticalSection(&service->lock);
        CHECK(!OpenNtBaseServiceSnapshotCopy(service,&epoch,&tree,&actual) && actual==8);
        CHECK(tree[0].key.generation==23 && tree[1].key.generation==21 && tree[2].key.generation==22);
        CHECK(tree[3].key.generation==24 && tree[4].key.generation==20);
        CHECK(tree[5].key.category==MANAGEMENT_NATIVE_TASK && tree[6].parent.object==tree[5].key.object);
        HeapFree(GetProcessHeap(),0,tree);
        EnterCriticalSection(&service->lock);
        for(uint32_t index=0;index<5;++index)RemoveEntryList(&seeded[index].link);
        LeaveCriticalSection(&service->lock);
    }
    /* Known reporter cannot nominate itself or a child of an unrelated
     * process as its own descendant. No identity is produced on rejection. */
    replayed=99;CHECK(OpenNtBaseServiceObserveNativeCreation(service,self,self,0,&replayed)==ERROR_NOT_SUPPORTED && !replayed);
    CHECK(CreateProcessW(image,line,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&startup,&foreign));
    replayed=99;CHECK(OpenNtBaseServiceObserveNativeCreation(service,foreign.hProcess,child.hProcess,0,&replayed)==ERROR_NOT_SUPPORTED && !replayed);
    /* Unit statistical capacity seed, not 1024 spawned processes or a smaller
     * production limit. The real admission branch must expose loss and leave
     * this actual, unpublished test process alive. */
    EnterCriticalSection(&service->lock);service->observation_count=SERVICE_OBSERVATION_MAX;LeaveCriticalSection(&service->lock);
    replayed=99;CHECK(OpenNtBaseServiceObserveNativeCreation(service,self,foreign.hProcess,0,&replayed)==ERROR_NOT_ENOUGH_QUOTA && !replayed);
    EnterCriticalSection(&service->lock);service->observation_count=2;LeaveCriticalSection(&service->lock);
    CHECK(!OpenNtBaseServiceTaskTrace(service,&key,&flags,&rows,&count) && count==2);
    CHECK(flags&TASK_TRACE_TRUNCATED);
    HeapFree(GetProcessHeap(),0,rows);rows=NULL;
    /* A subsequent trusted Direct bind in the same worker cannot reuse an
     * old process node identity or reparent its existing observed child. */
    EnterCriticalSection(&service->lock);
    service_observation_bind(service,7,2,foreign.hProcess);
    LeaveCriticalSection(&service->lock);
    CHECK(!OpenNtBaseServiceTaskTrace(service,&key,&flags,&rows,&count) && count==3);
    CHECK(rows[2].relation==TASK_TRACE_DIRECT && rows[2].node!=rows[0].node &&
        rows[1].node==first && rows[1].parent==rows[0].node);
    HeapFree(GetProcessHeap(),0,rows);rows=NULL;
    /* Same display PID is not a worker identity. Generation8 must not expose
     * generation7 history, even with the same actual process in this unit seed. */
    replacement.service=service;replacement.kind=OPENNT_BASE_WORKER_NATIVE;
    replacement.process.ProcessHandle=self;replacement.process.SequenceNumber=8;
    EnterCriticalSection(&service->lock);
    RemoveEntryList(&watch.link);InsertTailList(&service->worker_watches,&replacement.link);
    LeaveCriticalSection(&service->lock);
    CHECK(OpenNtBaseServiceTaskTrace(service,&key,&flags,&rows,&count)==ERROR_NOT_FOUND && !rows && !count);
    key.generation=8;
    CHECK(!OpenNtBaseServiceTaskTrace(service,&key,&flags,&rows,&count) && !rows && !count);
    CHECK(TerminateProcess(child.hProcess,23) && WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
    {
        OPENNT_BASE_WORKER_INFO *tree=NULL;uint64_t epoch=0;uint32_t actual=0;
        CHECK(!OpenNtBaseServiceSnapshotCopy(service,&epoch,&tree,&actual) && actual==3);
        CHECK(service->observation_count==2);
        for(uint32_t index=0;index<actual;++index)CHECK(tree[index].process_id!=child.dwProcessId);
        HeapFree(GetProcessHeap(),0,tree);
    }
    EnterCriticalSection(&service->lock);RemoveEntryList(&replacement.link);LeaveCriticalSection(&service->lock);
    /* Stop cancels pending process waits outside the lock. References are
     * observation-only: unregister/close does not terminate either target. */
    CHECK(OpenNtBaseServiceIsEmpty(service)); /* Observations are not retention. */
    CHECK(OpenNtBaseServiceStop(service));
    CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_OBJECT_0 && WaitForSingleObject(foreign.hProcess,0)==WAIT_TIMEOUT);
    CHECK(TerminateProcess(foreign.hProcess,0));
    CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0 && WaitForSingleObject(foreign.hProcess,5000)==WAIT_OBJECT_0);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);CloseHandle(foreign.hThread);CloseHandle(foreign.hProcess);CloseHandle(self);
    puts("PASS live native tree, ended-node/resource cleanup, duplicate/rejection and pending-wait rundown; surviving target survives observation stop");
    return 0;
}
