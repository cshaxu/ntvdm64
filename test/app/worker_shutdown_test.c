#include "worker-base/connection.h"
#include <stdio.h>
#include <stdlib.h>

PVOID CsrPortHeap;
static unsigned checks,failures;
#define CHECK(x) do {++checks;if(!(x)){++failures;printf("FAIL %u %s\n",__LINE__,#x);}} while(0)
static DWORD WINAPI close_local(void *context)
{
    unsigned mode=(unsigned)(ULONG_PTR)context;
    if(mode==2 || mode==5)Sleep(INFINITE); /* test-owned blocked original handler */
    return mode==1 ? ERROR_WRITE_FAULT : ERROR_SUCCESS;
}
static void run_case(unsigned mode,DWORD expected,BOOL acknowledge)
{
    WCHAR image[MAX_PATH],command[MAX_PATH+96];
    SECURITY_ATTRIBUTES attributes={sizeof(attributes),NULL,TRUE};
    HANDLE closed=CreateEventW(&attributes,TRUE,FALSE,NULL);
    STARTUPINFOW startup={sizeof(startup)};
    PROCESS_INFORMATION process={0};DWORD result=0;
    CHECK(closed && GetModuleFileNameW(NULL,image,MAX_PATH));
    swprintf_s(command,MAX_PATH+96,L"\"%s\" --close %u %Iu",image,mode,(ULONG_PTR)closed);
    CHECK(CreateProcessW(image,command,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
    if(process.hProcess) {
        CHECK(WaitForSingleObject(process.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeProcess(process.hProcess,&result) && result==expected);
        CHECK(WaitForSingleObject(closed,0)==(acknowledge ? WAIT_OBJECT_0 : WAIT_TIMEOUT));
        if(result==STILL_ACTIVE)TerminateProcess(process.hProcess,ERROR_TIMEOUT);
        CloseHandle(process.hThread);CloseHandle(process.hProcess);
    }
    CloseHandle(closed);
}
int wmain(int argc,WCHAR **argv)
{
    DWORD before=0,after=0;
    if(argc==4 && !wcscmp(argv[1],L"--close")) {
        unsigned mode=(unsigned)wcstoul(argv[2],NULL,10);
        HANDLE closed=(HANDLE)(ULONG_PTR)_wcstoui64(argv[3],NULL,10);
        DWORD grace=mode==2 || mode==5 ? 20 : mode==3 ? 5000 : INFINITE;
        /* The no-ack VDM owner retains its existing forced-close exit code;
         * native owners must not signal successful closure after failure. */
        worker_base_shutdown_close(close_local,(void *)(ULONG_PTR)mode,grace,
            mode==5 ? CONTROL_C_EXIT : ERROR_CANCELLED,
            mode==5 ? NULL : mode==4 ? (HANDLE)(ULONG_PTR)1 : closed);
        return ERROR_INVALID_STATE;
    }
    /* Prime Windows' first CreateProcess/Console setup before checking repeated
     * owned-event/process/thread handle disposal. This case is asserted too. */
    run_case(0,ERROR_CANCELLED,TRUE);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    run_case(0,ERROR_CANCELLED,TRUE);
    run_case(1,ERROR_WRITE_FAULT,FALSE);
    run_case(2,ERROR_TIMEOUT,FALSE);
    run_case(3,ERROR_CANCELLED,TRUE);
    run_case(4,ERROR_INVALID_HANDLE,FALSE);
    run_case(5,CONTROL_C_EXIT,FALSE);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && after==before);
    printf("handle-count before=%lu after=%lu\n",before,after);
    printf("%s worker shutdown: %u assertions, %u failures\n",failures ? "FAIL" : "PASS",checks,failures);
    return failures ? 1 : 0;
}
