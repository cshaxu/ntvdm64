#include <windows.h>
#include <stdio.h>
#include "lib-win32/thread/thread.h"
#define CHECK(x) do { if (!(x)) { fputs("thread test failed\n",stderr); return 1; } ++checks; } while (0)
static LONG entered,finished,ran;
static int enter(void *owner,const char *name) { (void)name; InterlockedIncrement(&entered); return owner==(void *)1; }
static void finish(void) { InterlockedIncrement(&finished); }
static DWORD __cdecl entry(void *value) { InterlockedIncrement(&ran); return (DWORD)(ULONG_PTR)value; }
int main(void)
{
    int checks=0; DWORD code=0; HANDLE thread; win32_thread_entry fn={0}; win32_thread_hooks hooks={0};
    CHECK(!win32_thread_create(NULL,0,fn,NULL,0,NULL,&hooks) && GetLastError()==ERROR_INVALID_PARAMETER);
    fn.cdecl_entry=entry;
    CHECK(!win32_thread_create(NULL,0,fn,NULL,0,NULL,NULL) && GetLastError()==ERROR_INVALID_PARAMETER);
    hooks.owner=(void *)1;hooks.enter=enter;hooks.finish=finish;
    thread=win32_thread_create(NULL,0,fn,(void *)37,0,NULL,&hooks);CHECK(thread!=NULL);
    CHECK(WaitForSingleObject(thread,INFINITE)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&code) && code==37 && entered==1 && finished==1 && ran==1);
    CloseHandle(thread);
    hooks.owner=(void *)2;entered=finished=ran=0;
    thread=win32_thread_create(NULL,0,fn,NULL,0,NULL,&hooks);CHECK(thread!=NULL);
    CHECK(WaitForSingleObject(thread,INFINITE)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&code) && code==ERROR_INVALID_STATE && entered==1 && !finished && !ran);
    CloseHandle(thread);puts("PASS thread unit");return 0;
}
