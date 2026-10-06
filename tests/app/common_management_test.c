#include "common/rpc/management.h"
#include "common/protocol/version.h"
#include "common/protocol/task_trace.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned checks,failures,calls,allocations,mode;
static common_rpc_management expected;
static const unsigned char version[APP_VERSION_BYTES]=APP_VERSION;
static DTASKMGR_KEY selected={17,2,123,0};
C_ASSERT(sizeof(common_task_trace_node)==sizeof(WORKER_TRACE_NODE));
C_ASSERT(sizeof(common_task_trace_node)==568);
C_ASSERT(offsetof(common_task_trace_node,image)==48);
C_ASSERT(offsetof(WORKER_TRACE_NODE,image)==48);
#define CHECK(x) do { ++checks; if (!(x)) { ++failures;printf("FAIL %u %s\n",__LINE__,#x); } } while(0)
void *__RPC_USER MIDL_user_allocate(size_t size) {void *p=malloc(size);if(p)++allocations;return p;}
void __RPC_USER MIDL_user_free(void *p) {if(p){--allocations;free(p);}}
static void peer(handle_t binding,HANDLE process,unsigned long protocol,unsigned char *application)
{
    ++calls;CHECK(binding==expected.binding);CHECK(process==expected.process);
    CHECK(protocol==APP_PROTOCOL_VERSION);CHECK(!memcmp(application,version,APP_VERSION_BYTES));
}
static DWORD result(void)
{
    if(mode==2)RpcRaiseException(RPC_S_CALL_FAILED);
    return mode==1 ? ERROR_ACCESS_DENIED : ERROR_SUCCESS;
}
error_status_t Client_TaskSnapshot(handle_t binding,HANDLE process,unsigned long protocol,
    unsigned char *application,unsigned long *count,DTASKMGR_WORKER **items)
{
    peer(binding,process,protocol,application);
    *count=mode==3 ? 0 : 1;
    *items=mode==3 ? NULL : MIDL_user_allocate(sizeof(**items));
    if(*items){ZeroMemory(*items,sizeof(**items));(*items)->process_id=123;}
    return result();
}
error_status_t Client_CloseManagementNode(handle_t binding,HANDLE process,unsigned long protocol,
    unsigned char *application,DTASKMGR_KEY *key)
{
    peer(binding,process,protocol,application);
    CHECK(key && key->instance==selected.instance && key->category==selected.category &&
        key->generation==selected.generation && key->object==selected.object);return result();
}
error_status_t Client_WorkerTaskTrace(handle_t binding,HANDLE process,unsigned long protocol,
    unsigned char *application,DTASKMGR_KEY *key,unsigned long *coverage,
    unsigned long *count,WORKER_TRACE_NODE **items)
{
    peer(binding,process,protocol,application);CHECK(key->instance==selected.instance);
    *coverage=1;*count=mode==3 ? 0 : 1;
    *items=mode==3 ? NULL : MIDL_user_allocate(sizeof(**items));
    if(*items){ZeroMemory(*items,sizeof(**items));(*items)->node=19;}
    return result();
}
int main(void)
{
    ULONG count;DTASKMGR_WORKER *items;unsigned i,before;
    expected.binding=(RPC_BINDING_HANDLE)(ULONG_PTR)11;
    expected.process=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(expected.process!=NULL);
    for(i=0;i<4;++i){
        DWORD error;mode=i;count=99;items=(DTASKMGR_WORKER *)(ULONG_PTR)1;
        error=common_rpc_task_snapshot(&expected,&count,&items);
        CHECK(error==(DWORD)(i==1 ? ERROR_ACCESS_DENIED : i==2 ? RPC_S_CALL_FAILED : ERROR_SUCCESS));
        if(i==0){CHECK(count==1 && items && items->process_id==123);MIDL_user_free(items);}
        else CHECK(count==0 && items==NULL);
        CHECK(allocations==0);
        CHECK(common_rpc_close_management_node(&expected,&selected)==error);
        {
            ULONG coverage=99,trace_count=99;
            WORKER_TRACE_NODE *trace=(WORKER_TRACE_NODE *)(ULONG_PTR)1;
            CHECK(common_rpc_worker_task_trace(&expected,&selected,&coverage,&trace_count,&trace)==error);
            if(i==0){CHECK(trace_count==1 && coverage==1 && trace->node==19);MIDL_user_free(trace);}
            else if(i==3)CHECK(!trace_count && !trace && coverage==1);
            else CHECK(!trace_count && !trace && !coverage);
            CHECK(!allocations);
        }
    }
    before=calls;count=99;items=(DTASKMGR_WORKER *)(ULONG_PTR)1;
    CHECK(common_rpc_task_snapshot(NULL,&count,&items)==ERROR_INVALID_STATE);
    CHECK(count==0 && items==NULL);
    CHECK(common_rpc_task_snapshot(&expected,NULL,&items)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_task_snapshot(&expected,&count,NULL)==ERROR_INVALID_PARAMETER);
    CHECK(common_rpc_close_management_node(NULL,&selected)==ERROR_INVALID_STATE);
    CHECK(common_rpc_close_management_node(&expected,NULL)==ERROR_INVALID_PARAMETER);
    CHECK(calls==before);CHECK(allocations==0);
    CHECK(WaitForSingleObject(expected.process,0)==WAIT_TIMEOUT);
    {
        common_rpc_management missing=expected;
        missing.binding=NULL;
        CHECK(common_rpc_task_snapshot(&missing,&count,&items)==ERROR_INVALID_STATE);
        CHECK(common_rpc_close_management_node(&missing,&selected)==ERROR_INVALID_STATE);
        missing=expected;missing.process=NULL;
        CHECK(common_rpc_task_snapshot(&missing,&count,&items)==ERROR_INVALID_STATE);
        CHECK(common_rpc_close_management_node(&missing,&selected)==ERROR_INVALID_STATE);
        CHECK(calls==before);
    }
    mode=3;expected.binding=(RPC_BINDING_HANDLE)(ULONG_PTR)22;
    CHECK(common_rpc_task_snapshot(&expected,&count,&items)==ERROR_SUCCESS);
    CHECK(count==0 && items==NULL);
    CloseHandle(expected.process);
    printf("common-management: %u checks, %u failures, %u allocations\n",checks,failures,allocations);
    return failures ? 1 : 0;
}
