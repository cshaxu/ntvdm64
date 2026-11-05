/* Production archive, real pipes/events/threads; no Console or service.
 * The attachment callback is local test composition, not RPC acceptance. */
#include "common/frontend-base/channel.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL %u: %s error=%lu\n",__LINE__,#x,GetLastError());ExitProcess(1);} } while(0)
typedef struct fixture {
    frontend_base_channel *channel;
    HANDLE peer,input,write_entered;
    DWORD generation,bind_error,attach_error;
    volatile LONG finished,calls;
} fixture;
static DWORD bind_host(void *context){return ((fixture *)context)->bind_error;}
static DWORD attach(void *context,DWORD request,HANDLE pipe,HANDLE ready,DWORD *generation)
{
    fixture *f=context;
    CHECK(request==17 && ready);
    if(f->attach_error)return f->attach_error;
    CHECK(DuplicateHandle(GetCurrentProcess(),pipe,GetCurrentProcess(),&f->peer,0,FALSE,DUPLICATE_SAME_ACCESS));
    *generation=71;return 0;
}
static void generation(void *context,DWORD value){((fixture *)context)->generation=value;}
static BOOL active_host(void *context){(void)context;return TRUE;}
static DWORD dispatch_host(void *context,const console_io_request *request,console_io_reply *reply,frontend_base_result *result)
{
    fixture *f=context;
    CHECK(f->generation==71);
    InterlockedIncrement(&f->calls);
    reply->result=TRUE;reply->sequence=request->sequence;
    if(request->bytes){CHECK(request->bytes==4);CHECK(!memcmp(request->data,"test",4));}
    if(request->state.mode==1)result->reply=FALSE;
    if(request->state.mode==2)result->terminal_error=ERROR_WRITE_FAULT;
    if(request->state.mode==3)return ERROR_INVALID_DATA;
    if(request->state.mode==4) {
        reply->bytes=CONSOLE_IO_TILE_BYTES;
        memset(reply->data,0x5a,reply->bytes);
        if(f->calls==2)CHECK(SetEvent(f->write_entered));
    }
    return 0;
}
static DWORD pending(void *context,BOOL *value){(void)context;*value=FALSE;return 0;}
static void finish_host(void *context){InterlockedIncrement(&((fixture *)context)->finished);}
static const frontend_base_channel_ops ops={bind_host,attach,generation,active_host,dispatch_host,pending,finish_host};
static DWORD start(fixture *f,HANDLE *consumed)
{
    f->input=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(f->input);
    CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),consumed,SYNCHRONIZE,FALSE,0));
    return frontend_base_channel_start(17,*consumed,f->input,&ops,f,&f->channel);
}
static void stop(fixture *f)
{
    CHECK(!frontend_base_channel_stop(f->channel));f->channel=NULL;
    if(f->peer)CloseHandle(f->peer);
    if(f->write_entered)CloseHandle(f->write_entered);
    CloseHandle(f->input);
}
static void transfer(HANDLE pipe,BOOL write,void *data,DWORD size)
{
    OVERLAPPED ov={0};DWORD done=0,error;
    ov.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(ov.hEvent);
    if(!(write ? WriteFile(pipe,data,size,&done,&ov) : ReadFile(pipe,data,size,&done,&ov))){
        error=GetLastError();CHECK(error==ERROR_IO_PENDING);
        CHECK(WaitForSingleObject(ov.hEvent,5000)==WAIT_OBJECT_0);
        CHECK(GetOverlappedResult(pipe,&ov,&done,FALSE));
    }
    CHECK(done==size);CloseHandle(ov.hEvent);
}
static void request(fixture *f,DWORD mode,DWORD sequence,BOOL fragmented)
{
    console_io_request q={0};console_io_reply r={0};
    q.operation=CONSOLE_IO_BARRIER;q.state.mode=mode;q.sequence=sequence;
    q.bytes=fragmented ? 4 : 0;
    if(fragmented){
        transfer(f->peer,TRUE,&q,3);
        transfer(f->peer,TRUE,(BYTE *)&q+3,CONSOLE_IO_REQUEST_HEADER_BYTES-3);
        transfer(f->peer,TRUE,"te",2);transfer(f->peer,TRUE,"st",2);
    }else transfer(f->peer,TRUE,&q,CONSOLE_IO_REQUEST_HEADER_BYTES);
    if(mode!=1 && mode!=3 && mode!=4){
        transfer(f->peer,FALSE,&r,CONSOLE_IO_REPLY_HEADER_BYTES);
        CHECK(r.result && r.sequence==sequence);
    }
}
int main(int argc,char **argv)
{
    fixture a={0},b={0},failed={0};HANDLE wa,wb,wf;DWORD exit_code,flags;
    if(argc==3 && !strcmp(argv[1],"--peer")) {
        HANDLE parent=OpenProcess(SYNCHRONIZE,FALSE,(DWORD)strtoul(argv[2],NULL,10));
        if(parent){WaitForSingleObject(parent,15000);CloseHandle(parent);}
        return 1;
    }
    CHECK(!start(&a,&wa));CHECK(!start(&b,&wb));
    request(&a,0,1,TRUE);request(&b,0,2,FALSE);
    request(&a,1,3,FALSE);request(&a,0,4,FALSE); /* No stray intermediate reply. */
    stop(&a);CHECK(a.finished==1);
    ZeroMemory(&a,sizeof(a));CHECK(!start(&a,&wa));
    {
        console_io_request q={0};q.reserved=1;
        transfer(a.peer,TRUE,&q,CONSOLE_IO_REQUEST_HEADER_BYTES);
        CHECK(WaitForSingleObject(frontend_base_channel_thread(a.channel),5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(frontend_base_channel_thread(a.channel),&exit_code) && exit_code==ERROR_INVALID_DATA);
        CHECK(!a.calls);stop(&a);CHECK(a.finished==1);
    }
    {
        WCHAR image[MAX_PATH],command[MAX_PATH+32];
        STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION process={0};
        ZeroMemory(&a,sizeof(a));
        CHECK(GetModuleFileNameW(NULL,image,MAX_PATH));
        swprintf_s(command,MAX_PATH+32,L"\"%s\" --peer %lu",image,GetCurrentProcessId());
        CHECK(CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&startup,&process));
        CloseHandle(process.hThread);
        a.input=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(a.input);
        CHECK(DuplicateHandle(GetCurrentProcess(),process.hProcess,GetCurrentProcess(),&wa,SYNCHRONIZE,FALSE,0));
        CHECK(!frontend_base_channel_start(17,wa,a.input,&ops,&a,&a.channel));
        request(&a,0,1,FALSE);
        CHECK(TerminateProcess(process.hProcess,19));
        CHECK(WaitForSingleObject(process.hProcess,5000)==WAIT_OBJECT_0);
        CloseHandle(process.hProcess);
        CHECK(WaitForSingleObject(frontend_base_channel_thread(a.channel),5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(frontend_base_channel_thread(a.channel),&exit_code) && exit_code==ERROR_PROCESS_ABORTED);
        stop(&a);CHECK(a.finished==1);
    }
    request(&b,0,5,FALSE); /* A's stop cannot cancel B. */
    request(&b,2,6,FALSE);
    CHECK(WaitForSingleObject(frontend_base_channel_thread(b.channel),5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(frontend_base_channel_thread(b.channel),&exit_code) && exit_code==ERROR_WRITE_FAULT);
    stop(&b);CHECK(b.finished==1);
    failed.bind_error=ERROR_ACCESS_DENIED;CHECK(start(&failed,&wf)==ERROR_ACCESS_DENIED);
    CHECK(!failed.channel && !GetHandleInformation(wf,&flags));stop(&failed);
    ZeroMemory(&failed,sizeof(failed));failed.attach_error=ERROR_INVALID_DATA;
    CHECK(start(&failed,&wf)==ERROR_INVALID_DATA);
    CHECK(!failed.channel && !GetHandleInformation(wf,&flags));stop(&failed);
    ZeroMemory(&a,sizeof(a));CHECK(!start(&a,&wa));
    /* Stop while a partial header leaves the production read pending. */
    transfer(a.peer,TRUE,"x",1);stop(&a);CHECK(a.finished==1);
    ZeroMemory(&a,sizeof(a));CHECK(!start(&a,&wa));
    a.write_entered=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(a.write_entered);
    request(&a,4,1,FALSE);request(&a,4,2,FALSE);request(&a,4,3,FALSE);
    CHECK(WaitForSingleObject(a.write_entered,5000)==WAIT_OBJECT_0);
    /* The peer never consumes the replies: cancellation must also drain writes. */
    stop(&a);CHECK(a.finished==1);
    ZeroMemory(&a,sizeof(a));CHECK(!start(&a,&wa));
    request(&a,3,1,FALSE);
    CHECK(WaitForSingleObject(frontend_base_channel_thread(a.channel),5000)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(frontend_base_channel_thread(a.channel),&exit_code) && exit_code==ERROR_INVALID_DATA);
    stop(&a);CHECK(a.finished==1);
    puts("PASS frontend-base channel isolation framing cancellation rollback and reply ordering");
    return 0;
}
