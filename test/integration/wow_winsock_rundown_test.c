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
#include "winsock_rundown_exact.h"
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
 for(i=0;i<3;i++){owned[i].p=calloc(1,sizeof(*owned[i].p));owned[i].p->Buffer32=calloc(1,256);owned[i].p->ThreadSerialNumber=i==2?12:11;owned[i].p->AsyncTaskHandle32=(HANDLE)(ULONG_PTR)(7+i);owned[i].active=1;owned[i].buffreed=owned[i].ctxfreed=0;list_insert(&WWS32AsyncContextBlockListHead,&owned[i].p->ContextBlockListEntry);}
}
static void finish(void){int i;for(i=0;i<3;i++){free(owned[i].p->Buffer32);free(owned[i].p);}}
int main(void){int errors[]={0,WSAEALREADY,WSAEINVAL,WSAEINPROGRESS,WSAENETDOWN,WSANOTINITIALISED};int e,r,i;
 InitializeCriticalSection(&WWS32CriticalSection);wsockapis[0].lpfn=(ULONG(*)())cancel;wsockapis[1].lpfn=last_error;
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
 DeleteCriticalSection(&WWS32CriticalSection);printf("WOW_WINSOCK_RUNDOWN checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
