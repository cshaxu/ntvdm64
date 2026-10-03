/* Deterministic fault-injection unit: exercise the production transfer body,
 * including simultaneous completion/death and pending-I/O drain ordering.
 * Actual pipe/kernel lifetime remains covered by the request/execution tests. */
#include <windows.h>
#include <stdio.h>
#include "common/transport/pipe_transfer.h"
static DWORD last_error,remaining,step,wait_result,stop_result,peer_result;
static BOOL pending,eof,fail_result,peer_first;
static unsigned checks,failures,cancels,drains,calls;
#define CHECK(x) do { ++checks; if(!(x)) { ++failures; printf("FAIL line %d: %s\n",__LINE__,#x); } } while(0)
static DWORD WINAPI fake_error(void) { return last_error; }
static DWORD WINAPI fake_single(HANDLE handle,DWORD timeout)
{ (void)timeout;return handle==(HANDLE)2 ? peer_result : stop_result; }
static BOOL WINAPI fake_reset(HANDLE event) { (void)event;return TRUE; }
static BOOL WINAPI fake_read(HANDLE pipe,LPVOID buffer,DWORD bytes,LPDWORD count,LPOVERLAPPED io)
{
    (void)pipe;(void)buffer;++calls;
    CHECK(io->hEvent==(HANDLE)1);
    CHECK(bytes==remaining);
    *count=eof ? 0 : (bytes<step ? bytes : step);
    if(pending) { last_error=ERROR_IO_PENDING;return FALSE; }
    remaining-=*count;return TRUE;
}
static BOOL WINAPI fake_write(HANDLE pipe,LPCVOID buffer,DWORD bytes,LPDWORD count,LPOVERLAPPED io)
{ return fake_read(pipe,(LPVOID)buffer,bytes,count,io); }
static DWORD WINAPI fake_wait(DWORD count,const HANDLE *handles,BOOL all,DWORD timeout)
{
    CHECK(count==2 || count==3);CHECK(!all && timeout==INFINITE);
    CHECK(handles[0]==(HANDLE)(peer_first ? 2 : 1) && handles[1]==(HANDLE)(peer_first ? 1 : 2));
    return wait_result;
}
static BOOL WINAPI fake_cancel(HANDLE pipe,LPOVERLAPPED io)
{ (void)pipe;(void)io;++cancels;return TRUE; }
static BOOL WINAPI fake_result(HANDLE pipe,LPOVERLAPPED io,LPDWORD count,BOOL wait)
{
    (void)pipe;(void)io;
    if(wait) { CHECK(cancels==1);++drains;last_error=ERROR_OPERATION_ABORTED;return FALSE; }
    if(fail_result) { last_error=ERROR_BROKEN_PIPE;return FALSE; }
    *count=eof ? 0 : (remaining<step ? remaining : step);
    remaining-=*count;return TRUE;
}
#define GetLastError fake_error
#define WaitForSingleObject fake_single
#define ResetEvent fake_reset
#define ReadFile fake_read
#define WriteFile fake_write
#define WaitForMultipleObjects fake_wait
#define CancelIoEx fake_cancel
#define GetOverlappedResult fake_result
#include "../../src/common/transport/pipe_transfer.c"
/* This fixture explicitly selects completion-first; endpoint priority is not
 * a launcher-owned wrapper around the common transport. */
static DWORD frontend_request_transfer(HANDLE pipe,HANDLE peer,HANDLE stop,HANDLE event,
    BOOL write,void *buffer,DWORD bytes)
{ return common_pipe_transfer(pipe,peer,stop,event,COMMON_PIPE_COMPLETION_FIRST,
    ERROR_PROCESS_ABORTED,write,buffer,bytes,bytes); }
