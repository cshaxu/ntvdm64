#include <windows.h>
#include <stdio.h>
#include "lib-win32/process-image/image.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL line=%d error=%lu\n",__LINE__,GetLastError()); return 1; } } while (0)
static int child_machine(const WCHAR *path, WORD expected)
{
    STARTUPINFOW startup={0}; PROCESS_INFORMATION child={0}; WORD machine=0;
    DWORD error; BOOL ok; HANDLE limited=NULL, denied=NULL;
    startup.cb=sizeof(startup);
    if(!CreateProcessW(path,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&startup,&child))return 0;
    error=win32_process_image_machine(child.hProcess,&machine);
    ok=error==ERROR_SUCCESS && machine==expected;
    if(ok && DuplicateHandle(GetCurrentProcess(),child.hProcess,GetCurrentProcess(),&limited,
        PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0)) {
        machine=0;ok=win32_process_image_machine(limited,&machine)==ERROR_SUCCESS && machine==expected;
    } else ok=FALSE;
    if(ok && DuplicateHandle(GetCurrentProcess(),child.hProcess,GetCurrentProcess(),&denied,0,FALSE,0)) {
        machine=0;ok=win32_process_image_machine(denied,&machine)==ERROR_ACCESS_DENIED;
    } else ok=FALSE;
    if(limited)CloseHandle(limited);if(denied)CloseHandle(denied);
    TerminateProcess(child.hProcess,0); WaitForSingleObject(child.hProcess,INFINITE);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
    return ok;
}
int wmain(int argc,WCHAR **argv)
{
    WORD machine=0; DWORD error;
    CHECK(argc==3);
    CHECK(win32_process_image_machine(NULL,&machine)==ERROR_INVALID_PARAMETER);
    CHECK(win32_process_image_machine(GetCurrentProcess(),NULL)==ERROR_INVALID_PARAMETER);
    CHECK(win32_process_image_machine(GetCurrentProcess(),&machine)==ERROR_SUCCESS);
    CHECK(machine==(sizeof(void*)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386));
    error=win32_process_image_machine((HANDLE)(ULONG_PTR)0x7fffffff,&machine);
    CHECK(error!=ERROR_SUCCESS);
    CHECK(child_machine(argv[1],IMAGE_FILE_MACHINE_I386));
    CHECK(child_machine(argv[2],IMAGE_FILE_MACHINE_AMD64));
    printf("PASS process image machine: self, invalid handle, query access, denied access, x86 and x64 suspended targets\n");
    return 0;
}
