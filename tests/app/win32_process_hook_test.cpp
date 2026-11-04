#include <windows.h>
#include <stdio.h>
#include "lib-win32/process-hook/detours/detours.h"
static unsigned copies,directs,helpers;
static BOOL copied=TRUE,installed=TRUE;
static BOOL WINAPI fake_copy(HANDLE,REFGUID,PVOID,DWORD) { ++copies; return copied; }
static BOOL WINAPI fake_direct(HANDLE,LPCSTR *,DWORD) { ++directs; return installed; }
static BOOL WINAPI fake_helper(DWORD,DWORD,LPCSTR *,PDETOUR_CREATE_PROCESS_ROUTINEW) { ++helpers; return installed; }
static DWORD WINAPI fake_pid(HANDLE) { return 123; }
#define DetourCopyPayloadToProcess fake_copy
#define DetourUpdateProcessWithDll fake_direct
#define DetourProcessViaHelperDllsW fake_helper
#define GetProcessId fake_pid
#include "lib-win32/process-hook/install.cpp"
#define CHECK(x) do { if(!(x)){fprintf(stderr,"process-hook check failed line %d\n",__LINE__);return 1;}++checks; } while(0)
int main(void)
{
    unsigned checks=0; GUID id={0}; char payload=0;
    DWORD native=sizeof(void *)==8 ? IMAGE_FILE_MACHINE_AMD64 : IMAGE_FILE_MACHINE_I386;
    DWORD other=sizeof(void *)==8 ? IMAGE_FILE_MACHINE_I386 : IMAGE_FILE_MACHINE_AMD64;
    CHECK(win32_process_hook_apply((HANDLE)1,native,&id,&payload,1,NULL)==0 && copies==1 && !directs && !helpers);
    CHECK(win32_process_hook_apply((HANDLE)1,native,&id,&payload,1,"hook.dll")==0 && directs==1 && !helpers);
    CHECK(win32_process_hook_apply((HANDLE)1,other,&id,&payload,1,"hook.dll")==0 && directs==1 && helpers==1);
    copied=FALSE;SetLastError(ERROR_ACCESS_DENIED);
    CHECK(win32_process_hook_apply((HANDLE)1,native,&id,&payload,1,"hook.dll")==ERROR_ACCESS_DENIED && directs==1);
    SetLastError(0);CHECK(win32_process_hook_apply((HANDLE)1,native,&id,&payload,1,"hook.dll")==ERROR_DLL_INIT_FAILED);
    copied=TRUE;installed=FALSE;SetLastError(ERROR_TIMEOUT);
    CHECK(win32_process_hook_apply((HANDLE)1,other,&id,&payload,1,"hook.dll")==ERROR_TIMEOUT && helpers==2);
    SetLastError(0);CHECK(win32_process_hook_apply((HANDLE)1,native,&id,&payload,1,"hook.dll")==ERROR_DLL_INIT_FAILED);
    printf("PASS process-hook mechanism checks=%u\n",checks);return 0;
}
