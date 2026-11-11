#include <windows.h>
#include <stdio.h>

/* Exact original MAKECALL/MAKECALLCAPTURE with native synchronization at
 * their external boundary. Not a production USER lock or kernel callback.
 * Test the original drop/call/reacquire contract before binding worker entry. */
static SRWLOCK lock=SRWLOCK_INIT;
static __declspec(thread) BOOL held;
static DWORD owner;
static volatile LONG errors, callback_count, worker_count;
static HANDLE begin_worker, end_worker;
#define CHECK(x) do { if (!(x)) { InterlockedIncrement(&errors); printf("FAIL line=%d\n",__LINE__); } } while (0)
static void EnterCrit(void)
{
    if (held) { CHECK(FALSE); return; }
    AcquireSRWLockExclusive(&lock);
    held=TRUE; owner=GetCurrentThreadId();
}
static void LeaveCrit(void)
{
    if (!held) { CHECK(FALSE); return; }
    CHECK(owner==GetCurrentThreadId());
    owner=0; held=FALSE;
    ReleaseSRWLockExclusive(&lock);
}
static DWORD WINAPI worker(LPVOID parameter)
{
    (void)parameter;
    if (WaitForSingleObject(begin_worker,5000)!=WAIT_OBJECT_0) { CHECK(FALSE); return 1; }
    EnterCrit();
    CHECK(owner==GetCurrentThreadId());
    InterlockedIncrement(&worker_count);
    LeaveCrit();
    CHECK(SetEvent(end_worker));
    return 0;
}
static LONG KeUserModeCallback(ULONG api, PVOID input, ULONG length,
    PVOID *output, ULONG *output_length)
{
    static DWORD result=0x12345678;
    CHECK(!held && api==7 && input!=NULL && length!=0);
    if (held) return (LONG)0xc0000001;
    /* Reentrant USER entry on this same native thread must be possible. */
    EnterCrit(); CHECK(owner==GetCurrentThreadId()); LeaveCrit();
    CHECK(SetEvent(begin_worker));
    CHECK(WaitForSingleObject(end_worker,5000)==WAIT_OBJECT_0);
    *output=&result; *output_length=sizeof(result);
    InterlockedIncrement(&callback_count);
    return 0;
}
#define FI_TEST 7
#include "original-user-callout.inc"
static void run_case(BOOL capture)
{
    struct { struct { ULONG cbCallback; } CaptureBuf; DWORD payload; } m, *mp=&m;
    PVOID pcbs=NULL;
    ULONG cbCBStatus=0;
    LONG Status=-1;
    HANDLE thread;
    DWORD exit_code;
    m.CaptureBuf.cbCallback=sizeof(m); m.payload=42;
    CHECK(ResetEvent(begin_worker)); CHECK(ResetEvent(end_worker));
    thread=CreateThread(NULL,0,worker,NULL,0,NULL);
    CHECK(thread!=NULL); if (!thread) return;
    EnterCrit();
    if (capture) { MAKECALLCAPTURE(TEST); }
    else { MAKECALL(TEST); }
    CHECK(held && owner==GetCurrentThreadId());
    CHECK(Status==0 && cbCBStatus==sizeof(DWORD) && pcbs!=NULL);
    LeaveCrit();
    CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&exit_code) && exit_code==0);
    CHECK(CloseHandle(thread));
}
int __cdecl main(void)
{
    begin_worker=CreateEventA(NULL,TRUE,FALSE,NULL);
    end_worker=CreateEventA(NULL,TRUE,FALSE,NULL);
    if (!begin_worker || !end_worker) return 3;
    run_case(FALSE); run_case(TRUE);
    CHECK(callback_count==2 && worker_count==2 && !held);
    CHECK(CloseHandle(begin_worker)); CHECK(CloseHandle(end_worker));
    printf("WOW_ORIGINAL_USER_LOCK errors=%ld callbacks=%ld other_thread=%ld reacquire=2\n",
        errors,callback_count,worker_count);
    return errors ? 3 : 0;
}
