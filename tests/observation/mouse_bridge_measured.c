#include "worker_performance.h"
#include "ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.h"
static int measured_push(mvdm_mouse_input *,const mvdm_mouse_input_sample *);
static int measured_take(mvdm_mouse_input *,mvdm_mouse_input_sample *);
#define mvdm_mouse_input_push measured_push
#define mvdm_mouse_input_take measured_take
#include "../../src/ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.c"
#undef mvdm_mouse_input_push
#undef mvdm_mouse_input_take
/* Both call sites already hold the original ICA lock. Do not add an input
 * scheduler or replace the actual production queue/IRQ implementation. */
static int measured_push(mvdm_mouse_input *queue,const mvdm_mouse_input_sample *sample)
{
    DWORD before;LONGLONG start;int result;
    if(!worker_performance_enabled())return mvdm_mouse_input_push(queue,sample);
    before=queue->count;start=worker_performance_clock();
    result=mvdm_mouse_input_push(queue,sample);
    worker_performance_push(queue,before,result,start);return result;
}
static int measured_take(mvdm_mouse_input *queue,mvdm_mouse_input_sample *sample)
{
    DWORD before,head;int result;
    if(!worker_performance_enabled())return mvdm_mouse_input_take(queue,sample);
    before=queue->count;head=queue->head;result=mvdm_mouse_input_take(queue,sample);
    worker_performance_take(queue,head,before,result,worker_performance_clock());return result;
}
