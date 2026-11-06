/* Exact original queue/interrupt bodies; real locks, controlled allocation
 * and ICA boundary. Does not execute a guest ISR or a network provider. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
typedef struct disposition {struct disposition *Next;VOID (*AsyncDispositionRoutine)(VOID);} VR_ASYNC_DISPOSITION;
static CRITICAL_SECTION AsyncEventQueueCritSec,QueuedInterruptCritSec;
static VR_ASYNC_DISPOSITION *AsyncEventQueueHead,*AsyncEventQueueTail;
static LONG QueuedInterrupts=-1;
static BOOLEAN FrozenVdmContext=FALSE;
static unsigned checks,failures,alloc_fail,live,interrupts,callbacks,sequence;
static void check(int ok,const char *label){++checks;if(!ok){++failures;printf("FAIL %s\n",label);}}
static HLOCAL allocate(UINT flags,SIZE_T bytes){HLOCAL p;if(alloc_fail)return NULL;p=LocalAlloc(flags,bytes);if(p)++live;return p;}
static HLOCAL release(HLOCAL p){if(p)--live;return LocalFree(p);}
static void interrupt(int ica,int line,int count){(void)ica;(void)line;check(count==1,"one ICA interrupt");++interrupts;}
#define IF_DEBUG(x) if(0)
#define CRITDUMP(x) ((void)0)
#define DBGPRINT(...) ((void)0)
#define NETWORK_ICA 1
#define NETWORK_LINE 2
#define LocalAlloc allocate
#define LocalFree release
#define call_ica_hw_interrupt interrupt
#include "redir_queue_exact.h"
#undef LocalAlloc
#undef LocalFree
static void first(void){check(sequence==0,"first callback order");sequence=1;++callbacks;}
static void second(void){check(sequence==1,"second callback order");sequence=2;++callbacks;}
int main(void){DWORD exception=0;
 InitializeCriticalSection(&AsyncEventQueueCritSec);InitializeCriticalSection(&QueuedInterruptCritSec);
 VrQueueCompletionHandler(first);VrRaiseInterrupt();VrQueueCompletionHandler(second);VrRaiseInterrupt();
 check(live==2&&interrupts==1&&QueuedInterrupts==1,"two queued one delivered IRQ");
 VrHandleAsyncCompletion();VrDismissInterrupt();VrHandleAsyncCompletion();VrDismissInterrupt();
 check(callbacks==2&&sequence==2&&!live&&QueuedInterrupts==-1&&!AsyncEventQueueHead,"normal queue retired and counter idle");
 alloc_fail=1;VrQueueCompletionHandler(first);VrRaiseInterrupt();
 check(!AsyncEventQueueHead&&QueuedInterrupts==0,"original caller raises after failed enqueue");
 __try{VrHandleAsyncCompletion();}__except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
 check(exception==EXCEPTION_ACCESS_VIOLATION,"reproduce failed-enqueue empty dequeue access violation");
 /* Fault unwinds inside the original queue lock; retire fixture ownership. */
 if(exception)LeaveCriticalSection(&AsyncEventQueueCritSec);
 DeleteCriticalSection(&AsyncEventQueueCritSec);DeleteCriticalSection(&QueuedInterruptCritSec);
 printf("REDIR_ASYNC_BASELINE checks=%u failures=%u exception=%08lx\n",checks,failures,exception);
 return failures?1:0;
}
