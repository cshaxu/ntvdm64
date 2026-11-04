#include <windows.h>
#include <stdio.h>
#include "lib-win32/process-observation/observation.h"
static HANDLE entered,release_callback;
static volatile LONG calls;
static VOID CALLBACK on_exit(void *context,BOOLEAN timed_out)
{
    (void)context;
    if(!timed_out)InterlockedIncrement(&calls);
    SetEvent(entered);
    WaitForSingleObject(release_callback,5000);
}
static DWORD WINAPI drain_thread(void *value)
{return win32_process_observation_drain((HANDLE)value) ? 0 : GetLastError();}
#define CHECK(x) do{if(!(x)){fprintf(stderr,"process observation failed line %d error=%lu\n",__LINE__,GetLastError());return 1;}++checks;}while(0)
int main(void)
{
    int checks=0;HANDLE retained=NULL,signal=NULL,wait=NULL,drainer=NULL;DWORD pid=0;FILETIME created={0};WCHAR image[32768];
    CHECK(win32_process_observation_retain(GetCurrentProcess(),&retained));
    CHECK(retained!=NULL && GetProcessId(retained)==GetCurrentProcessId());
    win32_process_observation_identity(retained,&pid,&created);
    CHECK(pid==GetCurrentProcessId() && (created.dwLowDateTime || created.dwHighDateTime));
    CHECK(win32_process_observation_image(retained,image,32768) && image[0]);
    CloseHandle(retained);retained=NULL;
    CHECK(!win32_process_observation_retain(INVALID_HANDLE_VALUE,&retained) || (retained && CloseHandle(retained)));
    /* Explicitly invalid non-pseudohandle must not be retained. */
    retained=NULL;CHECK(!win32_process_observation_retain((HANDLE)(ULONG_PTR)0x7fffffff,&retained) && !retained);
    created.dwLowDateTime=created.dwHighDateTime=99;pid=99;
    win32_process_observation_identity((HANDLE)(ULONG_PTR)0x7fffffff,&pid,&created);
    CHECK(pid==0 && created.dwLowDateTime==0 && created.dwHighDateTime==0);
    CHECK(!win32_process_observation_image((HANDLE)(ULONG_PTR)0x7fffffff,image,32768));
    entered=CreateEventW(NULL,TRUE,FALSE,NULL);release_callback=CreateEventW(NULL,TRUE,FALSE,NULL);signal=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(entered && release_callback && signal);
    CHECK(win32_process_observation_watch(&wait,signal,on_exit,NULL));
    CHECK(SetEvent(signal) && WaitForSingleObject(entered,5000)==WAIT_OBJECT_0 && calls==1);
    drainer=CreateThread(NULL,0,drain_thread,wait,0,NULL);CHECK(drainer!=NULL);
    CHECK(WaitForSingleObject(drainer,100)==WAIT_TIMEOUT);
    CHECK(SetEvent(release_callback) && WaitForSingleObject(drainer,5000)==WAIT_OBJECT_0);
    {DWORD exit_code=1;CHECK(GetExitCodeThread(drainer,&exit_code) && exit_code==0);}
    CHECK(calls==1);
    CloseHandle(drainer);CloseHandle(signal);CloseHandle(release_callback);CloseHandle(entered);
    printf("PASS process observation lifecycle checks=%d\n",checks);return 0;
}
