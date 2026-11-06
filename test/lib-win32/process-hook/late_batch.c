#include "lib-win32/process-hook/late_batch.h"
#include <stdio.h>
typedef int (*sample_fn)(int);
#ifdef _MSC_VER
__declspec(noinline) static int sample(int n){return n+7;}
#else
static __attribute__((noinline)) int sample(int n){return n+7;}
#endif
static sample_fn original;static sample_fn volatile call;
static int detour(int n){return original(n)+100;}
int main(void)
{
    process_hook_late_batch batch={0};process_hook_late_patch patches[1]={{0}};process_hook_late_patch *items[1]={0};
    process_hook_late_batch boundary={0};
    process_hook_late_patch boundary_patches[PROCESS_HOOK_LATE_PATCH_LIMIT]={{0}};
    process_hook_late_patch *boundary_items[PROCESS_HOOK_LATE_PATCH_LIMIT]={0};
    if(process_hook_late_batch_begin(&boundary,boundary_patches,boundary_items,0)!=ERROR_INVALID_PARAMETER ||
       process_hook_late_batch_begin(&boundary,boundary_patches,boundary_items,PROCESS_HOOK_LATE_PATCH_LIMIT) ||
       boundary.capacity!=PROCESS_HOOK_LATE_PATCH_LIMIT ||
       process_hook_late_batch_commit(&boundary,ERROR_ACCESS_DENIED,1,0)!=ERROR_ACCESS_DENIED || boundary.count)return 9;
    if(process_hook_late_batch_begin(&batch,patches,items,PROCESS_HOOK_LATE_PATCH_LIMIT+1)!=ERROR_INVALID_PARAMETER || batch.patches)return 8;
    original=sample;call=sample;
    if(process_hook_late_batch_begin(&batch,patches,items,1) || process_hook_late_batch_prepare(&batch,&original,(void *)detour))return 1;
    if(process_hook_late_batch_prepare(&batch,&original,(void *)detour)!=ERROR_INSUFFICIENT_BUFFER)return 2;
    if(process_hook_late_batch_commit(&batch,ERROR_ACCESS_DENIED,100,1)!=ERROR_ACCESS_DENIED || batch.count || original!=sample || call(5)!=12)return 3;
    if(process_hook_late_batch_begin(&batch,patches,items,1) || process_hook_late_batch_prepare(&batch,&original,(void *)detour))return 4;
    DWORD error=process_hook_late_batch_commit(&batch,ERROR_SUCCESS,100,1);
    if(error || !batch.installed || call(5)!=112 || original(5)!=12)return 5;
    if(process_hook_late_batch_begin(&batch,patches,items,1)!=ERROR_INVALID_STATE ||
       process_hook_late_batch_prepare(&batch,&original,(void *)detour)!=ERROR_INVALID_STATE ||
       process_hook_late_batch_commit(&batch,0,1,0)!=ERROR_INVALID_STATE ||
       process_hook_late_patch_discard(&patches[0])!=ERROR_INVALID_STATE)return 6;
    process_hook_late_batch sticky={.patches=patches,.items=items,.capacity=1,.sticky_error=ERROR_ACCESS_DENIED};
    if(process_hook_late_batch_begin(&sticky,patches,items,1)!=ERROR_ACCESS_DENIED ||
       process_hook_late_batch_prepare(&sticky,&original,(void *)detour)!=ERROR_ACCESS_DENIED ||
       process_hook_late_batch_commit(&sticky,0,1,0)!=ERROR_ACCESS_DENIED)return 7;
    printf("PROCESS HOOK LATE BATCH PASS bits=%u abort=1 retry=1 installed=1 retained=1 sticky=1 limit=1 boundary_0_88_89=1\n",(unsigned)(sizeof(void*)*8));return 0;
}
