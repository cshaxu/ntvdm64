#include <windows.h>
#include <stdio.h>
static BOOL fake_times(HANDLE p,LPFILETIME created,LPFILETIME e,LPFILETIME k,LPFILETIME u)
{(void)p;(void)e;(void)k;(void)u;created->dwLowDateTime=77;created->dwHighDateTime=88;return FALSE;}
static DWORD fake_pid(HANDLE p){(void)p;return 0;}
static BOOL fake_watch(PHANDLE wait,HANDLE process,WAITORTIMERCALLBACK callback,PVOID context,ULONG timeout,ULONG flags)
{(void)wait;(void)process;(void)callback;(void)context;if(timeout!=INFINITE || flags!=WT_EXECUTEONLYONCE)return TRUE;SetLastError(ERROR_ACCESS_DENIED);return FALSE;}
static BOOL fake_drain(HANDLE wait,HANDLE done)
{(void)wait;if(done!=INVALID_HANDLE_VALUE)return TRUE;SetLastError(ERROR_INVALID_HANDLE);return FALSE;}
#define GetProcessTimes fake_times
#define GetProcessId fake_pid
#define RegisterWaitForSingleObject fake_watch
#define UnregisterWaitEx fake_drain
#include "lib-win32/process-observation/observation.c"
#define CHECK(x) do{if(!(x)){fprintf(stderr,"observation failure check line %d\n",__LINE__);return 1;}++checks;}while(0)
int main(void)
{
    int checks=0;FILETIME created={99,99};DWORD pid=99;HANDLE wait=NULL;
    win32_process_observation_identity((HANDLE)1,&pid,&created);
    CHECK(pid==0 && created.dwLowDateTime==0 && created.dwHighDateTime==0);
    CHECK(!win32_process_observation_watch(&wait,(HANDLE)1,NULL,NULL) && GetLastError()==ERROR_ACCESS_DENIED && !wait);
    CHECK(!win32_process_observation_drain((HANDLE)1) && GetLastError()==ERROR_INVALID_HANDLE);
    printf("PASS process observation failure checks=%d\n",checks);return 0;
}
