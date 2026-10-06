/* Trusted unit seeds only. Real Get metadata and guest callback/RPC proof are
 * separate gates; this does not pretend to execute a DOS program. */
#include <service_internal.h>
#include <stdio.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL %u: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void)
{
    OPENNT_BASE_SERVICE *service=OpenNtBaseServiceStart();OPENNT_BASE_CONNECTION *worker=NULL;
    OPENNT_BASE_WORKER_WATCH watch={0};DWORD generation,pid=GetCurrentProcessId();
    HANDLE self=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,pid);
    common_dos_observation fact={0};common_task_trace_node *rows=NULL;uint32_t count,coverage;
    OPENNT_BASE_MANAGEMENT_KEY key;uint64_t first_child;
    CHECK(service && self && !OpenNtBaseServiceConnect(service,self,&worker,&generation));
    watch.service=service;watch.kind=OPENNT_BASE_WORKER_DOS;watch.process=worker->process;
    InitializeListHead(&watch.management_labels);
    EnterCriticalSection(&service->lock);
    worker->process.fVDM=TRUE;InsertTailList(&service->worker_watches,&watch.link);
    CHECK(!service_observation_dos_bind(service,generation,500,1,L"ROOT.COM"));
    LeaveCriticalSection(&service->lock);
    key=(OPENNT_BASE_MANAGEMENT_KEY){service->management_epoch,MANAGEMENT_WORKER,generation,0};
    fact.event=DOS_OBSERVATION_ENTER;fact.occurrence=1;fact.psp=0x120;fact.direct=500;
    CHECK(!OpenNtBaseServiceObserveDosEvent(worker,pid,generation,&fact,FALSE));
    fact.occurrence=2;fact.parent=1;fact.psp=0x200;fact.parent_psp=0x120;lstrcpyW(fact.image,L"CHILD.COM");
    CHECK(!OpenNtBaseServiceObserveDosEvent(worker,pid,generation,&fact,FALSE));
    CHECK(!OpenNtBaseServiceObserveDosEvent(worker,pid,generation,&fact,FALSE));
    CHECK(!OpenNtBaseServiceTaskTrace(service,&key,&coverage,&rows,&count) && count==2);
    CHECK(rows[0].node==500 && rows[0].dos_psp==0x120 && rows[1].dos_psp==0x200 &&
        rows[1].relation==TASK_TRACE_OBSERVED && rows[1].source==TASK_TRACE_SOURCE_DOS &&
        rows[1].parent==500 && rows[1].task==0x200 && rows[1].state==TASK_TRACE_UNCERTAIN);
    first_child=rows[1].node;HeapFree(GetProcessHeap(),0,rows);rows=NULL;
    fact.event=DOS_OBSERVATION_EXIT;fact.parent=0;fact.parent_psp=0;
    CHECK(!OpenNtBaseServiceObserveDosEvent(worker,pid,generation,&fact,FALSE));
    CHECK(!OpenNtBaseServiceObserveDosEvent(worker,pid,generation,&fact,FALSE));
    fact.event=DOS_OBSERVATION_ENTER;fact.occurrence=3;fact.parent=1;fact.parent_psp=0x120;
    CHECK(!OpenNtBaseServiceObserveDosEvent(worker,pid,generation,&fact,FALSE));
    CHECK(!OpenNtBaseServiceTaskTrace(service,&key,&coverage,&rows,&count) && count==3);
    CHECK(rows[1].node==first_child && rows[1].state==TASK_TRACE_EXITED && !(rows[1].flags&TASK_TRACE_EXIT_KNOWN));
    CHECK(rows[2].node!=first_child && rows[2].task==0x200 && rows[2].state==TASK_TRACE_UNCERTAIN);
    HeapFree(GetProcessHeap(),0,rows);rows=NULL;
    fact.direct=501;
    CHECK(OpenNtBaseServiceObserveDosEvent(worker,pid,generation,&fact,FALSE)==ERROR_NOT_FOUND);
    fact.direct=500;fact.occurrence=4;fact.parent_psp=0x999;
    CHECK(OpenNtBaseServiceObserveDosEvent(worker,pid,generation,&fact,FALSE)==ERROR_INVALID_DATA);
    CHECK(OpenNtBaseServiceObserveDosEvent(worker,pid,generation+1,&fact,FALSE)==ERROR_ACCESS_DENIED);
    EnterCriticalSection(&service->lock);service->observation_count=SERVICE_OBSERVATION_MAX;LeaveCriticalSection(&service->lock);
    fact.parent_psp=0x120;
    CHECK(OpenNtBaseServiceObserveDosEvent(worker,pid,generation,&fact,FALSE)==ERROR_NOT_ENOUGH_QUOTA);
    EnterCriticalSection(&service->lock);service->observation_count=3;LeaveCriticalSection(&service->lock);
    CHECK(!OpenNtBaseServiceTaskTrace(service,&key,&coverage,&rows,&count) && count==3 && (coverage&TASK_TRACE_TRUNCATED));
    HeapFree(GetProcessHeap(),0,rows);
    EnterCriticalSection(&service->lock);RemoveEntryList(&watch.link);worker->process.fVDM=FALSE;LeaveCriticalSection(&service->lock);
    CHECK(OpenNtBaseServiceDisconnect(worker)==ERROR_SUCCESS);CHECK(OpenNtBaseServiceIsEmpty(service));CHECK(OpenNtBaseServiceStop(service));
    CloseHandle(self);
    puts("PASS unit DOS occurrence/parent/replay/reuse, true EXIT versus uncertainty, scope/quota and non-retention");return 0;
}
