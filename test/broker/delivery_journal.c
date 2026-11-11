#include "vdm_delivery.h"
#include "vdm_receipt.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x); return 1; } } while(0)
typedef struct receiver { broker_vdm_receipts receipts; DWORD failure,calls; } receiver;
static DWORD revoke(void *context,uint32_t generation,uint32_t id)
{
    receiver *state=context;
    ++state->calls;
    if (state->failure) return state->failure;
    return broker_vdm_receipt_revoke(&state->receipts,generation,id);
}
int main(void)
{
    receiver peer={0};
    broker_vdm_delivery journal={0};
    broker_vdm_delivery_item *first,*second;
    uint32_t id,invalid;
    HANDLE event=CreateEventW(NULL,TRUE,FALSE,NULL),borrowed,taken,alias,readPipe,writePipe;
    DWORD flags;
    CHECK(event!=NULL);
    CHECK(!broker_vdm_receipts_initialize(&peer.receipts,7));
    CHECK(!broker_vdm_delivery_prepare(&journal,&peer,revoke,&first));
    CHECK(!broker_vdm_receipt_accept(&peer.receipts,BROKER_VDM_PARENT_WAIT,event,&id));
    CHECK(!broker_vdm_delivery_acknowledge(first,7,id));
    CHECK(!broker_vdm_delivery_prepare(&journal,&peer,revoke,&second));
    CHECK(broker_vdm_receipt_accept(&peer.receipts,BROKER_VDM_WORKER_WAIT,NULL,&invalid)==ERROR_INVALID_HANDLE);
    CHECK(!broker_vdm_delivery_forget(&journal,second)); /* Proven non-delivery. */
    CHECK(!broker_vdm_delivery_rollback(&journal) && !journal.pending && peer.calls==1);
    CHECK(broker_vdm_receipt_resolve(&peer.receipts,7,id,BROKER_VDM_PARENT_WAIT,&borrowed)==ERROR_NOT_FOUND);
    CHECK(SetEvent(event)); /* Rollback did not close the sender's original. */
    CHECK(!broker_vdm_delivery_prepare(&journal,&peer,revoke,&first));
    CHECK(broker_vdm_delivery_commit(&journal)==ERROR_IO_PENDING);
    CHECK(broker_vdm_delivery_rollback(&journal)==ERROR_IO_PENDING && journal.pending==first);
    CHECK(!broker_vdm_receipt_accept(&peer.receipts,BROKER_VDM_PARENT_WAIT,event,&id));
    CHECK(!broker_vdm_delivery_acknowledge(first,7,id));
    peer.failure=RPC_S_CALL_FAILED;
    CHECK(broker_vdm_delivery_rollback(&journal)==RPC_S_CALL_FAILED && journal.pending==first);
    peer.failure=0;
    CHECK(!broker_vdm_delivery_rollback(&journal) && !journal.pending);
    CHECK(!broker_vdm_delivery_prepare(&journal,&peer,revoke,&first));
    CHECK(!broker_vdm_receipt_accept(&peer.receipts,BROKER_VDM_PARENT_WAIT,event,&id));
    CHECK(!broker_vdm_delivery_acknowledge(first,7,id));
    CHECK(!broker_vdm_delivery_commit(&journal) && !journal.pending);
    CHECK(broker_vdm_receipt_resolve(&peer.receipts,7,id,BROKER_VDM_WORKER_WAIT,&borrowed)==ERROR_ACCESS_DENIED && !borrowed);
    CHECK(broker_vdm_receipt_resolve(&peer.receipts,7,id,BROKER_VDM_STDIN,&borrowed)==ERROR_ACCESS_DENIED && !borrowed);
    CHECK(broker_vdm_receipt_resolve(&peer.receipts,7,id,0,&borrowed)==ERROR_INVALID_PARAMETER && !borrowed);
    CHECK(!broker_vdm_receipt_resolve(&peer.receipts,7,id,BROKER_VDM_PARENT_WAIT,&borrowed));
    CHECK(WaitForSingleObject(borrowed,0)==WAIT_OBJECT_0);
    CHECK(GetHandleInformation(borrowed,&flags) && !(flags&HANDLE_FLAG_INHERIT));
    CHECK(broker_vdm_receipt_take(&peer.receipts,8,id,BROKER_VDM_PARENT_WAIT,&taken)==ERROR_ACCESS_DENIED && !taken);
    CHECK(broker_vdm_receipt_take(&peer.receipts,7,id,BROKER_VDM_WORKER_WAIT,&taken)==ERROR_ACCESS_DENIED && !taken);
    CHECK(!broker_vdm_receipt_take(&peer.receipts,7,id,BROKER_VDM_PARENT_WAIT,&taken) && taken==borrowed);
    CHECK(broker_vdm_receipt_take(&peer.receipts,7,id,BROKER_VDM_PARENT_WAIT,&alias)==ERROR_NOT_FOUND && !alias);
    CHECK(!broker_vdm_receipt_revoke(&peer.receipts,7,id));
    broker_vdm_receipts_drain(&peer.receipts);
    CHECK(WaitForSingleObject(taken,0)==WAIT_OBJECT_0 && CloseHandle(taken));
    CHECK(SetEvent(event) && CloseHandle(event));
    CHECK(CreatePipe(&readPipe,&writePipe,NULL,0));
    CHECK(!broker_vdm_receipts_initialize(&peer.receipts,8));
    CHECK(!broker_vdm_receipt_accept(&peer.receipts,BROKER_VDM_STDOUT,writePipe,&id));
    CHECK(!broker_vdm_receipt_resolve(&peer.receipts,8,id,BROKER_VDM_STDERR,&alias));
    CHECK(!broker_vdm_receipt_take(&peer.receipts,8,id,BROKER_VDM_STDOUT,&taken) && taken==alias);
    CHECK(GetHandleInformation(taken,&flags) && (flags&HANDLE_FLAG_INHERIT));
    CHECK(CloseHandle(writePipe));
    broker_vdm_receipts_drain(&peer.receipts);
    {
        char byte=0;
        DWORD bytes;
        CHECK(WriteFile(taken,"x",1,&bytes,NULL) && bytes==1);
        CHECK(ReadFile(readPipe,&byte,1,&bytes,NULL) && bytes==1 && byte=='x');
        CHECK(CloseHandle(taken));
        CHECK(!ReadFile(readPipe,&byte,1,&bytes,NULL) && GetLastError()==ERROR_BROKEN_PIPE);
    }
    CHECK(CloseHandle(readPipe));
    puts("PASS: receipt ownership handoff preserves wait/stream identity, alias, inheritability and pipe EOF; drain does not double-close");
    puts("PASS: partial acquisition rollback, unknown/retry retention, successful ownership commit and sender survival");
    return 0;
}
