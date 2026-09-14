/* Original BaseSrvCreate/ClosePairWaitHandles keep creation and cleanup order.
 * This native TU translates only their synchronous remote resource boundary. */
#include <base_wait.h>
NTSTATUS OpenNtBaseDuplicateWait(void *context,HANDLE sourceProcess,HANDLE source,
    HANDLE targetProcess,PHANDLE target,ACCESS_MASK access,ULONG attributes,ULONG options)
{
    OPENNT_BASE_WAIT_BINDING *binding=context;
    NTSTATUS status;
    uint32_t receipt=0;
    if (!binding || !source || access || attributes) return (NTSTATUS)0xc000000dL;
    if (options==DUPLICATE_CLOSE_SOURCE) {
        if (sourceProcess!=binding->target_process || targetProcess || target || !binding->revoke)
            return (NTSTATUS)0xc00000bbL;
        return binding->revoke(binding->context,(uint32_t)source);
    }
    if (options!=DUPLICATE_SAME_ACCESS || sourceProcess!=NtCurrentProcess() ||
        targetProcess!=binding->target_process || !target || !binding->deliver || binding->local_event)
        return (NTSTATUS)0xc00000bbL;
    /* Save before delivery so original failure cleanup can close its event. */
    binding->local_event=source;
    status=binding->deliver(binding->context,source,&receipt);
    if (status<0) return status;
    if (!receipt) return (NTSTATUS)0xc000000dL;
    *target=(HANDLE)receipt;
    return status;
}
NTSTATUS OpenNtBaseCloseWait(void *context,HANDLE event)
{
    OPENNT_BASE_WAIT_BINDING *binding=context;
    NTSTATUS status;
    if (!binding || !event || event!=binding->local_event) return (NTSTATUS)0xc0000008L;
    status=NtClose(event);
    if (status>=0) binding->local_event=NULL;
    return status;
}
