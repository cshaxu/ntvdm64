#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "lib-win32/pipe/pipe_pair.h"
#include "lib-win32/pipe/pipe_transfer.h"
#define CHECK(x) do { ++checks; if(!(x)){fprintf(stderr,"pipe pair failure line %d error=%lu\n",__LINE__,GetLastError());result=1;goto done;} } while(0)
int main(void)
{
    HANDLE server=INVALID_HANDLE_VALUE,client=INVALID_HANDLE_VALUE,a=NULL,b=NULL,event=NULL,peer=NULL;
    WCHAR name[96];char send[]="pipe-pair",receive[sizeof(send)]={0};DWORD pid=0;
    int checks=0,result=0;
    event=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(event!=NULL);
    peer=OpenProcess(SYNCHRONIZE,FALSE,GetCurrentProcessId());CHECK(peer!=NULL);
    CHECK(win32_pipe_pair_create(NULL,event,&a,&b)==ERROR_INVALID_PARAMETER);
    CHECK(a==INVALID_HANDLE_VALUE && b==INVALID_HANDLE_VALUE);
    CHECK(win32_pipe_pair_create(L"x",event,&a,&a)==ERROR_INVALID_PARAMETER);
    CHECK(swprintf_s(name,96,L"\\\\.\\pipe\\ntvdm-pair-test-%lu",GetCurrentProcessId())>0);
    CHECK(win32_pipe_pair_create(name,event,&server,&client)==ERROR_SUCCESS);
    CHECK(GetNamedPipeClientProcessId(server,&pid) && pid==GetCurrentProcessId());
    CHECK(win32_pipe_pair_create(name,event,&a,&b)!=ERROR_SUCCESS);
    CHECK(a==INVALID_HANDLE_VALUE && b==INVALID_HANDLE_VALUE);
    CHECK(common_pipe_transfer(client,peer,NULL,event,COMMON_PIPE_COMPLETION_FIRST,
        ERROR_PROCESS_ABORTED,TRUE,send,sizeof(send),sizeof(send))==ERROR_SUCCESS);
    CHECK(common_pipe_transfer(server,peer,NULL,event,COMMON_PIPE_COMPLETION_FIRST,
        ERROR_PROCESS_ABORTED,FALSE,receive,sizeof(receive),sizeof(receive))==ERROR_SUCCESS);
    CHECK(!memcmp(send,receive,sizeof(send)));
done:
    if(client!=INVALID_HANDLE_VALUE)CloseHandle(client);
    if(server!=INVALID_HANDLE_VALUE)CloseHandle(server);
    if(peer)CloseHandle(peer);
    if(event)CloseHandle(event);
    if(!result)printf("PASS Win32 local pipe pair checks=%d\n",checks);
    return result;
}
