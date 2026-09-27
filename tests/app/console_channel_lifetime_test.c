/* Production channel/dispatcher, real pipes, threads and Console APIs.
 * Only broker attachment is substituted; this is not an RPC/authentication
 * or guest test. Run on an unswitched private-desktop Console. */
#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include <tlhelp32.h>
#include "ntkvm-exe/native_console_frontend.h"
#define run16_console_dispatch actual_dispatch
#include "../../src/ntkvm-exe/console_frontend.c"
#undef run16_console_dispatch
#include "../../src/ntkvm-exe/console_video.c"
static HANDLE read_entered,peer;
static run16_native_frontend *test_frontend;
static DWORD expected_generation;
DWORD run16_console_dispatch(run16_console_frontend *owner,
    const console_io_request *request,console_io_reply *reply)
{
    if(request->operation==CONSOLE_IO_READ_INPUT) SetEvent(read_entered);
    return actual_dispatch(owner,request,reply);
}
#include "../../src/ntkvm-exe/console_channel.c"
#define CHECK(x) do { if(!(x)) { fprintf(stderr,"FAIL line=%u error=%lu\n", \
    (unsigned)__LINE__,GetLastError());ExitProcess(1); } } while(0)
/* Diagnostic-only self-process snapshot; never enters a product binary. */
static void audit_handles(const char *stage)
{
    typedef LONG (NTAPI *query_process)(HANDLE,ULONG,void *,ULONG,ULONG *);
    typedef LONG (NTAPI *query_object)(HANDLE,ULONG,void *,ULONG,ULONG *);
    typedef struct entry { HANDLE handle;ULONG_PTR handles,pointers;ULONG access,type,attributes,reserved; } entry;
    typedef struct snapshot { ULONG_PTR count,reserved;entry entries[1]; } snapshot;
    typedef struct text { USHORT length,capacity;WCHAR *value; } text;
    query_process qp=(query_process)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtQueryInformationProcess");
    query_object qo=(query_object)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtQueryObject");
    ULONG bytes=1024*1024,needed=0,counts[256]={0};HANDLE representatives[256]={0};
    snapshot *info=HeapAlloc(GetProcessHeap(),0,bytes);ULONG_PTR i;DWORD total;
    CHECK(info && qp && qo && qp(GetCurrentProcess(),51,info,bytes,&needed)>=0);
    CHECK(info->count<=(bytes-sizeof(ULONG_PTR)*2)/sizeof(entry));
    for(i=0;i<info->count;++i) {
        CHECK(info->entries[i].type<256);
        ++counts[info->entries[i].type];representatives[info->entries[i].type]=info->entries[i].handle;
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&total));printf("AUDIT %s total=%lu\n",stage,total);
    for(i=0;i<256;++i)if(counts[i]) {
        BYTE buffer[8192];text *name=(text *)buffer;
        if(qo(representatives[i],2,buffer,sizeof(buffer),&needed)>=0)
            printf("AUDIT type=%lu count=%lu name=%.*ls\n",(ULONG)i,counts[i],name->length/2,name->value);
    }
    HeapFree(GetProcessHeap(),0,info);
}
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
static void verify_dos_input_queue(run16_console_channel *channel)
{
    INPUT_RECORD first[130]={0},second[130]={0},received[260];
    DWORD i,count,pending;HANDLE ready=run16_native_frontend_dos_ready(test_frontend);
    for(i=0;i<130;++i) {
        first[i].EventType=second[i].EventType=KEY_EVENT;
        first[i].Event.KeyEvent.wRepeatCount=second[i].Event.KeyEvent.wRepeatCount=1;
        first[i].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(0x100+i);
        second[i].Event.KeyEvent.uChar.UnicodeChar=(WCHAR)(0x200+i);
    }
    CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
    CHECK(WaitForSingleObject(ready,0)==WAIT_TIMEOUT);
    CHECK(!run16_native_frontend_dos_prepend(test_frontend,second,130));
    CHECK(!run16_native_frontend_dos_prepend(test_frontend,first,130));
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CHECK(!run16_native_frontend_dos_read(test_frontend,TRUE,received,260,&count) && count==260);
    CHECK(!memcmp(received,first,sizeof(first)) && !memcmp(received+130,second,sizeof(second)));
    CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,received,17,&count) && count==17);
    CHECK(!memcmp(received,first,17*sizeof(*first)));
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CHECK(run16_native_frontend_dos_prepend(test_frontend,first,MAXDWORD)==ERROR_ARITHMETIC_OVERFLOW);
    CHECK(!run16_native_frontend_dos_prepend(test_frontend,first,0));
    CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,received,260,&count) && count==243);
    CHECK(!memcmp(received,first+17,113*sizeof(*first)) && !memcmp(received+113,second,sizeof(second)));
    CHECK(WaitForSingleObject(ready,0)==WAIT_TIMEOUT);
    CHECK(FlushConsoleInputBuffer(channel->console.input));
    CHECK(!run16_native_frontend_dos_prepend(test_frontend,first,3));
    run16_native_frontend_dos_leave(test_frontend);
    /* No native backend exists here: inspect the actual handoff destination,
     * not a substitute queue. Prepend must survive DOS owner relinquishment. */
    CHECK(!run16_native_frontend_dos_bind(test_frontend,channel,FALSE));
    CHECK(WaitForSingleObject(ready,0)==WAIT_TIMEOUT);
    CHECK(GetNumberOfConsoleInputEvents(channel->console.input,&pending) && pending>=3);
    CHECK(ReadConsoleInputW(channel->console.input,received,3,&count) && count==3);
    CHECK(!memcmp(received,first,3*sizeof(*first)));
    CHECK(!run16_native_frontend_dos_bind(test_frontend,channel,TRUE));
    CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
    CHECK(FlushConsoleInputBuffer(channel->console.input));
    do { CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,received,260,&count)); } while(count);
    run16_native_frontend_dos_leave(test_frontend);
    puts("PASS DOS input growth, atomic prepend/order, peek, partial drain/readiness, overflow preservation and real Console handoff");
}
static void run_case(unsigned mode,unsigned round)
{
    run16_console_channel *channel=NULL;
    HANDLE worker,thread,ready;
    PROCESS_INFORMATION child={0};
    console_io_request request={0};console_io_reply reply={0};
    DWORD exit_code;ULONGLONG started;
    expected_generation=1+round*5+mode;
    CHECK(ResetEvent(read_entered));
    if(mode==4) {
        STARTUPINFOW startup={sizeof(startup)};
        WCHAR image[MAX_PATH],command[MAX_PATH+32];
        DWORD length=GetModuleFileNameW(NULL,image,MAX_PATH);
        CHECK(length && length<MAX_PATH);
        CHECK(swprintf_s(command,MAX_PATH+32,L"\"%s\" --worker-wait",image)>0);
        CHECK(CreateProcessW(image,command,NULL,NULL,FALSE,CREATE_SUSPENDED,
            NULL,NULL,&startup,&child));
        CHECK(DuplicateHandle(GetCurrentProcess(),child.hProcess,GetCurrentProcess(),
            &worker,SYNCHRONIZE,FALSE,0));
    } else CHECK(DuplicateHandle(GetCurrentProcess(),GetCurrentProcess(),GetCurrentProcess(),
        &worker,SYNCHRONIZE,FALSE,0));
    CHECK(!run16_console_channel_start_request(expected_generation,worker,test_frontend,&channel));
    {
        WCHAR cell;DWORD count,flags;COORD origin={0,0};
        CHECK(ReadConsoleOutputCharacterW(channel->console.output,&cell,1,origin,&count) && count==1 && cell==L'K');
        CHECK(GetHandleInformation(channel->console.output,&flags) && !(flags&HANDLE_FLAG_INHERIT));
        CHECK(GetHandleInformation(channel->console.input,&flags) && !(flags&HANDLE_FLAG_INHERIT));
    }
    CHECK(!run16_native_frontend_dos_bind(test_frontend,channel,TRUE));
    CHECK(DuplicateHandle(GetCurrentProcess(),channel->thread,GetCurrentProcess(),
        &thread,0,FALSE,DUPLICATE_SAME_ACCESS));
    CHECK(DuplicateHandle(GetCurrentProcess(),channel->ready,GetCurrentProcess(),
        &ready,0,FALSE,DUPLICATE_SAME_ACCESS));
    {
        INPUT_RECORD stale[64];DWORD count;
        CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
        CHECK(FlushConsoleInputBuffer(channel->console.input));
        do { CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,stale,64,&count)); } while(count);
        run16_native_frontend_dos_leave(test_frontend);
    }
    request.version=CONSOLE_IO_VERSION;request.generation=expected_generation;
    request.sequence=1;request.operation=CONSOLE_IO_BARRIER;
    peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
    peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
    CHECK(reply.result && reply.sequence==1 && reply.generation==expected_generation && !reply.bytes);
    if(mode==0 && round<4) {
        console_video_description description={4,2,4,8,8,{0}};
        HWND window=NULL;DWORD window_pid;ULONGLONG deadline;
        description.palette[1]=0xffffff;
        request.sequence=2;request.operation=CONSOLE_IO_VIDEO_BEGIN;
        request.state.mode=1;request.bytes=sizeof(description);
        memcpy(request.data,&description,sizeof(description));
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+request.bytes);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        request.sequence=3;request.operation=CONSOLE_IO_VIDEO_DATA;request.bytes=4;
        memset(request.data,1,4);
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+4);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        Sleep(100);CHECK(!FindWindowW(NULL,L"NTVDM"));
        request.sequence=4;request.state.count=4;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+4);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        deadline=GetTickCount64()+5000;
        do {
            window=FindWindowW(NULL,L"NTVDM");
            if(window)break;
            Sleep(10);
        } while(GetTickCount64()<deadline);
        CHECK(window && GetWindowThreadProcessId(window,&window_pid) && window_pid==GetCurrentProcessId());
        if(!round) {
            INPUT_RECORD physical={0},records[64];DWORD count,written,i,keys=0;
            CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
            do { CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,records,64,&count)); } while(count);
            run16_native_frontend_dos_leave(test_frontend);
            physical.EventType=KEY_EVENT;physical.Event.KeyEvent.bKeyDown=TRUE;
            physical.Event.KeyEvent.wRepeatCount=1;physical.Event.KeyEvent.wVirtualKeyCode='Z';
            CHECK(WriteConsoleInputW(channel->console.input,&physical,1,&written) && written==1);
            CHECK(PostMessageW(window,WM_KEYDOWN,'A',0x001e0001));
            CHECK(PostMessageW(window,WM_KEYUP,'A',(LPARAM)0xc01e0001));
            deadline=GetTickCount64()+5000;
            while(keys<2 && GetTickCount64()<deadline) {
                CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
                CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,records,64,&count));
                run16_native_frontend_dos_leave(test_frontend);
                for(i=0;i<count;++i)if(records[i].EventType==KEY_EVENT) {
                    CHECK(records[i].Event.KeyEvent.wVirtualKeyCode=='A');
                    CHECK(records[i].Event.KeyEvent.bKeyDown==(keys==0));++keys;
                }
                if(keys<2)Sleep(10);
            }
            CHECK(keys==2);
            { DWORD_PTR result;
              CHECK(SendMessageTimeoutW(window,WM_KEYDOWN,'B',0x00300001,
                  SMTO_ABORTIFHUNG,3000,&result)); }
        }
        Sleep(100);CHECK(IsWindow(window)); /* No new frame is not TEXT. */
        request.sequence=5;request.operation=CONSOLE_IO_VIDEO_TEXT;
        request.state.mode=2;request.bytes=0;request.state.count=0;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        deadline=GetTickCount64()+5000;
        while(IsWindow(window) && GetTickCount64()<deadline)Sleep(10);
        CHECK(!IsWindow(window));
        if(!round) {
            INPUT_RECORD records[64];DWORD count,i,keys=0;
            CHECK(!run16_native_frontend_dos_enter(test_frontend,channel));
            CHECK(!run16_native_frontend_dos_read(test_frontend,FALSE,records,64,&count));
            run16_native_frontend_dos_leave(test_frontend);
            for(i=0;i<count;++i)if(records[i].EventType==KEY_EVENT) {
                CHECK(records[i].Event.KeyEvent.wVirtualKeyCode=='B');
                CHECK(records[i].Event.KeyEvent.bKeyDown==(keys==0));++keys;
            }
            CHECK(keys==2);
            puts("PASS Window retirement releases held DOS B before returning to Console");
        }
        if(!round)puts("PASS production channel complete graphics opens Window; partial/static frames and explicit TEXT retain correct mode");
        /* TEXT restored canonical output; fixture diagnostics above therefore
         * changed its first cell. Re-establish the next case's marker. */
        { DWORD written;COORD origin={0,0};
          CHECK(WriteConsoleOutputCharacterW(channel->console.output,L"K",1,origin,&written) && written==1); }
        request.sequence=6;request.operation=CONSOLE_IO_VIDEO_BEGIN;
        request.state.mode=3;request.bytes=sizeof(description);
        memcpy(request.data,&description,sizeof(description));
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+request.bytes);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        request.sequence=7;request.operation=CONSOLE_IO_VIDEO_DATA;request.bytes=8;
        memset(request.data,1,8);
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data)+8);
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));CHECK(reply.result);
        deadline=GetTickCount64()+5000;
        while(!FindWindowW(NULL,L"NTVDM") && GetTickCount64()<deadline)Sleep(10);
        CHECK(FindWindowW(NULL,L"NTVDM"));
    }
    if(mode==1) {
        if(!round)verify_dos_input_queue(channel);
        request.sequence=2;request.operation=CONSOLE_IO_READ_INPUT;
        request.state.input=1;request.state.count=1;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        CHECK(WaitForSingleObject(read_entered,5000)==WAIT_OBJECT_0);
        /* Production binding holds the I/O lock: an empty read must return
         * immediately, otherwise native handoff/teardown would deadlock. */
        peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
        CHECK(reply.result && !reply.state.count && !reply.bytes && reply.sequence==2);
        CHECK(WaitForSingleObject(thread,0)==WAIT_TIMEOUT);
    } else if(mode==2) {
        /* Real channel callback reads frontend policy under the I/O lock.
         * A request alone must not fabricate a DOS frame or open a Window. */
        unsigned selection;
        request.sequence=2;request.operation=CONSOLE_IO_WINDOW_QUERY;
        request.state.mode=CONSOLE_WINDOW_TEXT_FRAME_REQUIRED;
        for(selection=0;selection<3;++selection) {
            BOOL wanted=selection==1;
            ULONGLONG deadline=GetTickCount64()+5000;
            if(wanted) {
                INPUT_RECORD keys[4]={0};console_io_input received[4];DWORD written;
                unsigned key;
                for(key=0;key<4;++key) {
                    keys[key].EventType=KEY_EVENT;
                    keys[key].Event.KeyEvent.bKeyDown=key!=2;
                    keys[key].Event.KeyEvent.wRepeatCount=1;
                    keys[key].Event.KeyEvent.wVirtualKeyCode=key==3 ? 'X' : 'F';
                    keys[key].Event.KeyEvent.wVirtualScanCode=key==3 ? 0x2d : 0x21;
                    keys[key].Event.KeyEvent.uChar.UnicodeChar=key==3 ? L'x' : 0;
                    keys[key].Event.KeyEvent.dwControlKeyState=key==3 ? 0 : LEFT_CTRL_PRESSED|LEFT_ALT_PRESSED;
                }
                CHECK(WriteConsoleInputW(channel->console.input,keys,4,&written) && written==4);
                CHECK(WaitForSingleObject(run16_native_frontend_dos_ready(test_frontend),5000)==WAIT_OBJECT_0);
                request.operation=CONSOLE_IO_PEEK_INPUT;request.state.mode=0;request.state.count=4;
                peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
                peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
                if(!reply.result || reply.state.count!=1 || reply.bytes!=sizeof(console_io_input))
                    printf("CAF peek round=%u result=%lu count=%lu bytes=%lu status=%lu\n",
                        round,(DWORD)reply.result,(DWORD)reply.state.count,(DWORD)reply.bytes,(DWORD)reply.error);
                CHECK(reply.result && reply.state.count==1 && reply.bytes==sizeof(console_io_input));
                peer_io(FALSE,received,reply.bytes);++request.sequence;
                CHECK(received[0].virtual_key=='X'); /* Peek does not consume. */
                request.operation=CONSOLE_IO_READ_INPUT;
                peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
                peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
                CHECK(reply.result && reply.state.count==1 && reply.bytes==sizeof(console_io_input));
                peer_io(FALSE,received,reply.bytes);++request.sequence;
                CHECK(received[0].virtual_key=='X' && received[0].character=='x');
                request.operation=CONSOLE_IO_WINDOW_QUERY;request.state.count=0;
                request.state.mode=CONSOLE_WINDOW_TEXT_FRAME_REQUIRED;
            } else CHECK(!run16_native_frontend_display(test_frontend,FALSE));
            do {
                peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
                peer_io(FALSE,&reply,(DWORD)offsetof(console_io_reply,data));
                CHECK(reply.result && !reply.bytes);++request.sequence;
                if(reply.state.left==wanted)break;
                Sleep(10);
            } while(GetTickCount64()<deadline);
            CHECK(reply.state.left==wanted && !FindWindowW(NULL,L"NTVDM"));
        }
        /* Acknowledged normal output followed by peer EOF races owner stop. */
        CHECK(CloseHandle(peer));peer=NULL;
    } else if(mode==3) {
        request.sequence=2;request.bytes=CONSOLE_IO_DATA_BYTES+1;
        peer_io(TRUE,&request,(DWORD)offsetof(console_io_request,data));
        CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(thread,&exit_code) && exit_code==ERROR_INVALID_DATA);
        CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    } else if(mode==4) {
        CHECK(TerminateProcess(child.hProcess,91));
        CHECK(WaitForSingleObject(child.hProcess,5000)==WAIT_OBJECT_0);
        CHECK(WaitForSingleObject(thread,5000)==WAIT_OBJECT_0);
        CHECK(GetExitCodeThread(thread,&exit_code) && exit_code==ERROR_PROCESS_ABORTED);
        CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    }
    if(mode>=3) {
        OVERLAPPED io={0};BYTE byte;DWORD done=0,error;
        io.hEvent=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(io.hEvent);
        CHECK(!ReadFile(peer,&byte,1,&done,&io));error=GetLastError();
        if(error==ERROR_IO_PENDING) {
            CHECK(WaitForSingleObject(io.hEvent,5000)==WAIT_OBJECT_0);
            CHECK(!GetOverlappedResult(peer,&io,&done,FALSE));error=GetLastError();
        }
        CHECK(error==ERROR_BROKEN_PIPE && done==0);
        CloseHandle(io.hEvent);
    }
    started=GetTickCount64();
    run16_console_channel_stop(channel);
    CHECK(GetTickCount64()-started<5000);
    if(mode==0 && round<4)CHECK(!FindWindowW(NULL,L"NTVDM"));
    CHECK(WaitForSingleObject(thread,0)==WAIT_OBJECT_0);
    CHECK(GetExitCodeThread(thread,&exit_code) && exit_code!=STILL_ACTIVE && exit_code!=0);
    CHECK(WaitForSingleObject(ready,0)==WAIT_OBJECT_0);
    CloseHandle(thread);CloseHandle(ready);
    if(child.hProcess) {CloseHandle(child.hThread);CloseHandle(child.hProcess);}
    if(peer) {CloseHandle(peer);peer=NULL;}
    if(mode==4) { DWORD handles,written;COORD origin={0,0};
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&handles));
        printf("completed graphics/channel round=%u handles=%lu tick=%llu\n",round,handles,GetTickCount64());
        CHECK(WriteConsoleOutputCharacterW(GetStdHandle(STD_OUTPUT_HANDLE),L"K",1,origin,&written)); }
}
static void wait_initial_window_resources(void)
{
    DWORD previous,current,quiet=0;ULONGLONG deadline=GetTickCount64()+10000;
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&previous));
    /* The first Window starts OS text-input initialization after it closes.
     * No channels run during this bounded warm-up. Subsequent Window/channel
     * cycles still require exact handle equality, without a leak allowance. */
    do {
        Sleep(100);CHECK(GetProcessHandleCount(GetCurrentProcess(),&current));
        quiet=current==previous ? quiet+1 : 0;previous=current;
        if(quiet==20)return;
    } while(GetTickCount64()<deadline);
    CHECK(FALSE);
}
int main(int argc,char **argv)
{
    unsigned round,mode;DWORD before,after;
    HANDLE input,canonical,alternate;DWORD count;COORD origin={0,0};
    if(argc==2 && !strcmp(argv[1],"--worker-wait")) {Sleep(INFINITE);return 0;}
    read_entered=CreateEventW(NULL,TRUE,FALSE,NULL);CHECK(read_entered);
    CHECK(!run16_native_frontend_create(&test_frontend));
    CHECK(!run16_native_frontend_console(test_frontend,&input,&canonical));
    CHECK(WriteConsoleOutputCharacterW(canonical,L"K",1,origin,&count) && count==1);
    alternate=CreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
    CHECK(alternate!=INVALID_HANDLE_VALUE);
    CHECK(WriteConsoleOutputCharacterW(alternate,L"A",1,origin,&count) && count==1);
    CHECK(SetConsoleActiveScreenBuffer(alternate));
    /* Warm up lazy runtime/Console resources before counting owned handles. */
    for(mode=0;mode<5;++mode) run_case(mode,0);
    if(argc==2 && !strcmp(argv[1],"--handle-audit")) {
        HANDLE modules;MODULEENTRY32W module={sizeof(module)};
        audit_handles("before-idle");Sleep(3000);audit_handles("after-idle-no-channels");
        modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,GetCurrentProcessId());
        CHECK(modules!=INVALID_HANDLE_VALUE && Module32FirstW(modules,&module));
        do { printf("MODULE %ls\n",module.szExePath); } while(Module32NextW(modules,&module));
        CloseHandle(modules);
        CHECK(WriteConsoleOutputCharacterW(canonical,L"K",1,origin,&count) && count==1);
    }
    wait_initial_window_resources();
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(round=1;round<=16;++round)
        for(mode=0;mode<5;++mode) run_case(mode,round);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
    printf("channel handles before=%lu after=%lu\n",before,after);
    CHECK(after==before);
    {
        ULONGLONG began=GetTickCount64();
        run16_native_frontend_cancel(test_frontend);
        CHECK(run16_native_frontend_dos_bind(test_frontend,&before,TRUE)==ERROR_OPERATION_ABORTED);
        CHECK(GetTickCount64()-began<1000);
    }
    run16_native_frontend_destroy(test_frontend);
    CHECK(SetConsoleActiveScreenBuffer(canonical));
    CloseHandle(alternate);CloseHandle(canonical);CloseHandle(input);
    CloseHandle(read_entered);
    puts("PASS 85 real channel lifetimes: pipe cancel, nonblocking empty input, barrier/EOF/stop race, oversized request, real process death; joined threads, EOF readiness, no handle growth");
    puts("PASS four graphics channels each create/retire two Windows, including active graphics disposal; exact post-warm-up handle equality");
    puts("PASS all nested channels retain canonical K while active surface is A; private handle copies survive frontend teardown");
    puts("PASS stopped presentation owner rejects handoff without waiting for execution or restarting a helper");
    return 0;
}
