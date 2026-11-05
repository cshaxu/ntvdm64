#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include "lib-win32/handle/duplicate.h"
#define CHECK(x) do {if(!(x)){fprintf(stderr,"handle check failed line %d error=%lu\n",__LINE__,GetLastError());result=1;goto cleanup;}++checks;}while(0)
int main(int argc,char **argv)
{
    HANDLE stop=NULL,source=NULL,local=NULL,remote=NULL,readback=NULL;
    PROCESS_INFORMATION child={0};STARTUPINFOW startup={0};SECURITY_ATTRIBUTES sa={sizeof(sa),NULL,TRUE};
    WCHAR image[MAX_PATH],command[MAX_PATH+80];int checks=0,result=0;
    win32_handle_compare_fn compare=win32_handle_compare_provider();
    if(argc==3){HANDLE gate=(HANDLE)(UINT_PTR)_strtoui64(argv[2],NULL,10);return WaitForSingleObject(gate,20000)==WAIT_OBJECT_0?0:2;}
    CHECK(win32_handle_duplicate(GetCurrentProcess(),NULL,GetCurrentProcess(),NULL)==ERROR_INVALID_PARAMETER);
    source=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(source!=NULL);
    CHECK(win32_handle_duplicate(GetCurrentProcess(),source,GetCurrentProcess(),&local)==ERROR_SUCCESS && local!=NULL);
    CHECK(compare!=NULL);
    CHECK(compare(source,local));
    stop=CreateEventW(&sa,TRUE,FALSE,NULL);CHECK(stop!=NULL);
    CHECK(!compare(source,stop));
    CHECK(SetEvent(local) && WaitForSingleObject(source,0)==WAIT_OBJECT_0);
    CloseHandle(local);local=NULL;
    CHECK(win32_handle_duplicate(GetCurrentProcess(),NULL,GetCurrentProcess(),&local)!=ERROR_SUCCESS && local==NULL);
    CHECK(GetModuleFileNameW(NULL,image,MAX_PATH)>0);
    CHECK(swprintf_s(command,_countof(command),L"\"%ls\" child %llu",image,(unsigned long long)(UINT_PTR)stop)>0);
    startup.cb=sizeof(startup);
    CHECK(CreateProcessW(image,command,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&startup,&child));
    CHECK(win32_handle_duplicate(GetCurrentProcess(),source,child.hProcess,&remote)==ERROR_SUCCESS && remote!=NULL);
    CHECK(DuplicateHandle(child.hProcess,remote,GetCurrentProcess(),&readback,0,FALSE,DUPLICATE_SAME_ACCESS));
    CHECK(WaitForSingleObject(readback,0)==WAIT_OBJECT_0);CloseHandle(readback);readback=NULL;
    win32_handle_close_remote(child.hProcess,remote);
    CHECK(!DuplicateHandle(child.hProcess,remote,GetCurrentProcess(),&readback,0,FALSE,DUPLICATE_SAME_ACCESS));
    remote=NULL;
    win32_handle_close_remote(child.hProcess,NULL);
cleanup:
    if(remote && child.hProcess)win32_handle_close_remote(child.hProcess,remote);
    if(readback)CloseHandle(readback);if(local)CloseHandle(local);if(source)CloseHandle(source);
    if(stop)SetEvent(stop);
    if(child.hProcess){if(WaitForSingleObject(child.hProcess,5000)!=WAIT_OBJECT_0){TerminateProcess(child.hProcess,1);WaitForSingleObject(child.hProcess,5000);result=1;}CloseHandle(child.hProcess);}
    if(child.hThread)CloseHandle(child.hThread);if(stop)CloseHandle(stop);
    if(!result)printf("PASS Win32 handle local/remote lifecycle checks=%d\n",checks);
    return result;
}
