/* Translate the finite BaseSrvDup/CloseStandardHandles boundary; the original
 * ordered loop, stdout/stderr alias comparison and cleanup remain unchanged. */
#include <base_stream.h>
NTSTATUS OpenNtBaseDuplicateStream(void *context,HANDLE sourceProcess,HANDLE source,
    HANDLE targetProcess,PHANDLE target,ACCESS_MASK access,ULONG attributes,ULONG options)
{
    OPENNT_BASE_STREAM_BINDING *binding=context;
    HANDLE resource=NULL;
    uint32_t receipt=0;
    NTSTATUS status;
    if (!binding || !source || access) return (NTSTATUS)0xc000000dL;
    if (options==DUPLICATE_CLOSE_SOURCE) {
        if (sourceProcess!=binding->target_process || targetProcess || target || attributes || !binding->revoke)
            return (NTSTATUS)0xc00000bbL;
        return binding->revoke(binding->context,(uint32_t)source);
    }
    if (options!=DUPLICATE_SAME_ACCESS || attributes!=OBJ_INHERIT || !target ||
        sourceProcess!=binding->source_process || targetProcess!=binding->target_process || !binding->deliver)
        return (NTSTATUS)0xc00000bbL;
    /* Stream-family resolution permits the original stdout/stderr alias.
     * It never treats the source carrier as a native HANDLE. */
    if (broker_vdm_receipt_resolve(binding->source_receipts,binding->source_generation,
        (uint32_t)source,BROKER_VDM_STDIN,&resource)) return (NTSTATUS)0xc0000008L;
    status=binding->deliver(binding->context,resource,&receipt);
    if (status<0) return status;
    if (!receipt) return (NTSTATUS)0xc000000dL;
    *target=(HANDLE)receipt;
    return status;
}
