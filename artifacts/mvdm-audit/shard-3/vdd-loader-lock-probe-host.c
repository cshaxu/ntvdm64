#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

typedef void (__cdecl *set_detach_fn)(HANDLE,HANDLE,volatile LONG *);
typedef void (__cdecl *request_fn)(void);
typedef void (__cdecl *set_self_fn)(request_fn);
typedef void (__cdecl *callback_fn)(void);
static HANDLE start_event, done_event;
static volatile LONG requested;

static DWORD WINAPI loader_worker(void *unused)
{
    HMODULE module;
    (void)unused;
    if (WaitForSingleObject(start_event, INFINITE) != WAIT_OBJECT_0) return 1;
    module = LoadLibraryA("version.dll");
    if (!module) return 2;
    FreeLibrary(module);
    SetEvent(done_event);
    return 0;
}
static void __cdecl request_unload(void) { InterlockedExchange(&requested,1); }

int main(int argc, char **argv)
{
    HMODULE dll; HANDLE worker; DWORD result; volatile LONG observed=-1;
    set_detach_fn set_detach; set_self_fn set_self; callback_fn callback;
    if (argc != 2) return 1;
    start_event=CreateEventA(NULL,TRUE,FALSE,NULL);
    done_event=CreateEventA(NULL,TRUE,FALSE,NULL);
    if (!start_event || !done_event) return 2;
    dll=LoadLibraryA(argv[1]); if(!dll) return 3;
    set_detach=(set_detach_fn)GetProcAddress(dll,"probe_set_detach_wait");
    if (!set_detach) return 4;
    worker=CreateThread(NULL,0,loader_worker,NULL,0,NULL); if(!worker) return 5;
    set_detach(start_event,done_event,&observed);
    if (!FreeLibrary(dll)) return 6;
    if (WaitForSingleObject(worker,5000)!=WAIT_OBJECT_0 ||
        !GetExitCodeThread(worker,&result) || result!=0) return 7;
    printf("DETACH_WAIT result=%ld timeout=%d worker_after_detach=1\n",
        observed, observed==WAIT_TIMEOUT);
    CloseHandle(worker);CloseHandle(start_event);CloseHandle(done_event);
    if (observed!=WAIT_TIMEOUT) return 8;

    dll=LoadLibraryA(argv[1]); if(!dll) return 9;
    set_self=(set_self_fn)GetProcAddress(dll,"probe_set_self_request");
    callback=(callback_fn)GetProcAddress(dll,"probe_callback");
    if(!set_self || !callback) return 10;
    set_self(request_unload); callback();
    if(!requested) return 11;
    /* The callback has returned before this owner releases its reference. */
    if(!FreeLibrary(dll)) return 12;
    printf("SELF_REQUEST deferred_until_callback_return=1\n");
    return 0;
}
