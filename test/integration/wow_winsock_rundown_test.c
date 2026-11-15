/* Exact production rundown/completion bodies and context layout, with a
 * controlled provider. Quarantine allocations expose early/double release. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
typedef WORD HAND16;
typedef DWORD VPVOID;
#include "winsock_context_exact.h"
static LIST_ENTRY WWS32AsyncContextBlockListHead;
static CRITICAL_SECTION WWS32CriticalSection;
static struct {ULONG (*lpfn)();} wsockapis[2];
#define WOW_WSACANCELASYNCREQUEST 0
#define WOW_WSAGETLASTERROR 1
static unsigned checks,failures,cancel_calls;
static int error_code,reentrant,complete_other;
static struct {PWINSOCK_ASYNC_CONTEXT_BLOCK p;int active,buffreed,ctxfreed;} owned[3];
static void check(int ok,const char *s){++checks;if(!ok){++failures;printf("FAIL %s\n",s);}}
static void tracked_free(void *p){int i;for(i=0;i<3;i++){
 if(p==owned[i].p->Buffer32){check(!owned[i].active,"provider finished before buffer release");check(!owned[i].buffreed++,"buffer released once");return;}
 if(p==owned[i].p){check(!owned[i].ctxfreed++,"context released once");return;}
 }check(0,"known allocation");}
static void list_init(PLIST_ENTRY h){h->Flink=h->Blink=h;}
static void list_remove(PLIST_ENTRY e){e->Blink->Flink=e->Flink;e->Flink->Blink=e->Blink;}
static void list_insert(PLIST_ENTRY h,PLIST_ENTRY e){e->Flink=h;e->Blink=h->Blink;h->Blink->Flink=e;h->Blink=e;}
#define InitializeListHead list_init
#define RemoveEntryList list_remove
#define InsertTailList list_insert
#define RtlEnterCriticalSection EnterCriticalSection
#define RtlLeaveCriticalSection LeaveCriticalSection
#define free_w tracked_free
#define WWS32_MESSAGE_ASYNC_SELECT 0
#define WWS32_MESSAGE_ASYNC_GETHOST 1
#define WWS32_MESSAGE_ASYNC_GETPROTO 2
#define WWS32_MESSAGE_ASYNC_GETSERV 3
#define ASSERT(x) check(!!(x), #x)
static BOOL conversion(HWND,UINT,WPARAM,LPARAM);
#define WWS32PostAsyncSelect conversion
#define WWS32PostAsyncGetHost conversion
#define WWS32PostAsyncGetProto conversion
#define WWS32PostAsyncGetServ conversion
static BOOL (*WWS32PostDispatchTable[])(HWND,UINT,WPARAM,LPARAM)={conversion,conversion,conversion,conversion};
#include "winsock_rundown_exact.h"
static HANDLE converting,release_conversion;
static BOOL conversion(HWND window,UINT message,WPARAM value,LPARAM failure) {
 (void)window;(void)message;(void)value;
 if(failure)RaiseException(0xe0420001,0,0,NULL);
 SetEvent(converting);
 if(WaitForSingleObject(release_conversion,5000)!=WAIT_OBJECT_0) return FALSE;
 return TRUE;
}
static DWORD WINAPI dispatch_thread(void *unused) {
 (void)unused;return WWS32DispatchPostMessage(NULL,1,0,0)?0:1;
}
static DWORD WINAPI lock_thread(void *unused) {
 BOOL acquired=TryEnterCriticalSection(&WWS32CriticalSection);
 (void)unused;if(acquired)LeaveCriticalSection(&WWS32CriticalSection);
 return acquired?0:1;
}
static void dispatch_contract(void) {
 HANDLE thread;DWORD code=1;BOOL acquired,caught=FALSE;
 converting=CreateEvent(NULL,TRUE,FALSE,NULL);release_conversion=CreateEvent(NULL,TRUE,FALSE,NULL);
 thread=CreateThread(NULL,0,dispatch_thread,NULL,0,NULL);
 check(thread&&WaitForSingleObject(converting,5000)==WAIT_OBJECT_0,"completion entered converter");
 acquired=TryEnterCriticalSection(&WWS32CriticalSection);
 check(!acquired,"completion retains retirement lock through buffer conversion");
 if(acquired)LeaveCriticalSection(&WWS32CriticalSection);
 SetEvent(release_conversion);
 check(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0,"completion returns without deadlock");
 GetExitCodeThread(thread,&code);check(!code,"dispatcher preserves converter result");CloseHandle(thread);
 __try {WWS32DispatchPostMessage(NULL,1,0,1);}
 __except(GetExceptionCode()==0xe0420001?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){caught=TRUE;}
 check(caught,"converter exception propagates");
 thread=CreateThread(NULL,0,lock_thread,NULL,0,NULL);
 check(thread&&WaitForSingleObject(thread,5000)==WAIT_OBJECT_0,"exception unlock witness completes");
 GetExitCodeThread(thread,&code);check(!code,"finally releases lock for another thread");CloseHandle(thread);
 CloseHandle(converting);CloseHandle(release_conversion);
}

static void complete(int i){owned[i].active=0;check(WWS32FindAndRemoveAsyncContext(owned[i].p->AsyncTaskHandle32)==NULL,"retired completion suppresses guest delivery");}
static ULONG cancel(HANDLE h){int i;for(i=0;i<3&&owned[i].p->AsyncTaskHandle32!=h;i++);check(i<3,"known cancel handle");if(i==3)return -1;
 ++cancel_calls;
 if(reentrant){complete(i);check(!owned[i].ctxfreed,"active cancel retains context across completion");}
 if(complete_other&&i==0){complete(1);check(owned[1].ctxfreed==1,"other retired request completes during cancel");}
 if(!error_code||error_code==WSAEALREADY||error_code==WSAEINVAL)owned[i].active=0;
 return error_code?(ULONG)-1:0;
}
static ULONG last_error(void){return error_code;}
static void setup(int error,int callback,int other){int i;
 list_init(&WWS32AsyncContextBlockListHead);cancel_calls=0;error_code=error;reentrant=callback;complete_other=other;
 for(i=0;i<3;i++){owned[i].p=calloc(1,sizeof(*owned[i].p));owned[i].p->Buffer32=calloc(1,256);owned[i].p->ThreadSerialNumber=i==2?12:11;owned[i].p->AsyncTaskHandle32=(HANDLE)(ULONG_PTR)(7+i);owned[i].p->AsyncTaskHandle16=(HAND16)(i==1?102:101);owned[i].active=1;owned[i].buffreed=owned[i].ctxfreed=0;list_insert(&WWS32AsyncContextBlockListHead,&owned[i].p->ContextBlockListEntry);}
}
static void finish(void){int i;for(i=0;i<3;i++){free(owned[i].p->Buffer32);free(owned[i].p);}}
int main(void){int errors[]={0,WSAEALREADY,WSAEINVAL,WSAEINPROGRESS,WSAENETDOWN,WSANOTINITIALISED};int e,r,i;HANDLE native;PWINSOCK_ASYNC_CONTEXT_BLOCK context;
 InitializeCriticalSection(&WWS32CriticalSection);wsockapis[0].lpfn=(ULONG(*)())cancel;wsockapis[1].lpfn=last_error;
 setup(0,0,0);
 native=WWS32FindAsyncTaskHandle32(101,11);
 check(native==(HANDLE)7,"alias resolves to native request for its task");
 check(WWS32FindAsyncTaskHandle32(101,12)==(HANDLE)9,"same alias retains other task identity");
 check(!WWS32FindAsyncTaskHandle32(101,99)&&!WWS32FindAsyncTaskHandle32(999,11),"unknown owner or alias rejected");
 check(cancel(native)==0,"explicit native cancellation succeeds");
 context=WWS32FindAndRemoveAsyncContext(native);
 check(context==owned[0].p,"original native lookup removes cancelled request");
 if(context){tracked_free(context->Buffer32);tracked_free(context);}
 check(!WWS32FindAsyncTaskHandle32(101,11),"cancelled alias disappears");
 check(WWS32FindAsyncTaskHandle32(101,12)==(HANDLE)9&&!owned[2].ctxfreed,"sibling alias survives cancellation");
 complete(0);check(owned[0].buffreed==1&&owned[0].ctxfreed==1,"late explicit completion does not release twice");
 finish();
 for(e=0;e<6;e++)for(r=0;r<2;r++){
  setup(errors[e],r,0);WWS32CancelAsyncRequestsForTask(99);check(!cancel_calls,"unrelated task ignored");WWS32CancelAsyncRequestsForTask(11);
  check(cancel_calls==2,"both task requests cancelled once");check(!owned[2].buffreed&&!owned[2].p->RundownState,"other task remains live");
  for(i=0;i<2;i++){
   if(e>=3&&!r){check(!owned[i].buffreed&&!owned[i].ctxfreed,"unresolved provider buffer retained");((char*)owned[i].p->Buffer32)[0]='x';}
   else check(owned[i].buffreed==1&&owned[i].ctxfreed==1,"terminal request retired");
  }
  WWS32CancelAsyncRequestsForTask(11);check(cancel_calls==2,"rundown does not retry provider");
  for(i=0;i<2;i++){complete(i);check(owned[i].buffreed==1&&owned[i].ctxfreed==1,"late completion retires exactly once");complete(i);}
  owned[2].active=0;check(WWS32FindAndRemoveAsyncContext((HANDLE)9)==owned[2].p,"ordinary completion returns live context");tracked_free(owned[2].p->Buffer32);tracked_free(owned[2].p);finish();
 }
 setup(WSAEINPROGRESS,0,1);WWS32CancelAsyncRequestsForTask(11);check(cancel_calls==1,"completed sibling not cancelled again");complete(0);finish();
 dispatch_contract();
 DeleteCriticalSection(&WWS32CriticalSection);printf("WOW_WINSOCK_RUNDOWN checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
