/* Production channel/dispatcher, real pipes, threads and Console APIs.
 * Only broker attachment is substituted; this is not an RPC/authentication
 * or guest test. Run on an unswitched private-desktop Console. */
#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#define run16_console_dispatch actual_dispatch
#include "../../src/run16-exe/console_frontend.c"
#undef run16_console_dispatch
#include "../../src/run16-exe/console_video.c"
static HANDLE read_entered,peer;
static DWORD expected_generation;
DWORD run16_console_dispatch(run16_console_frontend *owner,
    const console_io_request *request,console_io_reply *reply)
{
    if(request->operation==CONSOLE_IO_READ_INPUT) SetEvent(read_entered);
    return actual_dispatch(owner,request,reply);
}
#include "../../src/run16-exe/console_channel.c"
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"FAIL line=%u error=%lu\n", \
    (unsigned)__LINE__,GetLastError());ExitProcess(1); } } while(0)
DWORD OpenNtBaseClientAttachFrontendRequest(DWORD request,HANDLE pipe,
    HANDLE ready,DWORD *generation)
{
    (void)ready;
    if(request!=expected_generation) return ERROR_INVALID_PARAMETER;
    if(!DuplicateHandle(GetCurrentProcess(),pipe,GetCurrentProcess(),&peer,
        0,FALSE,DUPLICATE_SAME_ACCESS)) return GetLastError();
    *generation=request;return 0;
}
static void peer_io(BOOL write,void *buffer,DWORD bytes)
{
    BYTE *cursor=buffer;
    while(bytes) {
        OVERLAPPED io={0};DWORD done=0;BOOL ok;
        io.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(io.hEvent);
        ok=write ? WriteFile(peer,cursor,bytes,&done,&io) : ReadFile(peer,cursor,bytes,&done,&io);
        if(!ok) {
            CHECK(GetLastError()==ERROR_IO_PENDING);
            CHECK(WaitForSingleObject(io.hEvent,5000)==WAIT_OBJECT_0);
            CHECK(GetOverlappedResult(peer,&io,&done,FALSE));
        }
        CHECK(done && done<=bytes);CloseHandle(io.hEvent);cursor+=done;bytes-=done;
    }
}
static void run_case(unsigned mode,unsigned round)
{
    run16_console_channel *channel=NULL;
    HANDLE worker,thread,ready;
    console_io_request request={0};console_io_reply reply={0};
    DWORD exit_code;ULONGLONG started;
    expected_generation=1+round*3+mode;
    CHECK(ResetEvent(read_entered));
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
        &worker,SYNCHRONIZE,FALSE,0));
    CHECK(!run16_console_channel_start_request(expected_generation,worker,&channel));
    CHECK(DuplicateHandle(GetCurrentProcess(),channel->thread,GetCurrentProcess(),
        &thread,0,FALSE,DUPLICATE_SAME_ACCESS));
    CHECK(DuplicateHandle(GetCurrentProcess(),channel->ready,GetCurrentProcess(),
        &ready,0,FALSE,DUPLICATE_SAME_ACCESS));
    CHECK(FlushConsoleInputBuffer(channel->console.input));
    request.version=CONSOLE_IO_VERSION;request.generation=expected_generation;
    request.sequence=1;request.operation=CONSOLE_IO_BARRIER;
    peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
    peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
    CHECK(reply.result && reply.sequence==1 && reply.generation==expected_generation && !reply.bytes);
    if(mode==1) {
        request.sequence=2;request.operation=CONSOLE_IO_READ_INPUT;
        request.state.input=1;request.state.count=1;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        CHECK(WaitForSingleObject(read_entered,5000)==WAIT_OBJECT_0);
        /* No producer exists: the empty native Console read cannot complete
         * normally. Cancellation must join it without injecting a key. */
        CHECK(WaitForSingleObject(thread,0)==WAIT_TIMEOUT);
    } else if(mode==2) {
        /* Acknowledged normal output followed by peer EOF races owner stop. */
        CHECK(CloseHandle(peer));peer=NULL;
    }
    started=GetTickCount64();
    run16_console_channel_stop(channel);
    CHECK(GetTickCount64()-started<5000);
    CHECK(WaitForSingleObject(thread,0)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&exit_code) && exit_code!=STILL_ACTIVE && exit_code!=0);
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CloseHandle(thread);CloseHandle(ready);
    if(peer) {CloseHandle(peer);peer=NULL;}
}
int main(void)
{
    unsigned round,mode;DWORD before,after;
    read_entered=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(read_entered);
    /* Warm up lazy runtime/Console resources before counting owned handles. */
    for(mode=0;mode<3;++mode) run_case(mode,0);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(round=1;round<=16;++round)
        for(mode=0;mode<3;++mode) run_case(mode,round);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && after==before);
    CloseHandle(read_entered);
    puts("PASS 51 real channel lifetimes: idle pipe cancel, empty Console read cancel, acknowledged barrier/EOF/stop race; joined threads, EOF readiness, no handle growth");
    return 0;
}
