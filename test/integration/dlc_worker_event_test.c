/* Exact initialization body; real event handles, controlled thread boundary.
 * This does not load DLCAPI or exercise a network adapter. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
static HANDLE DlcWorkerEvent,DlcWorkerCompletionEvent,DlcWorkerThreadHandle;
static unsigned checks,failures,event_calls,fail_event,thread_calls,closes;
static int fail_thread;
static HANDLE owned[3];
static void check(int ok,const char *name){++checks;if(!ok){++failures;printf("FAIL %s\n",name);}}
static VOID VrDlcWorkerThread(LPVOID p){(void)p;}
static HANDLE make_event(LPSECURITY_ATTRIBUTES a,BOOL manual,BOOL initial,LPCSTR name){
    HANDLE h;check(!a&&!manual&&!initial&&!name,"original auto-reset event contract");
    if(++event_calls==fail_event){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;}
    h=CreateEventA(a,manual,initial,name);check(h!=NULL,"native event allocated");
    owned[event_calls-1]=h;return h;
}
static HANDLE make_thread(LPSECURITY_ATTRIBUTES a,SIZE_T size,LPTHREAD_START_ROUTINE proc,LPVOID arg,DWORD flags,LPDWORD id){
    ++thread_calls;check(!a&&!size&&proc==(LPTHREAD_START_ROUTINE)VrDlcWorkerThread&&!arg&&!flags,"original worker launch contract");
    check(DlcWorkerEvent!=NULL&&DlcWorkerCompletionEvent!=NULL,"both events precede thread");
    if(fail_thread){SetLastError(ERROR_NOT_ENOUGH_MEMORY);return NULL;}
    *id=1;owned[2]=CreateEventA(NULL,FALSE,FALSE,NULL);return owned[2];
}
static BOOL release_handle(HANDLE h){
    unsigned i;for(i=0;i<3;i++)if(h&&owned[i]==h){owned[i]=NULL;++closes;return CloseHandle(h);}
    check(0,"close only owned valid handles");return FALSE;
}
#undef CreateEvent
#define CreateEvent make_event
#define CreateThread make_thread
#define CloseHandle release_handle
#include "dlc_worker_exact.h"
#undef CloseHandle
int main(void){
    unsigned mode,i;BOOLEAN result;
    for(mode=0;mode<4;mode++){
        event_calls=thread_calls=closes=0;fail_event=mode==1?1:mode==2?2:0;fail_thread=mode==3;
        DlcWorkerEvent=DlcWorkerCompletionEvent=DlcWorkerThreadHandle=NULL;
        result=InitializeDlcWorkerThread();
        check(result==(mode==0),"initialization result");
        check(thread_calls==(mode==0||mode==3?1u:0u),"no thread after event failure");
        check(closes==(mode==2?1u:mode==3?2u:0u),"failure event retirement count");
        if(mode==0){
            check(WaitForSingleObject(DlcWorkerEvent,0)==WAIT_TIMEOUT,"event initially unsignalled");
            check(SetEvent(DlcWorkerCompletionEvent)&&WaitForSingleObject(DlcWorkerCompletionEvent,0)==WAIT_OBJECT_0&&WaitForSingleObject(DlcWorkerCompletionEvent,0)==WAIT_TIMEOUT,"completion auto-reset behavior");
        }else check(!owned[0]&&!owned[1]&&!owned[2],"no untransferred live handles");
        for(i=0;i<3;i++)if(owned[i]){CloseHandle(owned[i]);owned[i]=NULL;}
    }
    printf("DLC_WORKER_EVENT checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
