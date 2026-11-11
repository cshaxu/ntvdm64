/* Fixture bridge: original BaseSrv stream loops -> formal native receipt
 * adapter -> actual product RPC receiver. No replacement stream algorithm. */
#include "basesrv.h"
#include <base_stream.h>
DWORD test_original_stream_rpc(HANDLE receiver,HANDLE stream,void *context,
    NTSTATUS (*deliver)(void *,HANDLE,uint32_t *),NTSTATUS (*revoke)(void *,uint32_t))
{
    broker_vdm_receipts receipts={0};
    OPENNT_BASE_STREAM_BINDING scope={GetCurrentProcess(),receiver,&receipts,1,context,deliver,revoke};
    OPENNT_BASE_RESOURCE_BINDING resource={&scope,OpenNtBaseDuplicateStream,NULL};
    const OPENNT_BASE_RESOURCE_BINDING *previousResource;
    CSR_PROCESS process={0};CSR_THREAD thread={0};PCSR_THREAD previousThread;
    VDMINFO info={0};DOSRECORD record={0};uint32_t id;
    DWORD error=0;
    if (broker_vdm_receipts_initialize(&receipts,1) ||
        broker_vdm_receipt_accept(&receipts,BROKER_VDM_STDOUT,stream,&id)) return ERROR_INVALID_HANDLE;
    process.ProcessHandle=GetCurrentProcess();thread.Process=&process;
    info.StdOut=info.StdErr=(HANDLE)id;record.lpVDMInfo=&info;
    previousThread=OpenNtBaseBindServerRequestThread(&thread);
    previousResource=OpenNtBaseBindResources(&resource);
    if (BaseSrvDupStandardHandles(receiver,&record)) error=ERROR_INVALID_DATA;
    else {
        if (!info.StdOut || info.StdOut!=info.StdErr) error=ERROR_INVALID_DATA;
        BaseSrvCloseStandardHandles(receiver,&record);
        if (info.StdOut || info.StdErr) error=ERROR_INVALID_DATA;
    }
    OpenNtBaseBindResources(previousResource);
    OpenNtBaseBindServerRequestThread(previousThread);
    broker_vdm_receipts_drain(&receipts);
    return error;
}