static void reset_case(void)
{
    last_error=0;remaining=17;step=3;wait_result=WAIT_OBJECT_0;
    stop_result=peer_result=WAIT_TIMEOUT;pending=FALSE;eof=FALSE;fail_result=FALSE;peer_first=FALSE;
    cancels=drains=calls=0;
}
int main(void)
{
    BYTE bytes[17];DWORD error;
    reset_case();
    CHECK(!frontend_request_transfer((HANDLE)4,(HANDLE)2,NULL,(HANDLE)1,FALSE,bytes,17));
    CHECK(calls==6 && remaining==0);
    reset_case();pending=TRUE;
    /* Both I/O and peer death are signaled: Windows wait returns index zero.
     * Every partial completed chunk must win, not be discarded as peer loss. */
    CHECK(!frontend_request_transfer((HANDLE)4,(HANDLE)2,NULL,(HANDLE)1,TRUE,bytes,17));
    CHECK(calls==6 && remaining==0 && !cancels && !drains);
    reset_case();pending=TRUE;wait_result=WAIT_OBJECT_0+1;
    error=frontend_request_transfer((HANDLE)4,(HANDLE)2,NULL,(HANDLE)1,FALSE,bytes,17);
    CHECK(error==ERROR_PROCESS_ABORTED && cancels==1 && drains==1);
    reset_case();pending=TRUE;wait_result=WAIT_OBJECT_0+2;
    error=frontend_request_transfer((HANDLE)4,(HANDLE)2,(HANDLE)3,(HANDLE)1,TRUE,bytes,17);
    CHECK(error==ERROR_OPERATION_ABORTED && cancels==1 && drains==1);
    reset_case();stop_result=WAIT_OBJECT_0;
    CHECK(frontend_request_transfer((HANDLE)4,(HANDLE)2,(HANDLE)3,(HANDLE)1,FALSE,bytes,17)==ERROR_OPERATION_ABORTED);
    CHECK(!calls && !cancels && !drains);
    reset_case();eof=TRUE;
    CHECK(frontend_request_transfer((HANDLE)4,(HANDLE)2,NULL,(HANDLE)1,FALSE,bytes,17)==ERROR_BROKEN_PIPE);
    reset_case();pending=TRUE;fail_result=TRUE;
    CHECK(frontend_request_transfer((HANDLE)4,(HANDLE)2,NULL,(HANDLE)1,FALSE,bytes,17)==ERROR_BROKEN_PIPE);
    CHECK(!cancels && !drains);
    reset_case();peer_first=TRUE;pending=TRUE;wait_result=WAIT_OBJECT_0+1;
    CHECK(!common_pipe_transfer((HANDLE)4,(HANDLE)2,NULL,(HANDLE)1,
        COMMON_PIPE_PEER_DEATH_FIRST,ERROR_PIPE_NOT_CONNECTED,FALSE,bytes,17,sizeof(bytes)));
    CHECK(calls==6 && !cancels && !drains);
    reset_case();peer_first=TRUE;pending=TRUE;wait_result=WAIT_OBJECT_0;
    CHECK(common_pipe_transfer((HANDLE)4,(HANDLE)2,NULL,(HANDLE)1,
        COMMON_PIPE_PEER_DEATH_FIRST,ERROR_PIPE_NOT_CONNECTED,FALSE,bytes,17,sizeof(bytes))==ERROR_PIPE_NOT_CONNECTED);
    CHECK(cancels==1 && drains==1);
    reset_case();peer_first=TRUE;peer_result=WAIT_OBJECT_0;
    CHECK(common_pipe_transfer((HANDLE)4,(HANDLE)2,NULL,(HANDLE)1,
        COMMON_PIPE_PEER_DEATH_FIRST,ERROR_PIPE_NOT_CONNECTED,FALSE,bytes,17,sizeof(bytes))==ERROR_PIPE_NOT_CONNECTED);
    CHECK(!calls && !cancels && !drains);
    reset_case();
    CHECK(common_pipe_transfer((HANDLE)4,(HANDLE)2,NULL,(HANDLE)1,
        COMMON_PIPE_COMPLETION_FIRST,ERROR_PROCESS_ABORTED,FALSE,bytes,18,sizeof(bytes))==ERROR_INVALID_PARAMETER);
    CHECK(common_pipe_transfer((HANDLE)4,(HANDLE)2,NULL,(HANDLE)1,
        (common_pipe_priority)99,ERROR_PROCESS_ABORTED,FALSE,bytes,17,sizeof(bytes))==ERROR_INVALID_PARAMETER);
    CHECK(!calls);
    printf("common/control transfer checks=%u failures=%u\n",checks,failures);
    return failures ? 1 : 0;
}
