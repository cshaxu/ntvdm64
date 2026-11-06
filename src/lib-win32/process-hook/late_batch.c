#include "late_batch.h"
#include "minhook/src/buffer.h"
DWORD process_hook_late_batch_begin(process_hook_late_batch *batch,process_hook_late_patch *patches,process_hook_late_patch **items,DWORD capacity)
{
    if(!batch || !patches || !items || !capacity || capacity>PROCESS_HOOK_LATE_PATCH_LIMIT)return ERROR_INVALID_PARAMETER;
    if(batch->sticky_error)return batch->sticky_error;
    if(batch->installed || batch->count)return ERROR_INVALID_STATE;
    batch->patches=patches;batch->items=items;batch->capacity=capacity;InitializeBuffer();return ERROR_SUCCESS;
}
DWORD process_hook_late_batch_prepare(process_hook_late_batch *batch,void *slot,void *replacement)
{
    if(!batch || !batch->patches || !batch->items)return ERROR_INVALID_PARAMETER;
    if(batch->sticky_error)return batch->sticky_error;
    if(batch->installed)return ERROR_INVALID_STATE;
    if(batch->count>=batch->capacity)return ERROR_INSUFFICIENT_BUFFER;
    DWORD error=process_hook_late_patch_prepare(&batch->patches[batch->count],slot,replacement);
    if(!error){batch->items[batch->count]=&batch->patches[batch->count];++batch->count;}
    return error;
}
DWORD process_hook_late_batch_commit(process_hook_late_batch *batch,DWORD error,DWORD attempts,DWORD delay_ms)
{
    if(!batch || !batch->patches || !batch->items || !attempts)return ERROR_INVALID_PARAMETER;
    if(batch->sticky_error)return batch->sticky_error;
    if(batch->installed)return ERROR_INVALID_STATE;
    if(!error){
        error=ERROR_BUSY;
        for(DWORD i=0;i<attempts && (error==ERROR_BUSY || error==ERROR_RETRY);++i){
            error=process_hook_late_patch_install_batch(&batch->guard,batch->items,batch->count);
            if((error==ERROR_BUSY || error==ERROR_RETRY) && delay_ms)Sleep(delay_ms);
        }
    }
    if(!error){batch->installed=TRUE;return ERROR_SUCCESS;}
    BOOL any=FALSE;for(DWORD i=0;i<batch->count;++i)if(batch->patches[i].installed)any=TRUE;
    if(batch->guard.threads || batch->guard.snapshots || any){batch->sticky_error=error;return error;}
    for(DWORD i=0;i<batch->count;++i)(void)process_hook_late_patch_discard(&batch->patches[i]);
    batch->count=0;return error;
}
