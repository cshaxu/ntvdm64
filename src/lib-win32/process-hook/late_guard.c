#include "late_guard.h"
#include <processsnapshot.h>
#include <stdint.h>

/* Process snapshot resources are prepared before suspension,
 * and the verification snapshot is disposed only after all threads resume. */
typedef struct snap { HPSS value; HPSSWALK marker; } snap;
typedef struct snapshot_owner { snap first,verify; } snapshot_owner;
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
static DWORD fail_after=MAXDWORD;
static void (*after_suspend)(void);
static void (*before_suspend)(void);
static DWORD fail_snapshot_free;
void process_hook_late_guard_test_fail_after(DWORD n){fail_after=n;}
void process_hook_late_guard_test_after_suspend(void (*f)(void)){after_suspend=f;}
void process_hook_late_guard_test_before_suspend(void (*f)(void)){before_suspend=f;}
void process_hook_late_guard_test_fail_free(DWORD times){fail_snapshot_free=times;}
#endif
static DWORD dispose(snap *s) {
    DWORD e=ERROR_SUCCESS;
    if(s->value){
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
        if(fail_snapshot_free){--fail_snapshot_free;e=ERROR_ACCESS_DENIED;}
        else
#endif
        e=PssFreeSnapshot(GetCurrentProcess(),s->value);
        if(!e)s->value=NULL;
    }
    if(s->marker){DWORD x=PssWalkMarkerFree(s->marker);if(!x)s->marker=NULL;if(!e)e=x;}
    return e;
}
static DWORD begin_snap(snap *s) {
    DWORD e=PssWalkMarkerCreate(NULL,&s->marker);if(e)return e;
    return PssCaptureSnapshot(GetCurrentProcess(),PSS_CAPTURE_THREADS,0,&s->value);
}
static DWORD next(snap *s,PSS_THREAD_ENTRY *entry) {
    return PssWalkSnapshot(s->value,PSS_WALK_THREADS,s->marker,entry,sizeof(*entry));
}
static DWORD thread_identity(HANDLE thread,DWORD pid,DWORD tid,FILETIME created) {
    FILETIME actual,exited,kernel,user;
    if(!thread || GetThreadId(thread)!=tid || GetProcessIdOfThread(thread)!=pid)return ERROR_RETRY;
    if(WaitForSingleObject(thread,0)!=WAIT_TIMEOUT || !GetThreadTimes(thread,&actual,&exited,&kernel,&user))return ERROR_RETRY;
    if(actual.dwLowDateTime!=created.dwLowDateTime || actual.dwHighDateTime!=created.dwHighDateTime)return ERROR_RETRY;
    return ERROR_SUCCESS;
}
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
DWORD process_hook_late_guard_test_identity(HANDLE thread,DWORD pid,DWORD tid,FILETIME created)
{return thread_identity(thread,pid,tid,created);}
#endif
static DWORD verify(const process_hook_late_guard *g,DWORD pid,DWORD self) {
    snapshot_owner *owner=g->snapshots;snap *verify_snap=&owner->verify;
    DWORD e=PssCaptureSnapshot(GetCurrentProcess(),PSS_CAPTURE_THREADS,0,&verify_snap->value),found=0;
    if(e)return e;
    PSS_THREAD_ENTRY t;
    while(!(e=next(verify_snap,&t))){
        if(t.ProcessId==pid && t.ThreadId!=self && !(t.Flags&PSS_THREAD_FLAGS_TERMINATED)){
            DWORD i=0;while(i<g->count && GetThreadId(g->threads[i])!=t.ThreadId)++i;
            if(i==g->count)return ERROR_RETRY;
            if(thread_identity(g->threads[i],pid,t.ThreadId,t.CreateTime))return ERROR_RETRY;
            ++found;
        }
    }
    if(e!=ERROR_NO_MORE_ITEMS)return e;
    return found==g->count?ERROR_SUCCESS:ERROR_RETRY;
}
DWORD process_hook_late_guard_end(process_hook_late_guard *g) {
    DWORD e=ERROR_SUCCESS;if(!g)return ERROR_INVALID_PARAMETER;
    for(DWORD i=0;i<g->suspended;++i){
        if(g->threads[i] && ResumeThread(g->threads[i])==(DWORD)-1){if(!e)e=GetLastError();}
        else if(g->threads[i]){CloseHandle(g->threads[i]);g->threads[i]=NULL;}
    }
    if(e)return e;
    snapshot_owner *owner=g->snapshots;
    if(owner){DWORD de=dispose(&owner->first);DWORD ve=dispose(&owner->verify);if(de)e=de;else if(ve)e=ve;}
    for(DWORD i=g->suspended;i<g->count;++i)if(g->threads[i])CloseHandle(g->threads[i]);
    if(g->threads)HeapFree(GetProcessHeap(),0,g->threads);
    g->threads=NULL;g->count=0;g->suspended=0;
    if(e)return e;
    if(owner)HeapFree(GetProcessHeap(),0,owner);
    ZeroMemory(g,sizeof(*g));return e;
}
DWORD process_hook_late_guard_begin(process_hook_late_guard *g,const process_hook_late_region *regions,DWORD count) {
    if(!g || g->threads || g->count || g->suspended || g->snapshots || (count&&!regions))return ERROR_INVALID_PARAMETER;
    for(DWORD i=0;i<count;++i)if(!regions[i].address || !regions[i].bytes || (uintptr_t)regions[i].address>UINTPTR_MAX-regions[i].bytes)return ERROR_INVALID_PARAMETER;
    snapshot_owner *owner=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*owner));if(!owner)return ERROR_NOT_ENOUGH_MEMORY;
    g->snapshots=owner;snap *first=&owner->first,*verify_snap=&owner->verify;
    DWORD e=begin_snap(first),capacity=0,pid=GetCurrentProcessId(),self=GetCurrentThreadId();PSS_THREAD_ENTRY t;
    if(e)goto done;
    /* PSS can retain terminated threads while another handle references them.
     * They cannot execute patched code and must not poison every retry. */
    while(!(e=next(first,&t)))if(t.ProcessId==pid && t.ThreadId!=self && !(t.Flags&PSS_THREAD_FLAGS_TERMINATED))++capacity;
    if(e!=ERROR_NO_MORE_ITEMS)goto done;
    e=ERROR_SUCCESS;
    if(capacity){g->threads=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(SIZE_T)capacity*sizeof(HANDLE));if(!g->threads){e=ERROR_NOT_ENOUGH_MEMORY;goto done;}}
    e=PssWalkMarkerSeekToBeginning(first->marker);if(e)goto done;
    while(!(e=next(first,&t)))if(t.ProcessId==pid && t.ThreadId!=self && !(t.Flags&PSS_THREAD_FLAGS_TERMINATED)){
        if(g->count==capacity){e=ERROR_MORE_DATA;goto done;}
        HANDLE h=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,t.ThreadId);
        if(!h){e=GetLastError();goto done;}
        e=thread_identity(h,pid,t.ThreadId,t.CreateTime);
        if(e){CloseHandle(h);goto done;}
        g->threads[g->count++]=h;
    }
    if(e!=ERROR_NO_MORE_ITEMS)goto done;
    e=dispose(first);if(e)goto done;
    e=PssWalkMarkerCreate(NULL,&verify_snap->marker);if(e)goto done;
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
    if(before_suspend)before_suspend();
#endif
    for(DWORD i=0;i<g->count;++i){
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
        if(i==fail_after){e=ERROR_ACCESS_DENIED;goto done;}
#endif
        if(SuspendThread(g->threads[i])==(DWORD)-1){e=GetLastError();goto done;}++g->suspended;
    }
    for(DWORD i=0;i<g->suspended;++i){CONTEXT c={0};c.ContextFlags=CONTEXT_CONTROL;
        if(!GetThreadContext(g->threads[i],&c)){e=GetLastError();goto done;}
#ifdef _WIN64
        uintptr_t ip=(uintptr_t)c.Rip;
#else
        uintptr_t ip=(uintptr_t)c.Eip;
#endif
        for(DWORD j=0;j<count;++j){uintptr_t start=(uintptr_t)regions[j].address;if(ip>=start && ip-start<regions[j].bytes){e=ERROR_BUSY;goto done;}}
    }
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
    if(after_suspend)after_suspend();
#endif
    e=verify(g,pid,self);
done:
    if(e){DWORD d=process_hook_late_guard_end(g);if(d)return d;}
    return e;
}
