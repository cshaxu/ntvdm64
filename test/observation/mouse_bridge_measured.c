#include "worker_performance.h"
#include "ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.h"
#include <stdio.h>
#include <string.h>
static int measured_push(mvdm_mouse_input *,const mvdm_mouse_input_sample *);
static int measured_take(mvdm_mouse_input *,mvdm_mouse_input_sample *);
static void measured_interrupt(void);
static void measured_snapshot(DWORD);
static DWORD measured_interrupts;
/* Defined by the original NT mouse owner.  The bridge already holds its ICA
 * lock, so this test-only observer reads an internally consistent post-kick
 * debt value without adding a lock, timer or production diagnostic. */
extern ULONG MseIntLazyCount;
#define mvdm_mouse_input_push measured_push
#define mvdm_mouse_input_take measured_take
#define DoMouseInterrupt measured_interrupt
#include "../../src/ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.c"
#undef mvdm_mouse_input_push
#undef mvdm_mouse_input_take
#undef DoMouseInterrupt
extern void DoMouseInterrupt(void);
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
static void measured_interrupt(void)
{
    LONGLONG start;
    if(!worker_performance_enabled()) {
        DoMouseInterrupt();return;
    }
    start=worker_performance_clock();
    DoMouseInterrupt();
    /* Count is the post-call original lazy-IRQ debt.  A nonzero value means
     * this accepted relative sample did not require an additional guest
     * movement record to leave future 10ms EOI work behind. */
    worker_performance_record("mouse-irq-debt",start,(DWORD)MseIntLazyCount,0);
    /* GUI/WOW termination does not necessarily pass the character I/O close
     * wrapper that normally flushes this test collector.  Snapshot only the
     * first and sixty-fourth actual bridge kick: the first proves this input
     * route was reached; the second observes burst debt without adding a
     * production timer, queue policy, or any recurring trace I/O. */
    ++measured_interrupts;
    if(measured_interrupts==1u || measured_interrupts==64u) {
        measured_snapshot((DWORD)MseIntLazyCount);
        /* Preserve the ordinary detailed test collector at the latter
         * checkpoint only. The direct summary above remains useful even when
         * a GUI worker never reaches character-channel teardown. */
        if(measured_interrupts==64u)
            worker_performance_flush();
    }
}
static void measured_snapshot(DWORD debt)
{
    char prefix[MAX_PATH],path[MAX_PATH],text[160];DWORD length,written;
    HANDLE file;
    length=GetEnvironmentVariableA("MVDM_TEST_WORKER_PERFORMANCE",prefix,ARRAYSIZE(prefix));
    if(!length || length>=ARRAYSIZE(prefix))return;
    if(sprintf_s(path,ARRAYSIZE(path),"%s-irq-summary-%lu.txt",prefix,GetCurrentProcessId())<0 ||
        sprintf_s(text,ARRAYSIZE(text),"interrupts=%lu debt=%lu\r\n",measured_interrupts,debt)<0)return;
    file=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return;
    (void)WriteFile(file,text,(DWORD)strlen(text),&written,NULL);
    CloseHandle(file);
}
